// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include <algorithm>
#include <iostream>
#include <cassert>
#include <qmutex.h>

#include "RingBuffer.h"

unsigned long min(unsigned long a, unsigned long b)
{
  return a < b ? a : b;
}

class RingBuffer::Private
{
  public:

    char            * start;
    char            * dataStart;
    char            * dataEnd;
    char            * end;

    QMutex mutex;
};

RingBuffer::RingBuffer(unsigned long size)
{
  d = new Private;

  d->start      = new char[size];
  d->end        = d->start + size;

  clear();
}

RingBuffer::~RingBuffer()
{
  delete [] d->start;
  delete d;
}

unsigned long RingBuffer::size() const
{
  return (d->end - d->start);
}

unsigned long RingBuffer::used() const
{
  if (d->dataEnd == d->dataStart)
    return size();

  if (d->dataEnd < d->dataStart)
    return (d->dataEnd - d->start) + (d->end - d->dataStart);

  return (d->dataEnd - d->dataStart);
}

unsigned long RingBuffer::available() const
{
  return size() - used();
}

bool RingBuffer::full() const
{
  return 0 == available();
}

bool RingBuffer::empty() const
{
  return (d->dataEnd == d->start) && (d->dataStart == d->end);
}

bool RingBuffer::write(const char * data, unsigned long byteCount)
{
  if (available() < byteCount)
    return false;

  std::cerr << "RingBuffer::write(" << byteCount << ")" << std::endl;
  std::cerr << " -> dataStart : " << int(d->dataStart - d->start) << std::endl;
  std::cerr << " -> dataEnd   : " << int(d->dataEnd - d->start) << std::endl;

  d->mutex.lock();

  if (d->dataStart < d->dataEnd)
  {
    std::cerr << "start < end" << std::endl;

    unsigned long bytesForSecondSection = min(d->end - d->dataEnd, byteCount);

    std::copy(data, data + bytesForSecondSection, d->dataEnd);

    d->dataEnd  += bytesForSecondSection;
    byteCount   -= bytesForSecondSection;

    std::cerr << "After writing second section:" << std::endl;
    std::cerr << " -> dataStart : " << int(d->dataStart - d->start) << std::endl;
    std::cerr << " -> dataEnd   : " << int(d->dataEnd - d->start) << std::endl;

    if (byteCount > 0)
    {
      unsigned long bytesForFirstSection =
        min(d->dataStart - d->start, byteCount);

      std::copy
        (
          data,
          data + bytesForFirstSection,
          d->start + bytesForSecondSection
        );

      d->dataEnd   = d->start + bytesForFirstSection;
      byteCount   -= bytesForFirstSection;

    std::cerr << "After writing first section:" << std::endl;
    std::cerr << " -> dataStart : " << int(d->dataStart - d->start) << std::endl;
    std::cerr << " -> dataEnd   : " << int(d->dataEnd - d->start) << std::endl;
    }

    assert(0 == byteCount);
  }
  else
  {
    if (empty())
      d->dataStart = d->dataEnd = d->start;

    std::cerr << "start > end" << std::endl;

    std::copy(data, data + byteCount, d->dataEnd);

    d->dataEnd += byteCount;

    std::cerr << "After writing:" << std::endl;
    std::cerr << " -> dataStart : " << int(d->dataStart - d->start) << std::endl;
    std::cerr << " -> dataEnd   : " << int(d->dataEnd - d->start) << std::endl;
  }

  d->mutex.unlock();

  return true;
}

bool RingBuffer::read(char * data, unsigned long byteCount)
{
  if (used() < byteCount)
    return false;

  std::cerr << "RingBuffer::read(" << byteCount << ")" << std::endl;
  std::cerr << " -> dataStart : " << int(d->dataStart - d->start) << std::endl;
  std::cerr << " -> dataEnd   : " << int(d->dataEnd - d->start) << std::endl;

  d->mutex.lock();

  if (d->dataStart < d->dataEnd)
  {
    std::copy(d->dataStart, d->dataStart + byteCount, data);
    d->dataStart += byteCount;

    if (d->dataStart == d->dataEnd)
    {
      d->dataStart  = d->end;
      d->dataEnd    = d->start;
    }

    std::cerr << "After reading:" << std::endl;
    std::cerr << " -> dataStart : " << int(d->dataStart - d->start) << std::endl;
    std::cerr << " -> dataEnd   : " << int(d->dataEnd - d->start) << std::endl;
  }
  else
  {
    unsigned long bytesToReadFromSecondSection =
      min(d->end - d->dataStart, byteCount);

    std::copy(d->dataStart, d->dataStart + bytesToReadFromSecondSection, data);

    d->dataStart  += bytesToReadFromSecondSection;
    byteCount     -= bytesToReadFromSecondSection;
    
    std::cerr << "After reading second section:" << std::endl;
    std::cerr << " -> dataStart : " << int(d->dataStart - d->start) << std::endl;
    std::cerr << " -> dataEnd   : " << int(d->dataEnd - d->start) << std::endl;

    unsigned long bytesToReadFromFirstSection =
      min(d->dataEnd - d->start, byteCount);

    std::copy
      (
        d->start,
        d->start + bytesToReadFromSecondSection,
        data + bytesToReadFromSecondSection
      );

    d->dataEnd    -= bytesToReadFromFirstSection;
    byteCount     -= bytesToReadFromFirstSection;

    std::cerr << "After reading first section:" << std::endl;
    std::cerr << " -> dataStart : " << int(d->dataStart - d->start) << std::endl;
    std::cerr << " -> dataEnd   : " << int(d->dataEnd - d->start) << std::endl;

    assert
      (
        (0 == byteCount)
        ||
        ((d->dataEnd == d->start) && (d->dataStart == d->end))
      );
  }
  d->mutex.unlock();

  return true;
}

void RingBuffer::clear()
{
  d->dataStart  = d->end;
  d->dataEnd    = d->start;
}

#ifdef TEST
int main()
{
  RingBuffer r(4);

  char buf[4];

  assert(4 == r.size());
  assert(4 == r.available());
  assert(0 == r.used());
  assert(r.write("abcd", 4));
  assert(0 == r.available());
  assert(4 == r.used());
  assert(r.read(buf, 4));
  assert(4 == r.available());
  assert(0 == r.used());
  assert(r.write("e", 1));
  assert(3 == r.available());
  assert(1 == r.used());
  r.clear();
  assert(0 == r.used());
  assert(4 == r.available());
}
#endif
