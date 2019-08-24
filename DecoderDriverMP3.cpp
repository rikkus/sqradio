// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

extern "C"
{
#ifdef SQRADIO_STATIC_BUILD
#include "ffmp3/avcodec.h"
#else
#include <ffmpeg/avcodec.h>
#endif
}

#include <qcstring.h>
#include <qapplication.h>

#include "DecoderDriverMP3.h"
#include "QThreadPipe.h"

namespace SqRadio
{
  class DecoderDriverMP3::Private
  {
    public:

      AVCodec * ffmp3Codec;

      bool checkedForID3Tag;
  };

  DecoderDriverMP3::DecoderDriverMP3()
  {
    d = new Private;

    d->checkedForID3Tag = false;

    avcodec_register_all();

    d->ffmp3Codec = avcodec_find_decoder(CODEC_ID_MP3LAME);

    if (0 == d->ffmp3Codec)
    {
      qDebug("DecoderDriverMP3 ctor: Can't find MP3 decoder in ffmpeg");
      return;
    }
  }

  DecoderDriverMP3::~DecoderDriverMP3()
  {
    delete d;
  }

  void DecoderDriverMP3::run(QThreadPipe * pipe)
  {
    if (0 == d->ffmp3Codec)
    {
      qDebug("No codec");
      return;
    }

    AVCodecContext  * ffmp3Context;

    ffmp3Context = avcodec_alloc_context();

    ffmp3Context->bit_rate     = 0;
    ffmp3Context->channels     = 2;
    ffmp3Context->sample_rate  = 44100;

    if (avcodec_open(ffmp3Context, d->ffmp3Codec) < 0)
    {
      qDebug("Can't open decoder");
      avcodec_close(ffmp3Context);
      return;
    }

    QByteArray pcmBuffer(AVCODEC_MAX_AUDIO_FRAME_SIZE / 2);

    while (true)
    {
      QByteArray mp3Buffer = pipe->readFromMainThread(4096);

      Q_ULONG bytesRead = mp3Buffer.size();

      if (0 == bytesRead)
      {
        avcodec_close(ffmp3Context);
        return;
      }

      int bytesDecoded = 0;

      char * pcmPtr = pcmBuffer.data();
      char * mp3Buf = mp3Buffer.data();

#if 0
      if (!d->checkedForID3Tag)
      {
        if (bytesDecoded < 3)
          continue;

        if (0 == memcmp(mp3Buf, "ID3", 3))
        {
          qDebug("Found ID3 tag at start of data");

          // Fall through. Not sure how to handle this best, yet.
        }

        d->checkedForID3Tag = true;
      }
#endif

      while (bytesRead > 0)
      {
        int ret =
          avcodec_decode_audio
          (
            ffmp3Context,
            (INT16 *)pcmPtr,
            &bytesDecoded,
            (UINT8 *)mp3Buf,
            bytesRead
          );

        if (ret < 0)
        {
          qDebug("Error while decoding mp3 (avcodec_decode_audio)");
        }

        mp3Buf    += ret;
        bytesRead -= ret;

        if (bytesDecoded > 0)
        {
          if (!pipe->writeToMainThread(pcmBuffer.data(), bytesDecoded))
          {
            avcodec_close(ffmp3Context);
            return;
          }
        }
      }
    }
  }
}

