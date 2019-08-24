// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include "OutputDriverNULL.h"

namespace SqRadio
{
  class OutputDriverNULL::Private
  {
    public:

      bool open;
      PCM::Format format;
  };

  OutputDriverNULL::OutputDriverNULL()
    : OutputDriver()
  {
    d = new Private;
  }

  OutputDriverNULL::~OutputDriverNULL()
  {
    delete d;
  }

  bool OutputDriverNULL::open(PCM::Format f)
  {
    d->format = f;
    d->open   = true;

    return true;
  }

  bool OutputDriverNULL::write(const char *, unsigned int byteCount)
  {
    usleep
      (
        uint((byteCount / d->format.bytesPerSample) / double(d->format.rate))
        *
        1000000
      );

    return true;
  }

  void OutputDriverNULL::close()
  {
    d->open = false;
  }

  bool OutputDriverNULL::isOpen() const
  {
    return d->open;
  }
}

