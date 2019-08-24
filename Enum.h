// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_ENUM_H
#define SQRADIO_ENUM_H

#include <qcstring.h>

namespace SqRadio
{
  namespace Encoding
  {
    enum Format
    {
      Unknown,
      Vorbis,
      MP3
    };
  }

  Encoding::Format format(const QByteArray & buf);

  namespace Status
  {
    enum Value
    {
      Stopped,
      Contacting,
      Buffering,
      Playing
    };
  }
}

#endif // SQRADIO_ENUM_H
