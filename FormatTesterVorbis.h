// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_FORMAT_TESTER_VORBIS_H
#define SQRADIO_FORMAT_TESTER_VORBIS_H

#include "FormatTester.h"

namespace SqRadio
{
  class FormatTesterVorbis : public FormatTester
  {
    public:

      FormatTesterVorbis(const QByteArray &);
      virtual ~FormatTesterVorbis();
  };
}

#endif // SQRADIO_FORMAT_TESTER_VORBIS_H
