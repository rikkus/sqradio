// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include "OutputDriver.h"
#include "OutputThread.h"

namespace SqRadio
{
  class OutputDriver::Private
  {
    public:

      QThread * thread;
  };

  OutputDriver::OutputDriver()
  {
    d = new Private;
    d->thread = 0;
  }

  OutputDriver::~OutputDriver()
  {
    delete d;
  }

  void OutputDriver::setThread(QThread * t)
  {
    d->thread = t;
  }

  void OutputDriver::usleep(unsigned int i)
  {
    if (0 != d->thread)
      static_cast<OutputThread *>(d->thread)->publicUSleep(i);
  }
}
