// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_OUTPUT_THREAD_H
#define SQRADIO_OUTPUT_THREAD_H

#ifdef SQRADIO_EMBEDDED
#include "rik/Thread.h"
#else
#include <qthread.h>
#endif // SQRADIO_EMBEDDED

class QThreadPipe;
class QObject;

namespace SqRadio
{
#ifdef SQRADIO_EMBEDDED
  class OutputThread : public rik::Thread
#else
  class OutputThread : public QThread
#endif // SQRADIO_EMBEDDED
  {
    public:

      OutputThread(QThreadPipe *, QObject *);
      virtual ~OutputThread();

      void publicUSleep(unsigned int);

    protected:

      virtual void run();

      bool openDevice(int sampleRate, int channels);

      void closeDevice();

    private:

      class Private;
      Private * d;
  };
}

#endif // SQRADIO_OUTPUT_THREAD_H
