// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2002 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef __RIK_SHARED_MEMORY_SEGMENT_H__
#define __RIK_SHARED_MEMORY_SEGMENT_H__

namespace rik
{
  class SharedMemorySegment
  {
    public:

      static SharedMemorySegment * find(int id);
      static SharedMemorySegment * create(unsigned int size);

      int             id() const;
      void          * address();
      unsigned int    size() const;
      bool            owner() const;

      virtual ~SharedMemorySegment();

    private:

      SharedMemorySegment(int id, void * address, unsigned int size, bool own);

      class Private;
      Private * d;
  };
}

#endif // __RIK_SHARED_MEMORY_SEGMENT_H__
