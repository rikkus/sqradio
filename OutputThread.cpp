// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include <qapplication.h>

#include "OutputThread.h"
#include "OutputDriver.h"
#include "OutputDriverFactory.h"
#include "ByteArray.h"
#include "QThreadPipe.h"
#include "KillMeEvent.h"

namespace SqRadio
{
  class OutputThread::Private
  {
    public:

      OutputDriver  * outputDriver;

      QThreadPipe   * pipe;
      QObject       * parent;
  };

  OutputThread::OutputThread(QThreadPipe * pipe, QObject * parent)
    : QThread ()
  {
    d = new Private;

    d->outputDriver = 0;
    d->pipe         = pipe;
    d->parent       = parent;

    openDevice(44100, 2);
  }

  OutputThread::~OutputThread()
  {
    delete d;
  }

  void OutputThread::run()
  {
    if (0 == d->outputDriver)
    {
      qDebug("OutputThread::run: No output driver. Stopping.");
      return;
    }

    const unsigned int MaxBufferSize = 1024 * 1024;

    QByteArray buffer;

    while (true)
    {
      Q_ULONG bytesAvailable  = d->pipe->maxBytesReadableFromMainThread();
      Q_ULONG bytesFree       = MaxBufferSize - buffer.size();

      Q_ULONG bytesToRead = QMIN(bytesAvailable, bytesFree);

      append(d->pipe->readFromMainThread(bytesToRead), buffer);

      Q_ULONG bytesToWrite = QMIN(8192, buffer.size());

      d->outputDriver->write(buffer.data(), bytesToWrite);

      cut(buffer, bytesToWrite);
    }

    qApp->postEvent(d->parent, new KillMeEvent(this));
  }

  void OutputThread::closeDevice()
  {
    if (0 != d->outputDriver)
      d->outputDriver->close();

    d->outputDriver = 0;
  }

  bool OutputThread::openDevice(int sampleRate, int channels)
  {
    PCM::Format pcmFormat
      (
        channels,                   // Channel count.
        2,                          // Bytes per sample.
        PCM::Format::Signed,        // Signedness.
        PCM::Format::LittleEndian,  // Endianness.
        sampleRate                  // Hz.
      );

    d->outputDriver = OutputDriverFactory::create("oss");

    if (0 == d->outputDriver)
    {
      qDebug("Can't create output driver");
      return false;
    }

    bool ok = d->outputDriver->open(pcmFormat);

    if (!ok)
    {
      qDebug("OutputThread::openDevice: Can't open device.");
    }

    d->outputDriver->setThread(this);

    return ok;
  }

  void OutputThread::publicUSleep(unsigned int us)
  {
    usleep(us);
  }
}

