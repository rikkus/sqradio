// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_CLIENT_H
#define SQRADIO_CLIENT_H

#include <qobject.h>
#include <qurl.h>
#include <qcstring.h>
#include <qstring.h>
#include <qmap.h>

#include "Enum.h"
#include "StationInfo.h"

namespace SqRadio
{
  class Client : public QObject
  {
    Q_OBJECT

    public:

      Client(const StationInfo & stationInfo, QObject * parent = 0);
      virtual ~Client();

      void play();

      StationInfo stationInfo() const;

      QByteArray readBlock(unsigned int maxSize = 4096);
      QByteArray peekBlock(unsigned int maxSize = 4096);

      unsigned int bytesAvailable() const;

      void setMaxBufferSize(unsigned int);
      unsigned int maxBufferSize() const;

    protected slots:

      void slotHostFound();
      void slotConnected();
      void slotConnectionClosed();
      void slotError(int);
      void slotReadyRead();
      void slotUDPMessage(const QByteArray & data);

    signals:

      void hostFound();
      void connected();
      void connectionClosed();
      void error(int);

      void dataAvailable();
      void streamInfo(QMap<QString, QString>);

      void redirected(const QUrl &);

      void buffering();

      void bufferPercent(unsigned int);

    protected:

      void write(const QString &);

      void readHeader();
      void readMetaDataSize();
      void readMetaData();
      void readData();

      void lookForAudioHeader();

      void parseMetaData();
      unsigned int _bufferPercent() const;

    private:

      enum
      {
        DefaultMaxBufferSize = 128 * 1024
      };

      enum WaitingFor
      {
        WaitingForHeaders,
        WaitingForMetaDataSize,
        WaitingForMetaData,
        WaitingForData
      };

      class Private;
      Private * d;
  };
}

#endif // SQRADIO_CLIENT_H
