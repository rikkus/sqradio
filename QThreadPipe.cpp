// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include <qapplication.h>
#include <qthread.h>

#include "QThreadPipe.h"

class QThreadPipe::Private
{
  public:

    QByteArray    outputBuffer;
    QByteArray    inputBuffer;

    Q_ULONG       maxOutputBufferSize;
    Q_ULONG       maxInputBufferSize;

    QMutex        inputMutex;
    QMutex        outputMutex;

    QSemaphore  * outputSemaphore;
    QSemaphore  * inputSemaphore;

    bool          mainThreadFinished;
    bool          subThreadFinished;

    bool          pause;
};

QThreadPipe::QThreadPipe
(
  Q_ULONG       toSubThreadBufferSize,
  Q_ULONG       toMainThreadBufferSize,
  QObject     * parent,
  const char  * name
)
  : QObject(parent, name)
{
  init();

  d->maxOutputBufferSize  = toSubThreadBufferSize;
  d->maxInputBufferSize   = toMainThreadBufferSize;
}

QThreadPipe::QThreadPipe(QObject * parent, const char * name)
  : QObject(parent, name)
{
  init();
}

void QThreadPipe::init()
{
  d = new Private;

  d->mainThreadFinished = false;
  d->subThreadFinished  = false;

  d->outputSemaphore = new QSemaphore(1);
  (*d->outputSemaphore)++;

  d->inputSemaphore = new QSemaphore(1);

  d->pause = false;
}

QThreadPipe::~QThreadPipe()
{
  delete d;
}

Q_ULONG QThreadPipe::maxBytesReadableFromSubThread() const
{
  if (d->subThreadFinished)
    return 0;

  Q_ULONG n;
  d->inputMutex.lock();
  n = d->inputBuffer.size();
  d->inputMutex.unlock();
  return n;
}

Q_ULONG QThreadPipe::maxBytesWritableToSubThread() const
{
  if (d->subThreadFinished)
    return 0;

  Q_ULONG n;
  d->outputMutex.lock();
  n = (d->maxOutputBufferSize - d->outputBuffer.size());
  d->outputMutex.unlock();
  return n;
}

QByteArray QThreadPipe::readFromSubThread(Q_ULONG maxBytes)
{
  if (d->subThreadFinished)
    return QByteArray();

  d->inputMutex.lock();

  Q_ULONG bytesToCopy = QMIN(maxBytes, d->inputBuffer.size());

  QByteArray a(bytesToCopy);

  memcpy(a.data(), d->inputBuffer.data(), bytesToCopy);

  memmove
    (
      d->inputBuffer.data(),
      d->inputBuffer.data() + bytesToCopy,
      d->inputBuffer.size() - bytesToCopy
    );

  d->inputBuffer.resize(d->inputBuffer.size() - bytesToCopy);

  if (d->inputBuffer.size() < d->maxInputBufferSize)
    (*d->inputSemaphore)--;

  d->inputMutex.unlock();

  return a;
}

bool QThreadPipe::writeToSubThread(const char * data, Q_ULONG size)
{
  if (d->subThreadFinished)
    return false;

  QByteArray a(size);
  a.setRawData(data, size);
  memcpy(a.data(), data, size);
  bool ok = writeToSubThread(a);
  a.resetRawData(data, size);
  return ok;
}

bool QThreadPipe::writeToSubThread(const QByteArray & a)
{
  if (d->subThreadFinished)
    return false;

  d->outputMutex.lock();

  if (d->outputBuffer.size() + a.size() > d->maxOutputBufferSize)
  {
    d->outputMutex.unlock();
    return false;
  }

  unsigned int oldOutputBufferSize = d->outputBuffer.size();

  d->outputBuffer.resize(oldOutputBufferSize + a.size());

  memcpy(d->outputBuffer.data() + oldOutputBufferSize, a.data(), a.size());

  if (!d->pause)
    (*d->outputSemaphore)--;

  d->outputMutex.unlock();

  return true;
}

Q_ULONG QThreadPipe::maxBytesWritableToMainThread() const
{
  if (d->mainThreadFinished)
    return 0;

  Q_ULONG n;
  d->inputMutex.lock();
  n = (d->maxInputBufferSize - d->inputBuffer.size());
  d->inputMutex.unlock();
  return n;
}

Q_ULONG QThreadPipe::maxBytesReadableFromMainThread() const
{
  if (d->mainThreadFinished)
    return 0;

  Q_ULONG n;
  d->outputMutex.lock();
  n = d->outputBuffer.size();
  d->outputMutex.unlock();
  return n;
}

