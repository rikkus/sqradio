// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_OUTPUT_DRIVER_H
#define SQRADIO_OUTPUT_DRIVER_H

#include "PCM.h"

class QThread;

namespace SqRadio
{
  class OutputDriver
  {
    public:

      OutputDriver();
      virtual ~OutputDriver();

      virtual bool open(PCM::Format format)                       = 0;
      virtual bool write(const char * data, unsigned int size)    = 0;
      virtual void close()                                        = 0;
      virtual bool isOpen() const                                 = 0;

      void setThread(QThread *);
      void usleep(unsigned int);

    private:

      class Private;
      Private * d;
  };
}

#endif // SQRADIO_OUTPUT_DRIVER_H
