// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include <qobject.h>
#include <qsocket.h>

#include "Utils.h"

namespace SqRadio
{
  QValueList<unsigned int> levelList(const QByteArray & buffer)
  {
    unsigned int sampleCount = ((buffer.size() / 2) / 4) * 4;

    Q_UINT16 * data = reinterpret_cast<Q_UINT16 *>(buffer.data());

    Q_UINT16 lavg = 0;
    Q_UINT16 ravg = 0;

    for (uint i = 0; i < sampleCount; i += 2)
    {
      Q_UINT16 l = data[i];
      Q_UINT16 r = data[i + 1];

      lavg = uint(((99.0 * lavg) + l) / 100.0);
      ravg = uint(((99.0 * ravg) + r) / 100.0);
    }

    lavg = uint((lavg / 65535.f) * 99);
    ravg = uint((ravg / 65535.f) * 99);

    QValueList<unsigned int> l;
    l << lavg << ravg;
    return l;
  }

  QString qSocketErrorToString(int i)
  {
    switch (i)
    {
      case QSocket::ErrConnectionRefused:
        return QObject::tr("Connection refused");
        break;

      case QSocket::ErrHostNotFound:
        return QObject::tr("Host not found");
        break;

      case QSocket::ErrSocketRead:
        return QObject::tr("Unable to read data from host");
        break;

      default:
        return QObject::tr("Unknown error");
        break;
    }
  }

  QByteArray & fadeOut(QByteArray & buf, double startPower, double endPower)
  {
    const unsigned int SampleCount = buf.size() / 2;
    short * shortStart = reinterpret_cast<short *>(buf.data());
    short * shortEnd = shortStart + SampleCount;

    double delta = (startPower - endPower) / double(SampleCount);
    double multiplier = startPower;

    for (short * cursor = shortStart; cursor < shortEnd; ++cursor)
    {
      *cursor = short(*cursor * multiplier);
      multiplier -= delta;
    }

    return buf;
  }
}

