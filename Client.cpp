// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include <unistd.h>

#include <qsocket.h>
#include <qstringlist.h>
#include <qtimer.h>

#include "ByteArray.h"
#include "UDPListener.h"
#include "Client.h"
#include "PCM.h"
#include "Utils.h"

namespace SqRadio
{
  class Client::Private
  {
    public:

      QSocket           * socket;
      UDPListener       * udpListener;
      StationInfo         stationInfo;
      bool                icecastStyleMetaData;
      unsigned int        icyMetaInt;
      Client::WaitingFor  waitingFor;
      unsigned int        bytesBeforeChange;

      QByteArray          buffer;
      QByteArray          metaDataBuffer;

      QMap<QString, QString> streamInfo;

      unsigned int        maxBufferSize;

      bool                bufferFilled;

      unsigned int        redirectCount;

      unsigned int        udpPort;
  };

  Client::Client(const StationInfo & stationInfo, QObject * parent)
    : QObject(parent)
  {
    d = new Private;

    d->udpPort              = 32695;
    d->udpListener          = new UDPListener(d->udpPort, this);
    d->socket               = new QSocket(this);
    d->stationInfo          = stationInfo;
    d->icecastStyleMetaData = false;
    d->icyMetaInt           = 0;
    d->waitingFor           = WaitingForHeaders;
    d->bytesBeforeChange    = 0;
    d->maxBufferSize        = DefaultMaxBufferSize;
    d->bufferFilled         = false;
    d->redirectCount        = 0;

    connect
      (
        d->udpListener,
        SIGNAL(message(const QByteArray &)),
        SLOT(slotUDPMessage(const QByteArray &))
      );

    connect
      (
        d->socket,
        SIGNAL(hostFound()),
        SLOT(slotHostFound())
      );

    connect
      (
        d->socket,
        SIGNAL(connected()),
        SLOT(slotConnected())
      );

    connect
      (
        d->socket,
        SIGNAL(readyRead()),
        SLOT(slotReadyRead())
      );

    connect
      (
        d->socket,
        SIGNAL(error(int)),
        SLOT(slotError(int))
      );
  }

  Client::~Client()
  {
    delete d;
  }

  StationInfo Client::stationInfo() const
  {
    return d->stationInfo;
  }

  void Client::play()
  {
    d->socket->connectToHost
      (d->stationInfo.url().host(), d->stationInfo.url().port());
  }

  void Client::slotHostFound()
  {
    emit(hostFound());
  }

  void Client::slotConnected()
  {
    write("GET " + d->stationInfo.url().encodedPathAndQuery() + " HTTP/1.0");
    write("Host: " + d->stationInfo.url().host());
    write("User-Agent: sqradio/0.0");
    write("Icy-MetaData: 1");
    write("x-audiocast-udpport: " + QString::number(d->udpPort));
    write("");

    emit(connected());
  }

  void Client::slotConnectionClosed()
  {
    emit(connectionClosed());
  }

  void Client::slotReadyRead()
  {
    while
      (
        (d->buffer.size() < maxBufferSize())
        &&
        (0 != d->socket->bytesAvailable())
      )
    {
      switch (d->waitingFor)
      {
        case WaitingForHeaders:
          if (!d->socket->canReadLine())
            return;
          readHeader();
          break;

        case WaitingForMetaDataSize:
          readMetaDataSize();
          break;

        case WaitingForMetaData:
          readMetaData();
          break;

        case WaitingForData:
          readData();
          break;

        default:
          qDebug("Client::slotReadyRead(): Hmm, waiting for what?");
          break;
      }
    }
  }

  void Client::readHeader()
  {
    QString s(d->socket->readLine().stripWhiteSpace());

    if ("ICY 200" == s.left(7))
    {
      // Looks like we will get metadata.
    }

    QStringList l(QStringList::split(" ", s));

    if ("302" == l[1])
    {
      if (d->redirectCount > 5)
      {
        qDebug("Too many redirects");
        d->socket->close();
        return;
      }
      ++(d->redirectCount);
      return;
    }

    if ((d->redirectCount > 0) && "Location:" == s.left(9))
    {
      d->socket->close();
      d->stationInfo.setUrl(QUrl(s.mid(9)));
      d->socket->connectToHost
        (d->stationInfo.url().host(), d->stationInfo.url().port());
      return;
    }

    QString key;
    QString value;

    int colonPos = s.find(':');

    if (-1 != colonPos)
    {
      key   = s.left(colonPos).stripWhiteSpace().lower();
      value = s.mid(colonPos + 1).stripWhiteSpace();

      if ("icy-metaint" == key)
      {
        // We really are going to get metadata.

        d->icecastStyleMetaData = true;

        d->icyMetaInt = s.mid(12).stripWhiteSpace().toUInt();

        d->bytesBeforeChange = d->icyMetaInt;
      }
      else if ("icy-name" == key)
      {
        d->streamInfo["Name"] = value;
      }
      else if ("icy-bitrate" == key)
      {
        d->streamInfo["Bitrate"] = value;
      }
      else if ("icy-url" == key)
      {
        d->streamInfo["URL"] = value;
      }
      else if ("icy-br" == key)
      {
        d->streamInfo["Bitrate"] = value;
      }
      else if ("icy-notice" == key.left(10))
      {
        // Who cares?
      }
    }

    if (s.isEmpty()) 
    {
      d->waitingFor = WaitingForData;
      emit(buffering());

      emit(streamInfo(d->streamInfo));
    }
  }

