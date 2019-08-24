// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <unistd.h>
#include <cstring>    // For strerror.
#include <cstdlib>    // For mkstemp.

#include <cerrno>

#include <qcstring.h>
#include <qfile.h>
#include <qsocketnotifier.h>

#include "UnixServerSocket.h"

class UnixServerSocket::Private
{
  public:

    QString path;
    int     fd;
    bool    bound;

    QSocketNotifier * socketNotifier;
};

UnixServerSocket::UnixServerSocket
(
  QObject     * parent,
  const char  * name
)
  : QObject(parent, name) 
{
  d = new Private;

  d->fd             = -1;
  d->bound          = false;
  d->socketNotifier = 0;
}

bool UnixServerSocket::bindToTempFile()
{
  QString tempFileName = QString("/tmp/sqradio-%1-XXXXXX").arg(getpid());
  QCString tempFileNameLocalised = QFile::encodeName(tempFileName);

  char * filenameTemplate = tempFileNameLocalised.data();

  int err = ::mkstemp(filenameTemplate);

  if (-1 == err)
  {
    qDebug("Unable to create temp file: %s", strerror(errno));
    return false;
  }

  return bind(QFile::decodeName(tempFileNameLocalised));
}

bool UnixServerSocket::bind(const QString & path)
{
  int fd;

  QCString localEncodingOfPath = QFile::encodeName(path);

  struct sockaddr_un socketName;

  Q_ULONG maxLength = sizeof(socketName.sun_path) - 1;

  if (localEncodingOfPath.length() > maxLength)
  {
    qDebug("UnixServerSocket: Path too long");
    return false;
  }

  fd = ::socket(AF_UNIX, SOCK_STREAM, 0);

  if (fd < 0)
  {
    qDebug("UnixServerSocket: socket() failed: %s", strerror(errno));
  }

  socketName.sun_family = AF_UNIX;

  ::memcpy
    (
      socketName.sun_path,
      localEncodingOfPath.data(),
      localEncodingOfPath.length() + 1
    );

  int err = ::unlink(socketName.sun_path);

  if ((0 != err) && (ENOENT != errno))
  {
    qDebug
      (
        "UnixServerSocket: unlink() failed: (%d): %s",
        errno, strerror(errno)
      );

    return false;
  }

  int nameLength =
    strlen(socketName.sun_path) + sizeof(socketName.sun_family);

  int ret =
    ::bind
    (
      fd,
      reinterpret_cast<struct sockaddr *>(&socketName),
      nameLength
    );

  if (ret < 0)
  {
    qDebug("UnixServerSocket: bind() failed: %s", strerror(errno));

    ::close(fd);
    return false;
  }

  if (::listen(fd, 128) < 0)
  {
    qDebug
      (
        "UnixServerSocket: listen() failed: %s",
        strerror(errno)
      );

    ::close(fd);
    return false;
  }

  d->fd     = fd;
  d->path   = path;
  d->bound  = true;

  d->socketNotifier = new QSocketNotifier(d->fd, QSocketNotifier::Read, this);

  connect
    (
      d->socketNotifier,
      SIGNAL(activated(int)),
      SLOT(slotReadyRead())
    );

  return true;
}

UnixServerSocket::~UnixServerSocket()
{
  if (d->bound)
  {
    ::close(d->fd);
    ::unlink(d->path);
  }

  delete d;
}

bool UnixServerSocket::bound() const
{
  return d->bound;
}

QString UnixServerSocket::path() const
{
  return d->path;
}

void UnixServerSocket::slotReadyRead()
{
  struct sockaddr incomingAddress;
  socklen_t       incomingAddressLength;

  int newSocket = ::accept(d->fd, &incomingAddress, &incomingAddressLength);

  if (-1 == newSocket)
  {
    qDebug
      (
        "UnixServerSocket: accept() failed: %s",
        strerror(errno)
      );
  }

  emit(newConnection(newSocket));
}
