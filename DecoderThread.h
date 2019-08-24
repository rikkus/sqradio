// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_DECODER_THREAD_H
#define SQRADIO_DECODER_THREAD_H

#ifdef SQRADIO_EMBEDDED
#include "rik/Thread.h"
#else
#include <qthread.h>
#endif // SQRADIO_EMBEDDED

#include <qcstring.h>

class UnixSocket;
class QThreadPipe;

namespace SqRadio
{
#ifdef SQRADIO_EMBEDDED
  class DecoderThread : public rik::Thread
#else
  class DecoderThread : public QThread
#endif // SQRADIO_EMBEDDED
  {
    public:

#ifdef SQRADIO_EMBEDDED
      DecoderThread(UnixSocket *, QObject * parent);
#else
      DecoderThread(QThreadPipe *, QObject * parent);
#endif // SQRADIO_EMBEDDED

      virtual ~DecoderThread();

      bool open(const QByteArray &);

    protected:

      virtual void run();

    private:

      class Private;
      Private * d;
  };
}

#endif // SQRADIO_DECODER_THREAD_H
