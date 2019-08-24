// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <unistd.h>
#include <cstring>    // For strerror.

#include <cerrno>

#include <qcstring.h>
#include <qfile.h>
#include <qsocketnotifier.h>
#include <qtimer.h>

#include "UnixSocket.h"

class UnixSocket::Private
{
  public:

    int fd;
    bool connected;
    QString path;

    QSocketNotifier * readNotifier;
    QSocketNotifier * writeNotifier;

    QByteArray readBuffer;
    QByteArray writeBuffer;

    Q_ULONG maxReadBufferSize;
    Q_ULONG maxWriteBufferSize;
};

UnixSocket::UnixSocket
(
  QObject     * parent,
  const char  * name
)
  : QObject(parent, name)
{
  d = new Private;

  d->fd             = -1;
  d->connected      = false;
  d->readNotifier   = 0;
  d->writeNotifier  = 0;

  d->maxReadBufferSize    = DefaultBufferSize;
  d->maxWriteBufferSize   = DefaultBufferSize;
}


UnixSocket::UnixSocket
(
  Q_ULONG       readBufferSize,
  Q_ULONG       writeBufferSize,
  QObject     * parent,
  const char  * name
)
  : QObject(parent, name)
{
  d = new Private;

  d->fd             = -1;
  d->connected      = false;
  d->readNotifier   = 0;
  d->writeNotifier  = 0;

  d->maxReadBufferSize    = readBufferSize;
  d->maxWriteBufferSize   = writeBufferSize;
}

UnixSocket::~UnixSocket()
{
  ::close(d->fd);
  delete d;
}

bool UnixSocket::connected() const
{
  return d->connected;
}

bool UnixSocket::connectToPath(const QString & path)
{
  d->path = path;

  QCString localEncodingOfPath = QFile::encodeName(d->path);

  struct sockaddr_un socketName;

  Q_ULONG maxLength = sizeof(socketName.sun_path) - 1;

  if (localEncodingOfPath.length() > maxLength)
  {
    qDebug("UnixSocket::UnixSocket(): Path too long");
    return false;
  }

  d->fd = ::socket(AF_UNIX, SOCK_STREAM, 0);

  socketName.sun_family = AF_UNIX;

  ::memcpy
    (
      socketName.sun_path,
      localEncodingOfPath.data(),
      localEncodingOfPath.length() + 1
    );

  int err =
    ::connect
    (
      d->fd,
      (const struct sockaddr *)(&socketName),
      sizeof(socketName)
    );

  if (0 != err)
  {
    qDebug("Can't connect to %s", localEncodingOfPath.data());
    return false;
  }

  d->readNotifier = new QSocketNotifier(d->fd, QSocketNotifier::Read, this);
  d->writeNotifier = new QSocketNotifier(d->fd, QSocketNotifier::Write, this);
  d->writeNotifier->setEnabled(false);

  connect
    (
      d->readNotifier,
      SIGNAL(activated(int)),
      SLOT(slotReadyRead(int))
    );

  connect
    (
      d->writeNotifier,
      SIGNAL(activated(int)),
      SLOT(slotReadyWrite(int))
    );

  d->connected = true;
  return true;
}

void UnixSocket::close()
{
  ::close(d->fd);
  d->fd = -1;
  d->connected = false;
  delete d->readNotifier;
  d->readNotifier = 0;
  delete d->writeNotifier;
  d->writeNotifier = 0;
}

void UnixSocket::flush()
{
  qDebug("STUB");
}

void UnixSocket::setSocket(int fd)
{
  d->fd = fd;

  d->readNotifier = new QSocketNotifier(d->fd, QSocketNotifier::Read, this);
  d->writeNotifier = new QSocketNotifier(d->fd, QSocketNotifier::Write, this);
  d->writeNotifier->setEnabled(false);

  connect
    (
      d->readNotifier,
      SIGNAL(activated(int)),
      SLOT(slotReadyRead(int))
    );

  connect
    (
      d->writeNotifier,
      SIGNAL(activated(int)),
      SLOT(slotReadyWrite(int))
    );

  d->connected = true;
}

Q_ULONG UnixSocket::readBufferAvailable() const
{
  return d->readBuffer.size();
}

Q_ULONG UnixSocket::writeBufferAvailable() const
{
  return d->maxWriteBufferSize - d->writeBuffer.size();
}

Q_LONG UnixSocket::readBlock(char * data, Q_ULONG maxlen)
{
  unsigned long int bytesToReturn = QMIN(maxlen, d->readBuffer.size());

  memcpy(data, d->readBuffer.data(), bytesToReturn);

  memmove
    (
      d->readBuffer.data(),
      d->readBuffer.data() + bytesToReturn,
      d->readBuffer.size() - bytesToReturn
    );

  d->readBuffer.resize(d->readBuffer.size() - bytesToReturn);

  d->readNotifier->setEnabled(true);

  return bytesToReturn;
}

