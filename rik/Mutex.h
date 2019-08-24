// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2001 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef __RIK_MUTEX_H__
#define __RIK_MUTEX_H__

namespace rik
{
  class Mutex
  {
    public:

      Mutex();
      ~Mutex();

      void lock();
      void unlock();
      bool locked();
      bool lockNonBlocking();

    private:

      class Private;
      Private * d;

      pthread_mutex_t * mutex();

      friend class Semaphore;
  };
}

#endif // __RIK_MUTEX_H__
