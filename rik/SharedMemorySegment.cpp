// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2002 Rik Hemsley (rikkus) <rik@kde.org>

#include <cerrno>
#include <iostream>
#include <cstdio>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "rik/SharedMemorySegment.h"

namespace rik
{
  class SharedMemorySegment::Private
  {
    public:

      int           id;
      void        * address;
      unsigned int  size;
      bool          owner;
  };

  SharedMemorySegment::SharedMemorySegment
    (
      int           id,
      void        * address,
      unsigned int  size,
      bool          owner
    )
  {
    d = new Private;

    d->id       = id;
    d->address  = address;
    d->size     = size;
    d->owner    = owner;
  }

  SharedMemorySegment::~SharedMemorySegment()
  {
    if (d->owner)
    {
      struct shmid_ds buf;

      if (-1 == shmctl(d->id, IPC_RMID, &buf))
      {
        perror("shmctl");
        std::cerr << "Couldn't remove segment - already removed?" << std::endl;
      }
    }

    if (-1 == shmdt(d->address))
    {
      perror("shmdt");
      std::cerr << "Couldn't detach from segment" << std::endl;
    }


    delete d;
  }

  SharedMemorySegment * SharedMemorySegment::find(int id)
  {
    struct shmid_ds buf;

    int ret = shmctl(id, IPC_STAT, &buf);

    if (-1 == ret)
    {
      perror("shmctl");
      return 0;
    }

    void * address = shmat(id, 0, 0);

    if (-1 == int(address))
    {
      perror("shmat");
      return 0;
    }

    return new SharedMemorySegment(id, address, buf.shm_segsz, false);
  }

  SharedMemorySegment * SharedMemorySegment::create(unsigned int size)
  {
    int id = shmget(IPC_PRIVATE, size, IPC_CREAT | 0600);

    if (-1 == id)
    {
      perror("shmget");
      return 0;
    }

    struct shmid_ds buf;

    int ret = shmctl(id, IPC_STAT, &buf);

    if (-1 == ret)
    {
      perror("shmctl");
      return 0;
    }

    void * address = shmat(id, 0, 0);

    if (-1 == int(address))
    {
      perror("shmat");
      return 0;
    }

    return new SharedMemorySegment(id, address, buf.shm_segsz, true);
  }

  int SharedMemorySegment::id() const
  {
    return d->id;
  }

  void * SharedMemorySegment::address()
  {
    return d->address;
  }

  unsigned int SharedMemorySegment::size() const
  {
    return d->size;
  }

  bool SharedMemorySegment::owner() const
  {
    return d->owner;
  }
}

#ifdef TEST
#include <unistd.h>
#include <sys/wait.h>
#include "rik/Thread.h"

class Child : public rik::Thread
{
  public:

    Child(int id)
    {
      seg = rik::SharedMemorySegment::find(id);
    }

    ~Child()
    {
      delete seg;
    }

    void run()
    {
    }

  private:

    rik::SharedMemorySegment * seg;
};

int main()
{
  rik::SharedMemorySegment * seg =
    rik::SharedMemorySegment::create(1024);

  if (0 == seg)
  {
    std::cerr << "Couldn't create shm seg" << std::endl;
    return 1;
  }

  Child * child = new Child(seg->id());

  child->start();
  child->join();

  delete seg;
}

#endif // TEST
