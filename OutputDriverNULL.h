// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_OUTPUT_DRIVER_NULL_H
#define SQRADIO_OUTPUT_DRIVER_NULL_H

#include <qstringlist.h>

#include "OutputDriver.h"

namespace SqRadio
{
  class OutputDriverNULL : public OutputDriver
  {
    public:

      OutputDriverNULL();
      virtual ~OutputDriverNULL();

      bool open(PCM::Format format);
      bool write(const char * data, unsigned int size);
      void close();

      bool isOpen() const;

    private:

      class Private;
      Private * d;
  };
}

#endif // SQRADIO_OUTPUT_DRIVER_NULL_H
