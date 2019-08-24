// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_FORMAT_TESTER_MP3_H
#define SQRADIO_FORMAT_TESTER_MP3_H

#include "FormatTester.h"

namespace SqRadio
{
  class FormatTesterMP3 : public FormatTester
  {
    public:

      FormatTesterMP3(const QByteArray &);
      virtual ~FormatTesterMP3();
  };
}

#endif // SQRADIO_FORMAT_TESTER_MP3_H
