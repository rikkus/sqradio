// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include <qapplication.h>

#include "DecoderDriver.h"
#include "DecoderDriverFactory.h"
#include "DecoderThread.h"
#include "ByteArray.h"
#include "KillMeEvent.h"

#ifdef SQRADIO_EMBEDDED
#include "UnixSocket.h"
#else
#include "QThreadPipe.h"
#endif // SQRADIO_EMBEDDED

namespace SqRadio
{
  class DecoderThread::Private
  {
    public:

      DecoderDriver * driver;

#ifdef SQRADIO_EMBEDDED
      UnixSocket * socket;
#else
      QThreadPipe * pipe;
#endif // SQRADIO_EMBEDDED

      QObject     * parent;
  };

#ifdef SQRADIO_EMBEDDED
  DecoderThread::DecoderThread
    (
      UnixSocket  * socket,
      QObject     * parent
    )
    : QThread()
  {
    d = new Private;

    d->driver = 0;
    d->socket = socket;
    d->parent = parent;
  }
#else
  DecoderThread::DecoderThread
    (
      QThreadPipe * pipe,
      QObject     * parent
    )
    : QThread()
  {
    d = new Private;

    d->driver = 0;
    d->pipe   = pipe;
    d->parent = parent;
  }
#endif // SQRADIO_EMBEDDED

  DecoderThread::~DecoderThread()
  {
    delete d;
  }

  bool DecoderThread::open(const QByteArray & a)
  {
    DecoderDriver * driver = DecoderDriverFactory::create(a);

    if (0 == driver)
      return false;

    d->driver = driver;
    return true;
  }

  void DecoderThread::run()
  {
    if (0 == d->driver)
    {
      qDebug("DecoderThread::run(): Not open. Stopping.");
      return;
    }

    d->driver->run(d->pipe);
    d->pipe->subThreadFinished();

    delete d->driver;
    d->driver = 0;

    qApp->postEvent(d->parent, new KillMeEvent(this));
  }
}

