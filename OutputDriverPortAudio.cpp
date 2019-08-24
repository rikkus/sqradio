// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifdef SQRADIO_USE_DRIVER_PORT_AUDIO

#ifdef WIN32
#include "windows.h" // For Sleep().
#define SLEEP(a) Sleep(a)
#else
#include <unistd.h>
#define SLEEP(a) sleep(a)
#endif

#include "portaudio.h"
#include "RingBuffer.h"
#include "OutputDriverPortAudio.h"

namespace
{
  const unsigned int HighWater  = 96 * 4608;
  const unsigned int BufferSize = 128 * 4608;
}

static int portAudioCallback
(
  void        * /* inputBuffer */,
  void        * outputBuffer,
  unsigned long framesPerBuffer,
  PaTimestamp   /* outTime */,
  void        * userData
)
{
  qDebug("portAudioCallback");

  using namespace SqRadio;

  OutputDriverPortAudio * driver =
    reinterpret_cast<OutputDriverPortAudio *>(userData);

  PCM::Format format = driver->format();

  unsigned long byteCount =
    framesPerBuffer * format.bytesPerSample * format.channelCount;

  RingBuffer * ringBuffer = driver->ringBuffer();

  qDebug
    (
      "portAudioCallback:: ringbuffer used: %ld",
      ringBuffer->used()
    );

  qDebug
    (
      "portAudioCallback:: ringbuffer available: %ld",
      ringBuffer->available()
    );

  if (ringBuffer->used() < byteCount)
  {
    qDebug("portAudioCallback: underrun");
    memset((char *)outputBuffer, 0, byteCount);
    return 0;
  }

  qDebug("portAudioCallback: Writing %ld bytes to outputBuffer", byteCount);

  if (!ringBuffer->read((char *)outputBuffer, byteCount))
  {
    qDebug("portAudioCallback: ringBuffer->read failed");
    memset((char *)outputBuffer, 0, byteCount);
    return 0;
  }

  return 0;
}

namespace SqRadio
{
  class OutputDriverPortAudio::Private
  {
    public:

      PortAudioStream * stream;
      RingBuffer      * ringBuffer;
      bool              started;
      PCM::Format       format;
  };

  OutputDriverPortAudio::OutputDriverPortAudio()
    : OutputDriver()
  {
    d = new Private;

    d->stream   = 0;
    d->started  = false;

    d->ringBuffer = new RingBuffer(BufferSize);

    if (0 == d->ringBuffer)
    {
      qDebug("OutputDriverPortAudio::ctor: Out of memory");
      return;
    }

    int err = Pa_Initialize();

    if (paNoError != err)
    {
      qDebug("PortAudio error: %s", Pa_GetErrorText(err));
      return;
    }

    qDebug("PortAudio initialised");
  }

  OutputDriverPortAudio::~OutputDriverPortAudio()
  {
    close();
    delete d->ringBuffer;
    delete d;
  }

  bool OutputDriverPortAudio::open(PCM::Format format)
  {
    qDebug("Doing Pa_OpenDefaultStream");

    int err = Pa_OpenDefaultStream
      (
        &d->stream,
        0,
        format.channelCount,
        paInt16,
        format.rate,
        256,
        0,
        portAudioCallback,
        this
      );

    if (paNoError != err)
    {
      qDebug
        (
          "OutputDriverPortAudio::open(): PortAudio error: %s",
          Pa_GetErrorText(err)
        );

      return false;
    }

    d->format = format;

    return true;
  }

  bool OutputDriverPortAudio::write(const char * data, unsigned int byteCount)
  {
    if (0 == d->stream)
    {
      qDebug("OutputDriverPortAudio::write(): Not open");
      return false;
    }

    const char * dataCursor = data;

    while (byteCount > 0)
    {
      qDebug
        (
          "OutputDriverPortAudio::write(): ringbuffer used: %ld",
          d->ringBuffer->used()
        );

      qDebug
        (
          "OutputDriverPortAudio::write(): ringbuffer available: %ld",
          d->ringBuffer->available()
        );


      unsigned long bytesToWrite = QMIN(byteCount, d->ringBuffer->available());

      if (0 == bytesToWrite)
      {
      }
      //  Pa_Sleep(10);

      else
      {
        qDebug
          (
            "OutputDriverPortAudio::write(): Writing %ld to ringbuffer",
            bytesToWrite
          );

        d->ringBuffer->write(dataCursor, bytesToWrite);

        byteCount   -= bytesToWrite;
        dataCursor  += bytesToWrite;

        if (!d->started && d->ringBuffer->used() >= HighWater)
        {
          qDebug("OutputDriverPortAudio::write(): Not started and we're over HighWater now, so starting");

          int err = Pa_StartStream(d->stream);

          if (paNoError != err)
          {
            qDebug
              (
                "OutputDriverPortAudio::write(): PortAudio error: %s",
                Pa_GetErrorText(err)
              );

            return false;
          }

          d->started = true;
        }
      }
    }


    return true;
  }

  void OutputDriverPortAudio::close()
  {
    if (0 != d->stream)
    {
      int err = Pa_CloseStream(d->stream);

      if (paNoError != err)
      {
        qDebug
          (
            "OutputDriverPortAudio::close(): PortAudio error: %s",
            Pa_GetErrorText(err)
          );
      }
    }
  }

  bool OutputDriverPortAudio::isOpen() const
  {
    return (0 != d->stream);
  }


  RingBuffer * OutputDriverPortAudio::ringBuffer()
  {
    return d->ringBuffer;
  }

  PCM::Format OutputDriverPortAudio::format() const
  {
    return d->format;
  }

  void OutputDriverPortAudio::underrun()
  {
    d->ringBuffer->clear();
    d->started = false;
  }
}

#endif // SQRADIO_USE_DRIVER_PORT_AUDIO
