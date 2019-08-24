// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef Q_UNIX_SOCKET_H
#define Q_UNIX_SOCKET_H

#include <qobject.h>
#include <qstring.h>

class UnixSocket : public QObject
{
  Q_OBJECT

  public:

    enum Error
    {
      FileNotFound,
      NoPermission
    };

    enum
    {
      DefaultBufferSize = 4096
    };

    UnixSocket
    (
      QObject         * parent  = 0,
      const char      * name    = 0
    );

    UnixSocket
    (
      Q_ULONG           readBufferSize,
      Q_ULONG           writeBufferSize,
      QObject         * parent  = 0,
      const char      * name    = 0
    );

    virtual ~UnixSocket();

    bool connected() const;

    void setSocket(int fd);
    bool connectToPath(const QString & path);
    void close();
    void flush();

    Q_ULONG readBufferSize() const;
    Q_ULONG writeBufferSize() const;

    Q_ULONG readBufferAvailable() const;
    Q_ULONG writeBufferAvailable() const;

    Q_LONG readBlock(char * data, Q_ULONG maxlen);
    Q_LONG writeBlock(const char * data, Q_ULONG maxlen);

    bool write(const QByteArray &);
    bool write(const QString & stringWhichWillBeSentAsUtf8);
    bool write(const char * cString);

    bool canReadLine() const;
    QString readLine();

    //void report() const;

  protected slots:

    void slotReadyRead(int);
    void slotReadyWrite(int);

  signals:

    void readyRead(Q_ULONG);
    void readyWrite(Q_ULONG);

  private:

#ifdef Q_DISABLE_COPY
    UnixSocket(const UnixSocket &);
    UnixSocket & operator = (const UnixSocket &);
#endif // Q_DISABLE_COPY

    class Private;
    Private * d;
};

#endif // Q_UNIX_SOCKET_H