  void Client::readMetaDataSize()
  {
    unsigned char metaDataSize;

    d->socket->readBlock(reinterpret_cast<char *>(&metaDataSize), 1);

    d->bytesBeforeChange = metaDataSize * 16;

    d->metaDataBuffer.resize(0);

    d->waitingFor = WaitingForMetaData;
  }

  void Client::readMetaData()
  {
    unsigned int bytesToRead =
      QMIN(d->socket->bytesAvailable(), d->bytesBeforeChange);

    readTo(d->socket, d->metaDataBuffer, bytesToRead);

    d->bytesBeforeChange -= bytesToRead;

    if (0 == d->bytesBeforeChange)
    {
      d->bytesBeforeChange = d->icyMetaInt;
      d->waitingFor = WaitingForData;

      if (!d->metaDataBuffer.isEmpty())
      {
        parseMetaData();
        emit(streamInfo(d->streamInfo));
      }
    }
  }

  void Client::readData()
  {
    if (d->icecastStyleMetaData)
    {
      unsigned int bytesToRead =
        QMIN(d->socket->bytesAvailable(), d->bytesBeforeChange);

      readTo(d->socket, d->buffer, bytesToRead);

      d->bytesBeforeChange -= bytesToRead;

      if (0 == d->bytesBeforeChange)
      {
        if (d->icecastStyleMetaData)
        {
          d->waitingFor = WaitingForMetaDataSize;
          d->bytesBeforeChange = 0;
        }
        else
        {
          d->waitingFor = WaitingForData;
          d->bytesBeforeChange = d->icyMetaInt;
        }
      }
    }
    else
    {
      readTo(d->socket, d->buffer, d->socket->bytesAvailable());
      d->waitingFor = WaitingForData;
    }

    emit(bufferPercent(_bufferPercent()));

    if (d->buffer.size() >= d->maxBufferSize)
    {
      d->bufferFilled = true;
    }

    emit(dataAvailable());
  }

  void Client::slotError(int i)
  {
    qDebug("Client::slotError(%d)", i);
    emit(error(i));
  }

  void Client::write(const QString & s)
  {
    QCString data = QCString(s.latin1()) + "\r\n";

    d->socket->writeBlock(data.data(), data.length());
  }

  unsigned int Client::bytesAvailable() const
  {
    return d->buffer.size();
  }

  QByteArray Client::readBlock(unsigned int maxBytes)
  {
    unsigned int bytesToRead = QMIN(maxBytes, d->buffer.size());

    QByteArray ret = cut(d->buffer, bytesToRead);

    if (0 != d->socket->bytesAvailable())
      QTimer::singleShot(0, this, SLOT(slotReadyRead()));

    if (d->buffer.isEmpty())
    {
      d->bufferFilled = false;
      emit(buffering());
    }

    emit(bufferPercent(_bufferPercent()));

    return ret;
  }

  QByteArray Client::peekBlock(unsigned int maxBytes)
  {
    unsigned int bytesToRead = QMIN(maxBytes, d->buffer.size());

    QByteArray ret(bytesToRead);

    memcpy(ret.data(), d->buffer.data(), bytesToRead);

    return ret;
  }

  void Client::setMaxBufferSize(unsigned int i)
  {
    d->maxBufferSize = i;
  }

  unsigned int Client::maxBufferSize() const
  {
    return d->maxBufferSize;
  }

  void Client::parseMetaData()
  {
    QString s =
      QString::fromLatin1(d->metaDataBuffer.data(), d->metaDataBuffer.size());

    if (s.startsWith("StreamTitle=")) 
    {
      s.remove(0, 13);

      int i = s.find("'");

      if (-1 != i)
        d->streamInfo["Title"] = s.left(i);
    }
  }

  void Client::slotUDPMessage(const QByteArray & data)
  {
    ::write(1, data.data(), data.size());
    int i;

    QString s = QString::fromLatin1(data.data(), data.size());

    if (s.startsWith("StreamTitle=")) 
    {
      s.remove(0, 13);

      i = s.find("'");

      if (-1 != i)
        d->streamInfo["Name"] = s.left(i - 1);
    }
    else if (-1 != (i = s.find("x-audiocast-streamtitle:")))
    {
      s.remove(0, i + 24);

      i = s.find('\n');

      if (-1 != i)
        d->streamInfo["Name"] = s.left(i - 1);
    }

    emit(streamInfo(d->streamInfo));
  }


  unsigned int Client::_bufferPercent() const
  {
    return uint(double(bytesAvailable() / double(maxBufferSize())) * 100);
  }
}

#if 0
  void Client::lookForAudioHeader()
  {
    //qDebug("lookForAudioHeader()");

    if (d->buffer.size() < 4)
      return;

    bool mp3    = false;
    bool vorbis = false;

    unsigned char c0, c1, c2, c3;

    for (unsigned int i = 0; i < d->buffer.size() - 4; ++i)
    {
      c0 = d->buffer[i];
      c1 = d->buffer[i + 1];
      c2 = d->buffer[i + 2];
      c3 = d->buffer[i + 3];

      mp3    = frameSync(c0) && mpeg1Layer3(c1);
      vorbis = oggVorbis(c0, c1, c2, c3);

      if (mp3 || vorbis)
      {
        d->foundAudioHeader = true;
        cut(d->buffer, i);
        break;
      }
    }

    if (!mp3 && !vorbis)
    {
      qDebug("Hmm, not MP3 or Vorbis.");
    }

    emit(format(mp3 ? MP3 : Vorbis));
  }
#endif

