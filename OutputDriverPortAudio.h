// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_OUTPUT_DRIVER_PORT_AUDIO_H
#define SQRADIO_OUTPUT_DRIVER_PORT_AUDIO_H

#ifdef SQRADIO_USE_DRIVER_PORT_AUDIO

#include <qstringlist.h>

#include "OutputDriver.h"

class RingBuffer;

namespace SqRadio
{
  class OutputDriverPortAudio : public OutputDriver
  {
    public:

      OutputDriverPortAudio();
      virtual ~OutputDriverPortAudio();

      bool open(PCM::Format format);
      bool write(const char * data, unsigned int size);
      void close();

      bool isOpen() const;

      RingBuffer * ringBuffer();
      PCM::Format format() const;
      void underrun();

    private:

      class Private;
      Private * d;
  };
}

#endif // SQRADIO_USE_DRIVER_PORT_AUDIO

#endif // SQRADIO_OUTPUT_DRIVER_PORT_AUDIO_H
