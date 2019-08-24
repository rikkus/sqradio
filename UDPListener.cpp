// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2002 Rik Hemsley (rikkus) <rik@kde.org>

#include <qtimer.h>
#include <qsocketdevice.h>
#include <qsocketnotifier.h>

#include "UDPListener.h"

namespace SqRadio
{
  class UDPListener::Private
  {
    public:

      Q_UINT16          port;
      QSocketDevice   * socket;
      QSocketNotifier * socketNotifier;
      bool              bound;
  };

  UDPListener::UDPListener(Q_UINT16 port, QObject * parent, const char * name)
    : QObject(parent, name)
  {
    d = new Private;

    d->port           = port;
    d->socket         = 0;
    d->socketNotifier = 0;
    d->bound          = false;

    startBindTimer(0);
  }

  UDPListener::~UDPListener()
  {
    delete d->socket;
    delete d;
  }

  void UDPListener::startBindTimer(uint timeout)
  {
    d->bound = false;

    delete d->socketNotifier;
    d->socketNotifier = 0;

    delete d->socket;
    d->socket = 0;

    QTimer::singleShot(timeout, this, SLOT(slotBind()));
  }

  void UDPListener::slotBind()
  {
    d->socket = new QSocketDevice(QSocketDevice::Datagram);

    if (!d->socket->bind(QHostAddress(), d->port))
    {
      qDebug("UDPListener::slotBind(): Can't bind");
      startBindTimer(1000);
      return;
    }

    qDebug("UDPListener::slotBind(): Bound to port: %d", d->socket->port());

    d->bound = true;
    emit(bound(d->socket->port()));

    d->socketNotifier =
      new QSocketNotifier
      (
        d->socket->socket(),
        QSocketNotifier::Read,
        this,
        "UDPListener socket notifier"
      );

    connect(d->socketNotifier, SIGNAL(activated(int)), SLOT(slotActivity(int)));
  }

  void UDPListener::slotActivity(int)
  {
    qDebug("UDPListener::slotActivity()");

    if (QSocketDevice::NoError != d->socket->error())
    {
      startBindTimer(0);
      return;
    }

    int bytesAvailable = d->socket->bytesAvailable();

    QByteArray data(bytesAvailable);

    int bytesRead = d->socket->readBlock(data.data(), data.size());

    if (bytesRead != bytesAvailable)
    {
      qDebug("Couldn't read all available bytes from socket");
      startBindTimer(0);
      return;
    }

    emit(message(data));
  }

  bool UDPListener::bound() const
  {
    return d->bound;
  }
}

