// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2002 Rik Hemsley (rikkus) <rik@kde.org>

#include <cerrno>
#include <cstdio>

#include "rik/Mutex.h"
#include "rik/Semaphore.h"

namespace rik
{
  class Semaphore::Private
  {
    public:

      Mutex             mutex;
      ::pthread_cond_t  cond;
      int               value;
      int               max;
  };

  Semaphore::Semaphore(int initial, int max)
  {
    d = new Private;

    d->value  = initial;
    d->max    = max;

    if (0 != ::pthread_cond_init(&d->cond, 0))
      ::perror("pthread_cond_init");

    d->value = initial;
    d->max = max;
  }

  Semaphore::~Semaphore()
  {
    if (0 != ::pthread_cond_destroy(&d->cond))
      ::perror("pthread_cond_destroy");

    delete d;
  }


  Semaphore & Semaphore::operator ++()
  {
    d->mutex.lock();

    while (d->value >= d->max)
      ::pthread_cond_wait(&d->cond, d->mutex.mutex());

    ++(d->value);

    if (d->value > d->max)
      d->value = d->max;

    d->mutex.unlock();
  }

  Semaphore & Semaphore::operator --()
  {
    d->mutex.lock();

    --(d->value);

    if (d->value < 0)
      d->value = 0;

    ::pthread_cond_broadcast(&d->cond);
    d->mutex.unlock();
  }
}
