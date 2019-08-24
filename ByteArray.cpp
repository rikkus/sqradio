// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include <cstring>
#include <cctype>
#include <qiodevice.h>

#include "Defines.h"
#include "ByteArray.h"

namespace SqRadio
{
  void readTo
    (
      QIODevice   * ioDevice,
      QByteArray  & buffer,
      unsigned int  size
    )
  {
    unsigned int oldBufferSize = buffer.size();
    buffer.resize(oldBufferSize + size);

    ioDevice->readBlock(buffer.data() + oldBufferSize, size);
  }

  QByteArray cut
    (
      QByteArray & a,
      unsigned int begin,
      unsigned int size
    )
  {
    QByteArray ret(size);
    memcpy(ret.data(), a.data() + begin, size);
    memmove(a.data() + begin, a.data() + begin + size, a.size() - size);
    a.resize(a.size() - size);
    return ret;
  }

  QByteArray cut
    (
      QByteArray & a,
      unsigned int size
    )
  {
    QByteArray ret(size);
    memcpy(ret.data(), a.data(), size);
    memmove(a.data(), a.data() + size, a.size() - size);
    a.resize(a.size() - size);
    return ret;
  }

  QByteArray & append
    (
      const QByteArray & source,
      QByteArray & destination
    )
  {
    unsigned int oldDestinationSize = destination.size();

    destination.resize(oldDestinationSize + source.size());

    memcpy
      (destination.data() + oldDestinationSize, source.data(), source.size());

    return destination;
  }

  QByteArray & append
    (
      const QByteArray & source,
      QByteArray & destination,
      unsigned int count
    )
  {
    unsigned int oldDestinationSize = destination.size();

    destination.resize(oldDestinationSize + count);

    memcpy(destination.data() + oldDestinationSize, source.data(), count);

    return destination;
  }

  void moveFrontToEnd
    (
      QByteArray    & source,
      QByteArray    & destination,
      unsigned int    byteCount
    )
  {
    unsigned int originalDestinationSize = destination.size();

    destination.resize(originalDestinationSize + byteCount);

    memcpy
      (
        destination.data() + originalDestinationSize, 
        source.data(),
        byteCount
      );

    memmove
      (
        source.data(),
        source.data() + byteCount,
        source.size() - byteCount
      );

    source.resize(source.size() - byteCount);
  }

  std::ostream & operator << (std::ostream & s, const QByteArray & a)
  {
    for (unsigned int i = 0; i < a.size(); ++i)
    {
      if (isalnum(a[i]))
        s << a[i];
      else
        s << '[' << static_cast<unsigned int>(a[i]) << ']';
    }

    return s;
  }
}
