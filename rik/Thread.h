// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2001 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef __RIK_THREAD_H__
#define __RIK_THREAD_H__

namespace rik
{
  namespace ThreadStart
  {
    enum Return
    {
      Success,
      UnableToInitialiseAttributes,
      TooFewSystemResources,
      UnableToCreatePipe,
      Unknown
    };
  }

  namespace ThreadJoin
  {
    enum Return
    {
      Success,
      NoSuchThread,
      DetachedOrAnotherThreadWaiting,
      CannotJoinSelf,
      Unknown
    };
  }

  namespace ThreadDetach
  {
    enum Return
    {
      Success,
      NoSuchThread,
      Unknown
    };
  }

  class Thread
  {
    public:

      Thread();
      virtual ~Thread();

      virtual void run() = 0;

      ThreadStart::Return   start();
      ThreadJoin::Return    join();
      ThreadDetach::Return  detach();

    private:

      class Private;
      Private * d;
  };
}

#endif // __RIK_THREAD_H__
