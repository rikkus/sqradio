// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_BUFFER_ITEM_POOL_H
#define SQRADIO_BUFFER_ITEM_POOL_H

#include <qlist.h>
#include <qthread.h>
#include <qqueue.h>

class BufferItem;

class BufferItemPool
{
  public:

    static BufferItemPool * instance(uint max = 128)
    {
      if (0 == instance_)
        instance_ = new BufferItemPool(max);

      return instance_;
    }

    uint count() const;

    BufferItem * give();
    BufferItem * take();
    BufferItem * peek();

    void flush();

    bool isEmpty() const;
    bool isFull() const;

  private:

    void _advance(uint &);

    BufferItemPool(uint max);
    ~BufferItemPool();

    static BufferItemPool * instance_;

    uint head_;
    uint tail_;
    uint max_;
    uint last_;

    QList<BufferItem> pool_;

    mutable QMutex mutex_;
};

#endif // SQRADIO_BUFFER_ITEM_POOL_H
