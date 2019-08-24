// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include "FormatTesterMP3.h"

namespace SqRadio
{
  inline bool mp3FrameSync(unsigned char c)
  {
    return 0xFF == c;
  }

  inline bool mpeg1Layer3(unsigned char c)
  {
    return true;
//    return ((0xFA == c) || (0xFB == c));
  }

  FormatTesterMP3::FormatTesterMP3(const QByteArray & a)
    : FormatTester()
  {
    if (a.size() >= 2)
      for (unsigned int i = 0; i < a.size() - 2; ++i)
        if (mp3FrameSync(a[i]) && mpeg1Layer3(a[i + 1]))
        {
          setAccepted(true);
          return;
        }

    setAccepted(false);
  }

  FormatTesterMP3::~FormatTesterMP3()
  {
    // Empty.
  }
}

