// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef Q_UNIX_SERVER_SOCKET_H
#define Q_UNIX_SERVER_SOCKET_H

#ifndef QT_NO_NETWORK

#include <qobject.h>
#include <qstring.h>

/**
 * @short UnixServerSocket provides a Unix socket based server.
 *
 * This class allows accepting incoming connections on a Unix socket.
 * 
 * The API is simple: Create a UnixServerSocket object, bind it to
 * a path, check for success and connect to the newConnection signal.
 *
 * @see UnixSocket, QSocket, QServerSocket, QSocketNotifier.
 */
class UnixServerSocket : public QObject
{
  Q_OBJECT

  public:

    /**
     * Create a server socket object that is not bound to a path.
     * The parent and name arguments are passed onto the QObject ctor.
     */
    UnixServerSocket
    (
      QObject         * parent  = 0,
      const char      * name    = 0
    );

    /**
     * Destroys the socket. No further connections are accepted on the
     * underlying socket. Existing connections will not be disconnected.
     */
    virtual ~UnixServerSocket();

    /**
     * Bind to the given path and accept incoming connections. When an
     * incoming connection is detected, newConnection is emitted.
     *
     * If the bind fails, the existing state of the object is unaffected.
     *
     * This method tries to unlink() the filename given, so don't try this
     * with something important.
     */
    bool    bind(const QString & path);

    /**
     * Bind to a new socket created with mkstemp and accept incoming
     * connections. When an incoming connection is detected, newConnection
     * is emitted.
     *
     * If the bind fails, the existing state of the object is unaffected.
     */
    bool    bindToTempFile();

    /**
     * @return true if this socket is bound to a path.
     */
    bool    bound() const;

    /**
     * @return the path this socket is bound to or QString::null if it is
     * not yet bound.
     */
    QString path() const;

  protected slots:

    /**
     * @internal
     */
    void slotReadyRead();

  signals:

    /**
     * Emitted when a new connection has been accepted.
     * @arg socket the file descriptor of the new connection.
     */
    void newConnection(int socket);

  private:

#ifdef Q_DISABLE_COPY
    UnixServerSocket(const UnixServerSocket &);
    UnixServerSocket & operator = (const UnixServerSocket &);
#endif // Q_DISABLE_COPY

    class Private;
    Private * d;
};

#endif // QT_NO_NETWORK

#endif // Q_UNIX_SERVER_SOCKET_H
