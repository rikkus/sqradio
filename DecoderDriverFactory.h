// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_DECODER_DRIVER_FACTORY_H
#define SQRADIO_DECODER_DRIVER_FACTORY_H

#include <qcstring.h>

namespace SqRadio
{
  class DecoderDriver;

  namespace DecoderDriverFactory
  {
    DecoderDriver * create(const QByteArray &);
  }
}

#endif // SQRADIO_DECODER_DRIVER_FACTORY_H
