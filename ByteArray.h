// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_BYTE_ARRAY_H
#define SQRADIO_BYTE_ARRAY_H

#include <qcstring.h>
#include <iostream>

class QIODevice;

namespace SqRadio
{
  void readTo
    (
      QIODevice   * ioDevice,
      QByteArray  & buffer,
      unsigned int  count
    );

  QByteArray cut
    (
      QByteArray & source,
      unsigned int begin,
      unsigned int size
    );

  QByteArray cut
    (
      QByteArray & source,
      unsigned int size
    );

  QByteArray & append
    (
      const QByteArray & source,
      QByteArray & destination
    );

  QByteArray & append
    (
      const QByteArray & source,
      QByteArray & destination,
      unsigned int count
    );

  std::ostream & operator << (std::ostream & s, const QByteArray & a);
}

#endif // SQRADIO_BYTE_ARRAY_H
