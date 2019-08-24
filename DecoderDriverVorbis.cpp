// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

extern "C"
{
#ifdef SQRADIO_STATIC_BUILD
#include "vorbis/vorbisfile.h"
#else
#include <vorbis/vorbisfile.h>
#endif
}

#include <sys/types.h>
#include <sys/socket.h>

#include <qcstring.h>
#include <qapplication.h>

#include "DecoderDriverVorbis.h"
#include "QThreadPipe.h"

  size_t
vorbis_read(void * ptr, size_t size, size_t n, void * stream)
{
  QThreadPipe * pipe = reinterpret_cast<QThreadPipe *>(stream);

  QByteArray a = pipe->readFromMainThread(size * n);

  memcpy(ptr, a.data(), a.size());

  return a.size();
}

  int
vorbis_seek(void * /* stream */, int64_t /* offset */, int /* whence */)
{
  return -1;
}

  int
vorbis_close(void * /* stream */)
{
  return 1;
}

  long
vorbis_tell(void * /* stream */)
{
  return -1;
}

namespace SqRadio
{
  class DecoderDriverVorbis::Private
  {
    public:

      int dummy;
  };

  DecoderDriverVorbis::DecoderDriverVorbis()
  {
    d = new Private;
  }

  DecoderDriverVorbis::~DecoderDriverVorbis()
  {
    delete d;
  }

  void DecoderDriverVorbis::run(QThreadPipe * pipe)
  {
    OggVorbis_File vorbisFile;

    memset(&vorbisFile, 0, sizeof(OggVorbis_File));

    ov_callbacks callbacks;

    callbacks.read_func		= vorbis_read;
    callbacks.seek_func   = vorbis_seek;
    callbacks.close_func  = vorbis_close;
    callbacks.tell_func   = vorbis_tell;

    if (ov_open_callbacks(pipe, &vorbisFile, NULL, 0, callbacks) < 0)
    {
      qWarning("Could not ov_open_callbacks");
      return;
    }

    // FIXME
#if 0
    if (0 != d->parent)
    {
      char ** ptr = ov_comment(&vorbisFile, -1)->user_comments;

      QString artist, album, title;

      while (0 != *ptr)
      {
        if (0 == qstrnicmp(*ptr, "ARTIST=", 7))
          artist = QString::fromUtf8(*ptr + 7);

        if (0 == qstrnicmp(*ptr, "ALBUM=", 6))
          album	= QString::fromUtf8(*ptr + 6);

        if (0 == qstrnicmp(*ptr, "TITLE=", 6))
          title = QString::fromUtf8(*ptr + 6);

        ++ptr;
      }

      QApplication::postEvent
        (d->parent, new VorbisInfoEvent(artist, album, title));
    }
#endif

    QByteArray out;

    int bitstream = 0;
    int bytesRead = 0;

    while (true)
    {
      out.resize(32768);

      bytesRead =
        ::ov_read(&vorbisFile, out.data(), out.size(), 0, 2, 1, &bitstream);

      if (0 == bytesRead)
        return;

      if (bytesRead < 0)
      {
        qDebug("Error decoding vorbis");
        return;
      }

      out.resize(bytesRead);

      if (!pipe->writeToMainThread(out))
        return;
    }
  }
}

