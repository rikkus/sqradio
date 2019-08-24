// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_DECODER_DRIVER_H
#define SQRADIO_DECODER_DRIVER_H

#include <qcstring.h>

class QThreadPipe;

namespace SqRadio
{
  class DecoderDriver
  {
    public:

      DecoderDriver();
      virtual ~DecoderDriver();

      virtual void run(QThreadPipe *) = 0;

    private:

      class Private;
      Private * d;
  };
}

#endif // SQRADIO_DECODER_DRIVER_H
