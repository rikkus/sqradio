// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include <qdir.h>

#include "Output.h"
#include "OutputThread.h"
#include "Utils.h"
#include "ByteArray.h"
#include "QThreadPipe.h"
#include "KillMeEvent.h"

namespace SqRadio
{
  class Output::Private
  {
    public:

      OutputThread      * thread;
      QThreadPipe       * pipe;
  };

  Output::Output(QObject * parent)
    : QObject(parent)
  {
    d = new Private;

    d->pipe = new QThreadPipe(128 * 1024, 64 * 1024, this);

    d->thread = new OutputThread(d->pipe, this);

    connect
      (
        d->pipe,
        SIGNAL(canReadFromSubThread()),
        SLOT(slotReadyRead())
      );

    connect
      (
        d->pipe,
        SIGNAL(canWriteToSubThread()),
        SLOT(slotReadyWrite())
      );

    pause();
    d->thread->start();
  }

  Output::~Output()
  {
    delete d;
  }

  bool Output::play(const QByteArray & buf)
  {
    if (0 == writeBufferAvailable())
    {
      qDebug("Output::play: No buffer available");
      return false;
    }

    if (!d->pipe->writeToSubThread(buf))
    {
      qDebug("Output::play: Can't write to subthread");
      return false;
    }

    emit(bufferPercent(_bufferPercent()));

    checkIfReadyAndEmit();

    return true;
  }

  Q_ULONG Output::bytesAvailable() const
  {
    return d->pipe->maxBytesWritableToSubThread();
  }

  void Output::slotReadyRead()
  {
    emit(readyRead());
  }

  void Output::slotReadyWrite()
  {
    emit(bufferPercent(_bufferPercent()));
    checkIfReadyAndEmit();
  }

  Q_ULONG Output::bufferFill() const
  {
    return bufferSize() - writeBufferAvailable();
  }

  Q_ULONG Output::bufferSize() const
  {
    return d->pipe->toSubThreadBufferSize();
  }

  Q_ULONG Output::writeBufferAvailable() const
  {
    return d->pipe->maxBytesWritableToSubThread();
  }

  void Output::checkIfReadyAndEmit()
  {
    if (0 != writeBufferAvailable())
      emit(readyForData());
  }

  QByteArray Output::readBlock(unsigned int maxBytes)
  {
    if (0 == d->pipe)
      return QByteArray();

    maxBytes = QMIN(maxBytes, bytesAvailable());

    return d->pipe->readFromSubThread(maxBytes);
  }

#ifndef SQRADIO_EMBEDDED
  bool Output::event(QEvent * e)
  {
    switch (e->type())
    {
      case 8060:
        delete static_cast<KillMeEvent *>(e)->thread();
        return true;
        break;

      default:
        return QObject::event(e);
        break;
    }
  }
#endif // SQRADIO_EMBEDDED


  unsigned int Output::_bufferPercent() const
  {
    Q_ULONG bytesUsed =
      d->pipe->toSubThreadBufferSize()
      -
      d->pipe->maxBytesWritableToSubThread();

    return
      uint(double(bytesUsed / double(d->pipe->toSubThreadBufferSize())) * 100);
  }

  void Output::pause()
  {
    d->pipe->pauseSubThread();
  }

  void Output::unpause()
  {
    d->pipe->unpauseSubThread();
  }
}