Q_LONG UnixSocket::writeBlock(const char * data, Q_ULONG maxlen)
{
  //qDebug("UnixSocket::writeBlock(..., %ld", maxlen);

  if (d->writeBuffer.size() >= d->maxWriteBufferSize)
  {
    //qDebug("UnixSocket::writeBlock(): Write buffer is full");
    return -1;
  }

  Q_ULONG bytesToTake =
    QMIN(d->maxWriteBufferSize - d->writeBuffer.size(), maxlen);

  //qDebug("UnixSocket::writeBlock(): bytesToTake: %ld", bytesToTake);

  Q_ULONG oldWriteBufferSize = d->writeBuffer.size();

  d->writeBuffer.resize(oldWriteBufferSize + bytesToTake);

  memcpy(d->writeBuffer.data() + oldWriteBufferSize, data, bytesToTake);

  d->writeNotifier->setEnabled(true);

  return bytesToTake;
}

bool UnixSocket::canReadLine() const
{
  return (-1 != d->readBuffer.find('\n'));
}

QString UnixSocket::readLine()
{
  int eolPos = d->readBuffer.find('\n');

  if (-1 == eolPos)
    return QString::null;

  return QString::fromLatin1(d->readBuffer.data(), eolPos);
}

void UnixSocket::slotReadyRead(int)
{
  d->readNotifier->setEnabled(false);

  if (!d->connected)
    return;

  if (d->readBuffer.size() >= d->maxReadBufferSize)
    return;

  Q_ULONG bytesToRead = d->maxReadBufferSize - d->readBuffer.size();

  d->readBuffer.resize(d->readBuffer.size() + bytesToRead);

  int bytesReceived =
    ::recv
    (
      d->fd,
      d->readBuffer.data() + d->readBuffer.size() - bytesToRead,
      bytesToRead,
      MSG_NOSIGNAL | MSG_DONTWAIT
    );

  if (-1 == bytesReceived)
  {
    switch (errno)
    {
      case EAGAIN:
        {
          //qDebug("UnixSocket::slotReadyRead(): Read would block");
          d->readNotifier->setEnabled(true);
        }
        break;

      case EINTR:
        {
          //qDebug("UnixSocket::slotReadyRead(): Interrupted");
          d->readNotifier->setEnabled(true);
        }
        break;

      default:
        {
          qDebug("UnixSocket::slotReadyRead(): Error: %s", strerror(errno));
          close();
        }
    }
  }
  else
  {
    if (bytesReceived < signed(bytesToRead))
    {
      d->readBuffer.resize
        (d->readBuffer.size() - (bytesToRead - bytesReceived));
    }
  }

  if (d->readBuffer.size() < d->maxReadBufferSize)
    d->readNotifier->setEnabled(true);

  if (!d->readBuffer.isEmpty())
    emit(readyRead(d->readBuffer.size()));
}

void UnixSocket::slotReadyWrite(int)
{
  //qDebug("UnixSocket::slotReadyWrite()");

  d->writeNotifier->setEnabled(false);

  if (!d->connected)
  {
    //qDebug("UnixSocket::slotReadyWrite(): Not connected");
    return;
  }

  if (d->writeBuffer.isEmpty())
  {
    //qDebug("UnixSocket::slotReadyWrite(): Nothing to write");
    emit(readyWrite(d->maxWriteBufferSize));
  }

  int bytesSent =
    ::send
    (
      d->fd,
      d->writeBuffer.data(),
      d->writeBuffer.size(),
      MSG_DONTWAIT | MSG_NOSIGNAL
    );

  if (-1 == bytesSent)
  {
    switch (errno)
    {
      case EMSGSIZE:
        {
          qDebug("UnixSocket::slotReadyWrite(): I'm using too large a buffer");
        }
        break;

      case EAGAIN:
        {
          //qDebug("UnixSocket::slotReadyWrite(): Writing would block");
          d->writeNotifier->setEnabled(true);
        }
        break;

      case EINTR:
        {
          //qDebug("UnixSocket::slotReadyWrite(): Interrupted");
          d->writeNotifier->setEnabled(true);
        }

      case EPIPE:
      default:
        {
          qDebug("UnixSocket::slotReadyWrite(): Error: %s", strerror(errno));
          close();
        }
        break;
    }
  }
  else
  {
    memmove
      (
        d->writeBuffer.data(),
        d->writeBuffer.data() + bytesSent,
        d->writeBuffer.size() - bytesSent
      );

    d->writeBuffer.resize(d->writeBuffer.size() - bytesSent);
  }

  emit(readyWrite(d->maxWriteBufferSize - d->writeBuffer.size()));
}

bool UnixSocket::write(const QByteArray & a)
{
  return writeBlock(a.data(), a.size());
}

bool UnixSocket::write(const QString & s)
{
  QCString utf8(s.utf8());

  Q_LONG bytesWritten = writeBlock(utf8.data(), utf8.length());

  return (bytesWritten == Q_LONG(utf8.size()));
}

bool UnixSocket::write(const char * cString)
{
  QCString s(cString);

  Q_LONG bytesWritten = writeBlock(s.data(), s.length());

  return (bytesWritten == Q_LONG(s.size()));
}

Q_ULONG UnixSocket::readBufferSize() const
{
  return d->maxReadBufferSize;
}

Q_ULONG UnixSocket::writeBufferSize() const
{
  return d->maxWriteBufferSize;
}

#if 0
void UnixSocket::report() const
{
  qDebug
    (
      "UnixSocket(%s)::report: readBufferAvailable: %ld",
      name(),
      readBufferAvailable()
    );

  qDebug
    (
      "UnixSocket(%s)::report: writeBufferAvailable: %ld",
      name(),
      writeBufferAvailable()
    );
}
#endif
