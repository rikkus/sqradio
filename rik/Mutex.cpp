// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2001 Rik Hemsley (rikkus) <rik@kde.org>

#include <pthread.h>
#include <cerrno>

#include "rik/Mutex.h"

namespace rik
{
  class Mutex::Private
  {
    public:

      pthread_mutex_t mutex;
  };

  Mutex::Mutex()
  {
    d = new Private;

    pthread_mutexattr_t attr;

    pthread_mutexattr_init(&attr);

    pthread_mutex_init(&d->mutex, &attr);

    pthread_mutexattr_destroy(&attr);
  }

  Mutex::~Mutex()
  {
    pthread_mutex_destroy(&d->mutex);

    delete d;
  }

  void Mutex::lock()
  {
    pthread_mutex_lock(&d->mutex);
  }

  void Mutex::unlock()
  {
    pthread_mutex_unlock(&d->mutex);
  }

  bool Mutex::locked()
  {
    if (EBUSY == pthread_mutex_trylock(&d->mutex))
    {
      return true;
    }
    else
    {
      pthread_mutex_unlock(&d->mutex);
      return false;
    }
  }

  bool Mutex::lockNonBlocking()
  {
    if (EBUSY == pthread_mutex_trylock(&d->mutex))
    {
      return false;
    }
    else
    {
      return true;
    }
  }

  pthread_mutex_t * Mutex::mutex()
  {
    return &d->mutex;
  }
}

