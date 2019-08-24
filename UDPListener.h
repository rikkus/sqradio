// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2002 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_UDP_LISTENER_H
#define SQRADIO_UDP_LISTENER_H

#include <qcstring.h>
#include <qobject.h>

namespace SqRadio
{
  class UDPListener : public QObject
  {
    Q_OBJECT

    public:

      UDPListener
        (
          Q_UINT16      port    = 0,
          QObject     * parent  = 0,
          const char  * name    = 0
        );

      virtual ~UDPListener();

      bool bound() const;

    signals:

      void bound(Q_UINT16 port);
      void message(const QByteArray &);

    protected slots:

      virtual void slotActivity(int);
      virtual void slotBind();

    protected:

      virtual void startBindTimer(uint timeout);

    private:

      class Private;
      Private * d;
  };
}

#endif // SQRADIO_UDP_LISTENER_H
