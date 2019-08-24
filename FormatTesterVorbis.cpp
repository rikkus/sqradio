// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include "FormatTesterVorbis.h"

namespace SqRadio
{
  inline bool oggVorbis(char c0, char c1, char c2, char c3)
  {
    return
      (
        'O' == c0
        &&
        'g' == c1
        &&
        'g' == c2
        &&
        'S' == c3
      );
  }

  FormatTesterVorbis::FormatTesterVorbis(const QByteArray & a)
    : FormatTester()
  {
    if (a.size() >= 4)
      for (unsigned int i = 0; i < a.size() - 4; ++i)
        if (oggVorbis(a[i], a[i + 1], a[i + 2], a[i + 3]))
        {
          setAccepted(true);
          return;
        }

    setAccepted(false);
  }

  FormatTesterVorbis::~FormatTesterVorbis()
  {
    // Empty.
  }
}

