// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2002 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef __RIK_SEMAPHORE_H__
#define __RIK_SEMAPHORE_H__

#include <pthread.h>

namespace rik
{
  class Semaphore
  {
    public:

      Semaphore(int initial, int max);

      ~Semaphore();

      Semaphore & operator++();
      Semaphore & operator--();

    private:

      class Private;
      Private * d;
  };
}

#endif // __RIK_SEMAPHORE_H__
