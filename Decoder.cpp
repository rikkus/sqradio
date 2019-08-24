// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include "MetaDataEvent.h"
#include "DecoderThread.h"
#include "KillMeEvent.h"
#include "Decoder.h"
#include "Enum.h"

#ifdef SQRADIO_EMBEDDED
#include "UnixServerSocket.h"
#include "UnixSocket.h"
#else
#include "QThreadPipe.h"
#endif // SQRADIO_EMBEDDED

namespace SqRadio
{
  class Decoder::Private
  {
    public:

      DecoderThread     * thread;

#ifdef SQRADIO_EMBEDDED
      UnixServerSocket  * serverSocket;
      UnixSocket        * clientSocket;
#else
      QThreadPipe       * pipe;
#endif // SQRADIO_EMBEDDED
  };

  Decoder::Decoder(QObject * parent)
    : QObject(parent)
  {
    d = new Private;

    d->thread = 0;

#ifdef SQRADIO_EMBEDDED
    d->serverSocket = 0;
    d->clientSocket = 0;
#else
    d->pipe   = 0;
#endif // SQRADIO_EMBEDDED
  }

  Decoder::~Decoder()
  {
    delete d;
  }

  bool Decoder::open(const QByteArray & buf)
  {
#ifdef SQRADIO_EMBEDDED
#else
    QThreadPipe * pipe = new QThreadPipe(4096, 65536, this);
    pipe->unpauseSubThread();

    DecoderThread * thread = new DecoderThread(pipe, this);

    if (!thread->open(buf))
    {
      delete pipe;
      delete thread;
      return false;
    }

    d->pipe = pipe;
    d->thread = thread;

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


#endif // SQRADIO_EMBEDDED
    d->thread->start();

    return true;
  }

  bool Decoder::decode(const QByteArray & buf)
  {
    if (0 == d->thread || 0 == d->pipe)
    {
      return false;
    }

    if (0 == writeBufferAvailable())
    {
      return false;
    }

    if (!d->pipe->writeToSubThread(buf))
    {
      qDebug("Decoder::decode: Can't write to subthread.");
      return false;
    }

    checkIfReadyAndEmit();

    return true;
  }

  bool Decoder::isOpen() const
  {
    return 0 != d->thread;
  }

  void Decoder::close()
  {
    if (0 != d->pipe)
      d->pipe->mainThreadFinished();

    d->thread = 0;
  }

  void Decoder::slotReadyRead()
  {
    emit(readyRead());
  }

  Q_ULONG Decoder::bytesAvailable() const
  {
    if (0 == d->pipe)
    {
      qDebug("Decoder::bytesAvailable: No pipe");
      return 0;
    }

    return d->pipe->maxBytesReadableFromSubThread();
  }

  QByteArray Decoder::readBlock(unsigned int maxBytes)
  {
    if (0 == d->pipe)
    {
      qDebug("Decoder::readBlock: No pipe");
      return QByteArray();
    }

    maxBytes = QMIN(maxBytes, bytesAvailable());

    return d->pipe->readFromSubThread(maxBytes);
  }

  void Decoder::slotReadyWrite()
  {
    checkIfReadyAndEmit();
  }

  Q_ULONG Decoder::bufferFill() const
  {
    return bufferSize() - writeBufferAvailable();
  }

  Q_ULONG Decoder::bufferSize() const
  {
    if (0 == d->pipe)
      return 0;

    return d->pipe->toSubThreadBufferSize();
  }

  Q_ULONG Decoder::writeBufferAvailable() const
  {
    if (0 == d->pipe)
      return 0;

    return d->pipe->maxBytesWritableToSubThread();
  }

  void Decoder::checkIfReadyAndEmit()
  {
    if (0 != writeBufferAvailable())
      emit(readyForData());
  }

  Q_ULONG Decoder::readBufferAvailable() const
  {
    return
      d->pipe->toMainThreadBufferSize()
      -
      d->pipe->maxBytesReadableFromSubThread();
  }

#ifndef SQRADIO_EMBEDDED
  bool Decoder::event(QEvent * e)
  {
    switch (e->type())
    {
      case 8060:
        delete static_cast<KillMeEvent *>(e)->thread();
        return true;
        break;

      case 8070:
        {
          MetaDataEvent * metaDataEvent = static_cast<MetaDataEvent *>(e);

          QMap<QString, QString> infoMap;

          infoMap["Artist"] = metaDataEvent->artist();
          infoMap["Album"]  = metaDataEvent->album();
          infoMap["Title"]  = metaDataEvent->title();

          emit(streamInfo(infoMap));
          return true;
        }
        break;

      default:
        return QObject::event(e);
        break;
    }
  }
#endif // SQRADIO_EMBEDDED
}

