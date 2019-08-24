// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef QTHREADPIPE_H
#define QTHREADPIPE_H

#include <qobject.h>

class QThreadPipe : public QObject
{
  Q_OBJECT

  public:

    enum { DefaultBufferSize = 4096 };

    QThreadPipe(QObject * parent = 0, const char * name = 0);

    QThreadPipe
      (
        Q_ULONG       toSubThreadBufferSize,
        Q_ULONG       toMainThreadBufferSize,
        QObject     * parent  = 0,
        const char  * name    = 0
      );

    virtual ~QThreadPipe();

    // For use by main thread.

    Q_ULONG     maxBytesReadableFromSubThread () const;
    Q_ULONG     maxBytesWritableToSubThread   () const;
    QByteArray  readFromSubThread             (Q_ULONG maxBytes);
    bool        writeToSubThread              (const QByteArray &);
    bool        writeToSubThread              (const char * data, Q_ULONG size);
    Q_ULONG     toSubThreadBufferSize         () const;

    // For use by sub thread.

    Q_ULONG     maxBytesWritableToMainThread  () const;
    Q_ULONG     maxBytesReadableFromMainThread() const;
    QByteArray  readFromMainThread            (Q_ULONG maxBytes);
    bool        writeToMainThread             (const QByteArray &);
    bool        writeToMainThread             (const char * data, Q_ULONG size);
    Q_ULONG     toMainThreadBufferSize        () const;

    bool        writeToMainThreadFlushingBuffer
      (const QByteArray &);

    bool        writeToMainThreadFlushingBuffer
      (const char * data, Q_ULONG size);

    void        mainThreadFinished();
    void        subThreadFinished();

    void        pauseSubThread();
    void        unpauseSubThread();

  protected:

    enum Event
    {
      CanReadFromSubThreadEventT  = 8080,
      CanWriteToSubThreadEventT   = 8081,
      SubThreadFinishedEventT     = 8082
    };

    class CanReadFromSubThreadEvent : public QCustomEvent
    {
      public:

        CanReadFromSubThreadEvent::CanReadFromSubThreadEvent()
          : QCustomEvent(CanReadFromSubThreadEventT)
        {
          // Empty.
        }
    };

    class CanWriteToSubThreadEvent : public QCustomEvent
    {
      public:

        CanWriteToSubThreadEvent::CanWriteToSubThreadEvent()
          : QCustomEvent(CanWriteToSubThreadEventT)
        {
          // Empty.
        }
    };

    class SubThreadFinishedEvent : public QCustomEvent
    {
      public:

        SubThreadFinishedEvent::SubThreadFinishedEvent()
          : QCustomEvent(SubThreadFinishedEventT)
        {
          // Empty.
        }
    };

    bool event(QEvent *);

  signals:

    void canReadFromSubThread();
    void canWriteToSubThread();
    void subThreadClosed();

  private:

    void init();

    class Private;
    Private * d;
};


#endif // QTHREADPIPE_H
