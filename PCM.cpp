// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include "PCM.h"

#include <algorithm>

namespace SqRadio
{
  class PCM::Private
  {
    public:

      Format        format;
      char *        data;
      unsigned int  sampleCount;
  };

  PCM::PCM()
  {
    d = new Private;

    d->format.channelCount    = 0;
    d->format.bytesPerSample  = 0;
    d->format.signedness      = Format::Signed;
    d->format.byteOrder       = Format::BigEndian;
    d->data                   = 0;
    d->sampleCount            = 0;
  }

  PCM::PCM
    (
      Format          format,
      unsigned int    sampleCount
    )
  {
    d = new Private;

    d->format       = format;
    d->sampleCount  = sampleCount;
    d->data         = new char[size()];
  }

  PCM::PCM
  (
    Format        format,
    const char  * data,
    unsigned int  sampleCount
  )
  {
    d = new Private;

    d->format       = format;
    d->sampleCount  = sampleCount;

    d->data = new char[size()];

    std::copy(data, data + size(), d->data);
  }

  PCM::PCM(const PCM & other)
  {
    d = new Private;

    d->format       = other.d->format;
    d->sampleCount  = other.d->sampleCount;

    d->data = new char[other.size()];

    std::copy(other.d->data, other.d->data + other.size(), d->data);
  }

  PCM::~PCM()
  {
    delete d->data;
    delete d;
  }

    PCM::PCM &
  PCM::operator = (const PCM & other)
  {
    delete d->data;
    d->data = new char[other.size()];
    std::copy(other.d->data, other.d->data + other.size(), d->data);
    return *this;
  }

    PCM::Format
  PCM::format() const
  {
    return d->format;
  }

    unsigned int
  PCM::sampleCount() const
  {
    return d->sampleCount;
  }

    unsigned int
  PCM::size() const
  {
    return d->format.channelCount * d->format.bytesPerSample * d->sampleCount;
  }

    const char *
  PCM::data() const
  {
    return d->data;
  }

    char *
  PCM::data()
  {
    return d->data;
  }

    void
  PCM::setFormat(Format format)
  {
    d->format = format;
  }
}

