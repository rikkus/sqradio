// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include "BufferItem.h"
#include "BufferItemPool.h"

BufferItemPool * BufferItemPool::instance_ = 0L;

BufferItemPool::BufferItemPool(uint max)
  : head_   (0),
    tail_   (0),
    max_    (max),
    last_   (max - 1)
{
  pool_.setAutoDelete(true);

  for (uint i = 0; i < max_; i++)
    pool_.append(new BufferItem);
}

  BufferItem *
BufferItemPool::give()
{
  mutex_.lock();

  BufferItem * i(pool_.at(head_));
  _advance(head_);

  mutex_.unlock();

  return i;
}

  BufferItem *
BufferItemPool::take()
{
  mutex_.lock();

  BufferItem * i(pool_.at(tail_));
  _advance(tail_);

  mutex_.unlock();

  return i;
}

  BufferItem *
BufferItemPool::peek()
{
  mutex_.lock();

  BufferItem * i(pool_.at(tail_));

  mutex_.unlock();

  return i;
}

 bool
BufferItemPool::isEmpty() const
{
  mutex_.lock();

  bool empty = head_ == tail_;

  mutex_.unlock();

  return empty;
}

  bool
BufferItemPool::isFull() const
{
  mutex_.lock();

  bool full =
    (
    ((tail_ == 0) && (head_ == last_)) ||
    (tail_ == head_ + 1)
    );

  mutex_.unlock();

  return full;
}

  uint
BufferItemPool::count() const
{
  mutex_.lock();

  int c = (head_ < tail_) ? (last_ - head_ + tail_) : (head_ - tail_);

  mutex_.unlock();

  return c;
}

  void
BufferItemPool::flush()
{
  mutex_.lock();

  head_ = tail_ = 0;

  mutex_.unlock();
}

  void
BufferItemPool::_advance(uint & i)
{
  if (i == last_)
    i = 0;
  else
    ++i;
}

