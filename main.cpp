// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include <sys/types.h>
#include <unistd.h>
#include <sched.h>


#include "Application.h"

#include <cstdlib>

static void nullMessageHandler(QtMsgType, const char *)
{
  // Empty.
}

int main(int argc, char ** argv)
{
  struct sched_param sp;

  long priority =
    (sched_get_priority_max(SCHED_FIFO) + sched_get_priority_min(SCHED_FIFO))
    / 2;

  sp.sched_priority = priority;

  if (sched_setscheduler(0, SCHED_FIFO, &sp) == 0)
  {
    // qDebug("Running as realtime, priority %ld", priority);
  }
  else
  {
    // qDebug("Couldn't set realtime priority");
  }

  if (geteuid() != getuid()) 
  {
    seteuid(getuid());
  }

  SqRadio::Application app(argc, argv);

//  if (0 != getenv("DEBUG"))
//    qInstallMsgHandler(nullMessageHandler);

  return app.exec();
}

