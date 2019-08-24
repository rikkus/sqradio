// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include "DecoderDriver.h"

namespace SqRadio
{
  class DecoderDriver::Private
  {
    int dummy;
  };

  DecoderDriver::DecoderDriver()
  {
    d = new Private;
  }

  DecoderDriver::~DecoderDriver()
  {
    delete d;
  }

}
