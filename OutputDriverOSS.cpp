// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifdef SQRADIO_USE_DRIVER_OSS

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#if defined(__OpenBSD__) || defined(__NetBSD__)
#include <soundcard.h>
#else
#include <sys/soundcard.h>
#endif // OpenBSD || NetBSD
#include <sys/ioctl.h>

#include <qcstring.h>
#include "OutputDriverOSS.h"

namespace SqRadio
{
  class OutputDriverOSS::Private
  {
    public:

      int           fd;
      PCM::Format   format;
  };

  OutputDriverOSS::OutputDriverOSS()
    : OutputDriver()
  {
    d = new Private;

    d->fd = -1;
  }

  OutputDriverOSS::~OutputDriverOSS()
  {
    delete d;
  }

  bool OutputDriverOSS::open(PCM::Format format)
  {
    close();

    d->format = format;

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

    d->fd = ::open("/dev/dsp", O_WRONLY);

    if (d->fd < 0)
    {
      qDebug("Can't open /dev/dsp for writing");
      return false;
    }

    int stereo = (format.channelCount == 2 ? 1 : 0);

    if (ioctl(d->fd, SNDCTL_DSP_STEREO, &stereo) < 0)
    {
      qDebug("Can't set channel count to %d", format.channelCount);
      ::close(d->fd);
      return false;
    }

    int byteFormat;

    switch (format.bytesPerSample)
    {
      case 1:
        if (PCM::Format::Signed == format.signedness)
          byteFormat = AFMT_S8;
        else
          byteFormat = AFMT_U8;
        break;

      case 2:
        if (PCM::Format::BigEndian == format.byteOrder)
          if (PCM::Format::Signed == format.signedness)
            byteFormat = AFMT_S16_BE;
          else
            byteFormat = AFMT_U16_BE;
        else
          if (PCM::Format::Signed == format.signedness)
            byteFormat = AFMT_S16_LE;
          else
            byteFormat = AFMT_U16_LE;
        break;

      default:
        qDebug("Error: should have caught incorrect bytes per sample");
        ::close(d->fd);
        return false;
        break;
    }

    if (ioctl(d->fd, SNDCTL_DSP_SAMPLESIZE, &byteFormat) < 0)
    {
      qDebug("Can't set sample size to %d", format.bytesPerSample);
      ::close(d->fd);
      return false;
    }

    int rate = format.rate;

    int ret = ioctl(d->fd, SNDCTL_DSP_SPEED, &rate);

    if (ret < 0 || rate < 0.99 * format.rate || rate > 1.01 * format.rate)
    {
      qDebug("Can't set rate to %d", format.rate);
      ::close(d->fd);
      return false;
    }

    return true;
  }

  bool OutputDriverOSS::write(const char * data, unsigned int byteCount)
  {
    return (::write(d->fd, data, byteCount) >= 0);
  }

  void OutputDriverOSS::close()
  {
    if (0 != d->fd)
      ::close(d->fd);

    d->fd = -1;
  }

  bool OutputDriverOSS::isOpen() const
  {
    return (-1 != d->fd);
  }
}

#endif // SQRADIO_USE_DRIVER_OSS
