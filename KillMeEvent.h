// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_KILL_ME_EVENT_H
#define SQRADIO_KILL_ME_EVENT_H

#ifndef SQRADIO_EMBEDDED

#include <qevent.h>

#include <qthread.h>

namespace SqRadio
{
  class KillMeEvent : public QCustomEvent
  {
    public:

      KillMeEvent(QThread * thread)
        : QCustomEvent(8060),
          thread_(thread)
      {
        // Empty.
      }

      QThread * thread()
      {
        return thread_;
      }

    private:

      QThread * thread_;
  };
}

#endif // SQRADIO_EMBEDDED

#endif // SQRADIO_KILL_ME_EVENT_H
