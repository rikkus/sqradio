// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_DECODER_DRIVER_VORBIS_H
#define SQRADIO_DECODER_DRIVER_VORBIS_H

#include "DecoderDriver.h"

class QThreadPipe;

namespace SqRadio
{
  class DecoderDriverVorbis : public DecoderDriver
  {
    public:

      DecoderDriverVorbis();
      virtual ~DecoderDriverVorbis();

      virtual void run(QThreadPipe *);

    private:

      class Private;
      Private * d;
  };
}

#endif // SQRADIO_DECODER_DRIVER_VORBIS_H
