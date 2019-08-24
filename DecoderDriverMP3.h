// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_DECODER_DRIVER_MP3_H
#define SQRADIO_DECODER_DRIVER_MP3_H

#include "DecoderDriver.h"

namespace SqRadio
{
  class DecoderDriverMP3 : public DecoderDriver
  {
    public:

      DecoderDriverMP3();
      virtual ~DecoderDriverMP3();

      virtual void run(QThreadPipe *);

    private:

      class Private;
      Private * d;
  };
}

#endif // SQRADIO_DECODER_DRIVER_MP3_H
