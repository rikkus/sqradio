// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifdef SQRADIO_USE_DRIVER_ESD

#include <unistd.h>

#include "esd/esd.h"

#include "OutputDriverESD.h"

namespace SqRadio
{
  class OutputDriverESD::Private
  {
    public:

      int fd;
  };

  OutputDriverESD::OutputDriverESD()
    : OutputDriver()
  {
    d = new Private;

    d->fd = -1;
  }

  OutputDriverESD::~OutputDriverESD()
  {
    delete d;
  }

  bool OutputDriverESD::open(PCM::Format format)
  {
    if (format.bytesPerSample > 2)
    {
      qDebug("Don't do %d bytes per sample", format.bytesPerSample);
      return false;
    }

    if (format.channelCount > 2)
    {
      qDebug("Don't do more than 2 channels");
      return false;
    }

    int byteFormat;

    switch (format.bytesPerSample)
    {
      case 1:
        byteFormat = ESD_BITS8;
        break;

      case 2:
        byteFormat = ESD_BITS16;
        break;

      default:
        qDebug("Error: should have caught incorrect bytes per sample");
        return false;
        break;
    }

    int channels = (2 == format.channelCount) ? ESD_STEREO : ESD_MONO;

    int esdFormat = byteFormat | channels | ESD_STREAM | ESD_PLAY;

    d->fd = esd_play_stream(esdFormat, format.rate, 0, "sqradio");

    if (0 == d->fd)
    {
      d->fd = -1;
      qDebug("Can't talk to ESD");
      return false;
    }

    return true;
  }

  bool OutputDriverESD::write(const char * data, unsigned int byteCount)
  {
    return (::write(d->fd, data, byteCount) >= 0);
  }

  void OutputDriverESD::close()
  {
    if (-1 != d->fd)
    {
      ::close(d->fd);
      d->fd = -1;
    }
  }

  bool OutputDriverESD::isOpen() const
  {
    return (-1 != d->fd);
  }
}

#endif // SQRADIO_USE_DRIVER_ESD
