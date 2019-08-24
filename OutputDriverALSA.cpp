// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifdef SQRADIO_USE_DRIVER_ALSA

#include <alsa/asoundlib.h>

#include <qcstring.h>
#include "OutputDriverALSA.h"

namespace SqRadio
{
  class OutputDriverALSA::Private
  {
    public:

      snd_pcm_t   * pcmHandle;
      QString       deviceName;
      PCM::Format   format;
      unsigned int  bufferSize;
      unsigned int  periods;
  };

  OutputDriverALSA::OutputDriverALSA()
    : OutputDriver()
  {
    d = new Private;

    d->pcmHandle  = 0;
    d->bufferSize = 1024;
    d->periods    = 4;
    d->deviceName = "default";
  }

  OutputDriverALSA::~OutputDriverALSA()
  {
    delete d;
  }

  bool OutputDriverALSA::open(PCM::Format format)
  {
    close();

    d->format = format;

    if (format.bytesPerSample > 3)
    {
      qDebug("Don't do %d bytes per sample", format.bytesPerSample);
      return false;
    }

    if (format.channelCount > 2)
    {
      qDebug("Don't do more than 2 channels");
      return false;
    }

    int err =
      snd_pcm_open
      (
        &d->pcmHandle,
        d->deviceName.latin1(),
        SND_PCM_STREAM_PLAYBACK,
        0
      );

    if (err < 0)
      return alsaError("snd_pcm_open", err);

    snd_pcm_hw_params_t * hardwareParameters = 0;

    snd_pcm_hw_params_alloca(&hardwareParameters);

    err = snd_pcm_hw_params_any(d->pcmHandle, hardwareParameters);

    if (err < 0)
      return alsaError("snd_pcm_hw_params_any", err);

    err = snd_pcm_hw_params_set_access
      (
        d->pcmHandle,
        hardwareParameters,
        SND_PCM_ACCESS_RW_INTERLEAVED
      );

    if (err < 0)
      return alsaError("snd_pcm_hw_params_set_access", err);

    snd_pcm_format_t outputFormat;

    switch (format.bytesPerSample)
    {
      case 1:
        outputFormat = SND_PCM_FORMAT_S8;
        break;

      case 2:
        if (PCM::Format::BigEndian == format.byteOrder)
        {
          outputFormat = SND_PCM_FORMAT_S16_BE;
        }
        else
        {
          outputFormat = SND_PCM_FORMAT_S16_LE;
        }
        break;

      case 3:
        if (PCM::Format::BigEndian == format.byteOrder)
        {
          outputFormat = SND_PCM_FORMAT_S24_BE;
        }
        else
        {
          outputFormat = SND_PCM_FORMAT_S24_LE;
        }
        break;

      default:

        qDebug
          (
            "Internal bug: should have caught too-large bytes per"
            "sample earlier"
          );

        return false;
        break;
    }

    err = snd_pcm_hw_params_set_format
      (
        d->pcmHandle,
        hardwareParameters,
        outputFormat
      );

    if (err < 0)
      return alsaError("snd_pcm_hw_params_set_format", err);

    err = snd_pcm_hw_params_set_channels
      (
        d->pcmHandle,
        hardwareParameters,
        format.channelCount
      );

    if (err < 0)
      return alsaError("snd_pcm_hw_params_set_channels", err);

    err = snd_pcm_hw_params_set_rate_near
      (
        d->pcmHandle,
        hardwareParameters,
        format.rate,
        0
      );

    if (err < 0)
      return alsaError("snd_pcm_hw_params_set_rate_near", err);

    err = snd_pcm_hw_params_set_period_size
      (
        d->pcmHandle,
        hardwareParameters,
        d->bufferSize / (format.bytesPerSample * format.channelCount),
        0
      );

    if (err < 0)
      return alsaError("snd_pcm_hw_params_set_period_size", err);

    err = snd_pcm_hw_params_set_periods
      (
        d->pcmHandle,
        hardwareParameters,
        d->periods * format.channelCount,
        0
      );

    if (err < 0)
      return alsaError("snd_pcm_hw_params_set_periods", err);

    err = snd_pcm_hw_params
      (
        d->pcmHandle,
        hardwareParameters
      );

    if (err < 0)
      return alsaError("snd_pcm_hw_params", err);

    err = snd_pcm_prepare(d->pcmHandle);

    if (err < 0)
      return alsaError("snd_pcm_prepare", err);

    return true;
  }

  bool OutputDriverALSA::write(const char * data, unsigned int byteCount)
  {
    char * buffer = const_cast<char *>(data);

    unsigned int framesToWrite =
      byteCount / (d->format.bytesPerSample * d->format.channelCount);

    int result;

    do
    {
      result = snd_pcm_writei(d->pcmHandle, buffer, framesToWrite);

      if (result > 0)
      {
        framesToWrite  -= result;
        buffer          += result;
      }
    }
    while (framesToWrite > 0 && (result > 0 || -EAGAIN == result));

    if (-EPIPE == result)
    {
      // Underrun. Reset stream.

      snd_pcm_prepare(d->pcmHandle);

      result = snd_pcm_writei(d->pcmHandle, buffer, framesToWrite);

      if (result != int(framesToWrite))
      {
        qDebug("ALSA error: snd_pcm_write: %s", snd_strerror(result));
        return false;
      }
    }

    return true;
  }

  void OutputDriverALSA::close()
  {
    if (0 != d->pcmHandle)
    {
      snd_pcm_close(d->pcmHandle);
      d->pcmHandle = 0;
    }
  }

  bool OutputDriverALSA::alsaError
    (
      const char *  functionName,
      int           alsaErrorNumber
    )
  {
    qDebug("ALSA error: %s: %s", functionName, snd_strerror(alsaErrorNumber));
    snd_pcm_close(d->pcmHandle);
    d->pcmHandle = 0;
    return false;
  }

  bool OutputDriverALSA::isOpen() const
  {
    return (0 != d->pcmHandle);
  }
}

#endif // SQRADIO_USE_DRIVER_ALSA
