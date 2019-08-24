// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef RING_BUFFER_H
#define RING_BUFFER_H

class RingBuffer
{
  public:

    RingBuffer(unsigned long size);
    ~RingBuffer();

    unsigned long size() const;
    unsigned long used() const;
    unsigned long available() const;

    bool full() const;
    bool empty() const;

    bool write(const char *, unsigned long);
    bool read(char *, unsigned long);
    void clear();

  private:

    class Private;
    Private * d;
};

#endif // RING_BUFFER_H