QByteArray QThreadPipe::readFromMainThread(Q_ULONG maxBytes)
{
  if (d->mainThreadFinished)
    return QByteArray();

  (*d->outputSemaphore)++;

  d->outputMutex.lock();

  Q_ULONG bytesToCopy = QMIN(maxBytes, d->outputBuffer.size());

  QByteArray a(bytesToCopy);

  memcpy(a.data(), d->outputBuffer.data(), bytesToCopy);

  memmove
    (
      d->outputBuffer.data(),
      d->outputBuffer.data() + bytesToCopy,
      d->outputBuffer.size() - bytesToCopy
    );

  d->outputBuffer.resize(d->outputBuffer.size() - bytesToCopy);

  QApplication::postEvent(this, new CanWriteToSubThreadEvent);

  if (!d->outputBuffer.isEmpty())
    if (!d->pause)
      (*d->outputSemaphore)--;

  d->outputMutex.unlock();

  return a;
}

bool QThreadPipe::writeToMainThread(const char * data, Q_ULONG size)
{
  QByteArray a(size);
  a.setRawData(data, size);
  memcpy(a.data(), data, size);
  bool ok = writeToMainThread(a);
  a.resetRawData(data, size);
  return ok;
}

bool QThreadPipe::writeToMainThread(const QByteArray & a)
{
  if (d->mainThreadFinished)
    return false;

  unsigned int bytesLeft = a.size();

  while (0 != bytesLeft)
  {
    if (d->mainThreadFinished)
      return false;

    (*d->inputSemaphore)++;

    d->inputMutex.lock();

    unsigned int bytesToCopy =
      QMIN(bytesLeft, d->maxInputBufferSize - d->inputBuffer.size());

    unsigned int oldInputBufferSize = d->inputBuffer.size();

    d->inputBuffer.resize(oldInputBufferSize + bytesToCopy);

    memcpy
      (
        d->inputBuffer.data() + oldInputBufferSize,
        a.data() + a.size() - bytesLeft,
        bytesToCopy
      );

    bytesLeft -= bytesToCopy;

    QApplication::postEvent(this, new CanReadFromSubThreadEvent);

    d->inputMutex.unlock();
  }

  return true;
}

bool QThreadPipe::event(QEvent * e)
{
  switch (e->type())
  {
    case CanReadFromSubThreadEventT:
      emit(canReadFromSubThread());
      return true;
      break;

    case CanWriteToSubThreadEventT:
      emit(canWriteToSubThread());
      return true;
      break;

    case SubThreadFinishedEventT:
      emit(subThreadFinished());

    default:
      return QObject::event(e);
      break;
  }
}

Q_ULONG QThreadPipe::toSubThreadBufferSize() const
{
  return d->maxOutputBufferSize;
}

Q_ULONG QThreadPipe::toMainThreadBufferSize() const
{
  return d->maxInputBufferSize;
}

void QThreadPipe::mainThreadFinished()
{
  d->mainThreadFinished = true;
}

void QThreadPipe::subThreadFinished()
{
  if (!d->subThreadFinished)
  {
    d->subThreadFinished = true;
    QApplication::postEvent(this, new SubThreadFinishedEvent);
  }
}

bool QThreadPipe::writeToMainThreadFlushingBuffer
(
  const char  * data,
  Q_ULONG       size
)
{
  QByteArray a(size);
  a.setRawData(data, size);
  memcpy(a.data(), data, size);
  bool ok = writeToMainThreadFlushingBuffer(a);
  a.resetRawData(data, size);
  return ok;
}

bool QThreadPipe::writeToMainThreadFlushingBuffer(const QByteArray & a)
{
  if (d->mainThreadFinished)
    return false;

  unsigned int bytesLeft = a.size();

  while (0 != bytesLeft)
  {
    if (d->mainThreadFinished)
      return false;

    (*d->inputSemaphore)++;

    d->inputMutex.lock();

    unsigned int bytesToCopy = QMIN(bytesLeft, d->maxInputBufferSize);

    d->inputBuffer.resize(bytesToCopy);

    memcpy
      (
        d->inputBuffer.data(),
        a.data() + a.size() - bytesLeft,
        bytesToCopy
      );

    bytesLeft -= bytesToCopy;

    QApplication::postEvent(this, new CanReadFromSubThreadEvent);

    d->inputMutex.unlock();
  }

  return true;
}

void QThreadPipe::pauseSubThread()
{
  d->pause = true;
}

void QThreadPipe::unpauseSubThread()
{
  d->outputMutex.lock();
  d->pause = false;
  if (!d->outputBuffer.isEmpty())
    (*d->outputSemaphore)--;
  d->outputMutex.unlock();
}

