// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_OUTPUT_DRIVER_OSS_H
#define SQRADIO_OUTPUT_DRIVER_OSS_H

#ifdef SQRADIO_USE_DRIVER_OSS

#include <qstringlist.h>

#include "OutputDriver.h"

namespace SqRadio
{
  class OutputDriverOSS : public OutputDriver
  {
    public:

      OutputDriverOSS();
      virtual ~OutputDriverOSS();

      bool open(PCM::Format format);
      bool write(const char * data, unsigned int size);
      void close();

      bool isOpen() const;

    private:

      class Private;
      Private * d;
  };
}

#endif // SQRADIO_USE_DRIVER_OSS

#endif // SQRADIO_OUTPUT_DRIVER_OSS_H
