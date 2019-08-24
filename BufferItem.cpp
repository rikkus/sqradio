// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include "BufferItem.h"

namespace SqRadio
{
  uint  BufferItem::maxLength_  = 4096;
  int   BufferItem::ID_         = 0;

  BufferItem::BufferItem()
    : length_       (0),
      time_         (-1),
      trackLength_  (-1),
      bitRate_      (-1),
      sampleRate_   (-1),
      channels_     (-1),
      trackID_      (QString::null),
      id_           (ID_++)
  {
    buffer_ = new char[maxLength_];
  }

  BufferItem::~BufferItem()
  {
    delete [] buffer_;
  }
}
