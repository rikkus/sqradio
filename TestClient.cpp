// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include <qfile.h>
#include <qtimer.h>

#include "TestClient.h"
#include "PCM.h"

namespace SqRadio
{
  class TestClient::Private
  {
    public:

      QFile file;
      QTimer timer;
      StationInfo stationInfo;
  };

  TestClient::TestClient(const StationInfo & stationInfo, QObject * parent)
    : QObject(parent)
  {
    d = new Private;
    d->stationInfo = stationInfo;

    connect
      (
        &d->timer,
        SIGNAL(timeout()),
        SLOT(slotReadyRead())
      );
  }

  TestClient::~TestClient()
  {
    delete d;
  }

  StationInfo TestClient::stationInfo() const
  {
    return d->stationInfo;
  }

  void TestClient::play()
  {
    d->file.setName("test.mp3");

    if (!d->file.open(IO_ReadOnly))
    {
      qDebug("Can't open test.mp3");
      return;
    }

    emit(hostFound());
    emit(connected());

    d->timer.start(1000, true);
  }

  void TestClient::slotReadyRead()
  {
    if (bytesAvailable() > 0)
      emit(dataAvailable());

    else
    {
      qDebug("EOF on input");
    }
  }

  unsigned int TestClient::bytesAvailable() const
  {
    return d->file.size() - d->file.at();
  }

  QByteArray TestClient::readBlock(unsigned int maxBytes)
  {
    unsigned int bytesToRead = QMIN(maxBytes, bytesAvailable());

    QByteArray buf(bytesToRead);

    int len = d->file.readBlock(buf.data(), buf.size());

    if (len != int(bytesToRead))
    {
      qDebug("TestClient::slotReadyRead: readBlock failed");
      return QByteArray();
    }

    return buf;
  }
}

