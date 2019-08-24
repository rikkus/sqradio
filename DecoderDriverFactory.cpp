// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include "DecoderDriverFactory.h"
#include "DecoderDriver.h"
#include "DecoderDriverMP3.h"
#include "DecoderDriverVorbis.h"
#include "FormatTesterMP3.h"
#include "FormatTesterVorbis.h"

namespace SqRadio
{
  namespace DecoderDriverFactory
  {
    DecoderDriver * create(const QByteArray & a)
    {
      if (FormatTesterVorbis(a).accepted())
        return new DecoderDriverVorbis;

      if (FormatTesterMP3(a).accepted())
        return new DecoderDriverMP3;

      qDebug("DecoderDriverFactory::create: No-one knows this format.");
      return 0;
    }
  }
}


