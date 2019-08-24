// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2001 Rik Hemsley (rikkus) <rik@kde.org>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/poll.h>

#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <cstdio>
#include <cassert>
#include <cerrno>
#include <ctime>

#include "rik/Thread.h"

namespace rik
{
  extern "C"
  {
    static void * runThread(void * arg)
    {
      rik::Thread * t = reinterpret_cast<rik::Thread *>(arg);

      if (0 == t)
      {
        std::cerr << "runThread: arg is null" << std::endl;
        return 0;
      }

      t->run();

      return 0;
    }
  };

  class Thread::Private
  {
    public:

      pthread_t                 threadHandle;
      pthread_attr_t            threadAttributes;
  };

  Thread::Thread()
  {
    d = new Private;
  }

  Thread::~Thread()
  {
    delete d;
  }

  ThreadStart::Return Thread::start()
  {
    if (0 != ::pthread_attr_init(&d->threadAttributes))
      return ThreadStart::UnableToInitialiseAttributes;

    int retval = ::pthread_create
      (&d->threadHandle, &d->threadAttributes, runThread, this);

    switch (retval)
    {
      case 0:
        return ThreadStart::Success;
        break;

      case EAGAIN:
        return ThreadStart::TooFewSystemResources;
        break;

      default:
        return ThreadStart::Unknown;
        break;
    }
  }

  ThreadJoin::Return Thread::join()
  {
    switch (::pthread_join(d->threadHandle, 0))
    {
      case 0:
        return ThreadJoin::Success;
        break;

      case ESRCH:
        return ThreadJoin::NoSuchThread;
        break;

      case EINVAL:
        return ThreadJoin::DetachedOrAnotherThreadWaiting;
        break;

      case EDEADLK:
        return ThreadJoin::CannotJoinSelf;
        break;

      default:
        return ThreadJoin::Unknown;
        break;
    }
  }

  ThreadDetach::Return Thread::detach()
  {
    switch (::pthread_detach(d->threadHandle))
    {
      case 0:
      case EINVAL:
        return ThreadDetach::Success;
        break;

      case ESRCH:
        return ThreadDetach::NoSuchThread;
        break;

      default:
        return ThreadDetach::Unknown;
        break;
    }
  }
}

