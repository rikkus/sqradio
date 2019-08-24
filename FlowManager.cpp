// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include <qdatetime.h>
#include <qtimer.h>
#include <qvaluelist.h>

#include "FlowManager.h"
#include "ByteArray.h"
#include "StationInfo.h"
#include "Client.h"
#include "Decoder.h"
#include "Output.h"
#include "Utils.h"

namespace SqRadio
{
  class FlowManager::Private
  {
    public:

      Client        * client;
      Decoder       * decoder;
      Output        * output;

      StationInfo     stationInfo;

      QTime           lastFeedbackTime;

      bool            buffering;

      QTimer          flowTimer;
  };
    
  FlowManager::FlowManager(QObject * parent)
    : QObject(parent)
  {
    d = new Private;

    d->client     = 0;

    d->decoder  = new Decoder(this);
    d->output   = new Output(this);

    d->lastFeedbackTime = QTime::currentTime();

    d->buffering      = false;

    connect
      (
        d->decoder,
        SIGNAL(readyForData()),
        SLOT(slotDecoderReady())
      );

    connect
      (
        d->decoder,
        SIGNAL(readyRead()),
        SLOT(slotPCMDataAvailable())
      );

    connect
      (
        d->decoder,
        SIGNAL(streamInfo(QMap<QString, QString>)),
        SLOT(slotDecoderStreamInfo(QMap<QString, QString>))
      );

    connect
      (
        d->output,
        SIGNAL(readyForData()),
        SLOT(slotOutputReady())
      );

    connect
      (
        d->output,
        SIGNAL(feedback(const QByteArray &)),
        SLOT(slotOutputFeedback(const QByteArray &))
      );

    connect
      (
        d->output,
        SIGNAL(bufferPercent(unsigned int)),
        SIGNAL(outputBufferPercent(unsigned int))
      );

    connect
      (
        &d->flowTimer,
        SIGNAL(timeout()),
        SLOT(slotFlow())
      );
  }

  FlowManager::~FlowManager()
  {
    delete d->decoder;
    delete d;
  }

  void FlowManager::slotPlay()
  {
    if (0 == d->client)
    {
      d->client = new Client(d->stationInfo, this);
      connectToClient();
    }

    emit(lookingUpHost(d->client->stationInfo().url().host()));
    d->client->play();
  }

  void FlowManager::slotStop()
  {
    d->decoder->close();
    delete d->client;
    d->client = 0;
    emit(stopped());
  }

  void FlowManager::open(const StationInfo & stationInfo)
  {
    delete d->client;
    d->stationInfo = stationInfo;

    d->client = new Client(stationInfo, this);

    connectToClient();
  }

  void FlowManager::slotClientHostFound()
  {
    emit(connectingToHost(d->client->stationInfo().url().host()));
  }

  void FlowManager::slotClientConnected()
  {
    emit(streamingFromHost(d->client->stationInfo().url().host()));
    d->buffering = true;
    emit(buffering());
  }

  void FlowManager::slotClientConnectionClosed()
  {
    delete d->client;
    d->client = 0;
  }

  void FlowManager::slotClientError(int i)
  {
    qDebug("FlowManager::slotClientError(%d)", i);
    delete d->client;
    d->client = 0;
  }

  void FlowManager::connectToClient()
  {
    connect
      (
        d->client,
        SIGNAL(hostFound()),
        SLOT(slotClientHostFound())
      );

    connect
      (
        d->client,
        SIGNAL(connected()),
        SLOT(slotClientConnected())
      );

    connect
      (
        d->client,
        SIGNAL(connectionClosed()),
        SLOT(slotClientConnectionClosed())
      );

    connect
      (
        d->client,
        SIGNAL(error(int)),
        SLOT(slotClientError(int))
      );

    connect
      (
        d->client,
        SIGNAL(dataAvailable()),
        SLOT(slotEncodedDataAvailable())
      );

    connect
      (
        d->client,
        SIGNAL(streamInfo(QMap<QString, QString>)),
        SLOT(slotClientStreamInfo(QMap<QString, QString>))
      );

    connect
      (
        d->client,
        SIGNAL(redirected(const QUrl &)),
        SLOT(slotRedirected(const QUrl &))
      );

    connect
      (
        d->client,
        SIGNAL(bufferPercent(unsigned int)),
        SIGNAL(inputBufferPercent(unsigned int))
      );
  }

  bool FlowManager::output()
  {
    if (!canOutput())
      return false;

    return d->output->play
      (
        d->decoder->readBlock
        (QMIN(d->output->writeBufferAvailable(), d->decoder->bytesAvailable()))
      );
  }

  bool FlowManager::decode()
  {
    if (!canDecode())
      return false;

    d->decoder->decode
      (
        d->client->readBlock
        (QMIN(d->client->bytesAvailable(), d->decoder->writeBufferAvailable()))
      );

    return true;
  }

  void FlowManager::slotOutputFeedback(const QByteArray & buffer)
  {
    QTime currentTime = QTime::currentTime();

    if (d->lastFeedbackTime.msecsTo(currentTime) > 100)
    {
      QValueList<unsigned int> l(levelList(buffer));

      emit(leftLevel(l[0]));
      emit(rightLevel(l[1]));

      d->lastFeedbackTime = currentTime;
    }
  }

  void FlowManager::slotClientStreamInfo(QMap<QString, QString> i)
  {
    emit(streamInfo(i));
  }

  void FlowManager::slotDecoderStreamInfo(QMap<QString, QString> i)
  {
    emit(trackInfo(i));
  }

  void FlowManager::slotOutputReady()
  {
    d->flowTimer.start(0, true);
  }

  void FlowManager::slotDecoderReady()
  {
    d->flowTimer.start(0, true);
  }

  void FlowManager::slotPCMDataAvailable()
  {
    d->flowTimer.start(0, true);
  }

  void FlowManager::slotEncodedDataAvailable()
  {
    d->flowTimer.start(0, true);
  }

  void FlowManager::slotFlow()
  {
    if (openDecoder())
    {
      outputAndDecodeWhileEitherPossible();

      if (d->buffering)
      {
        if (buffersFull())
          stopBufferingStartPlayback();
      }
      else
      {
        if (buffersEmpty())
          stopPlaybackStartBuffering();
      }
    }
  }

  void FlowManager::stopPlaybackStartBuffering()
  {
    d->output->pause();

    d->buffering = true;
    emit(buffering());
  }

  void FlowManager::stopBufferingStartPlayback()
  {
    d->buffering = false;

    d->output->unpause();

    emit(playing());

    d->flowTimer.start(0, true);
  }

  bool FlowManager::openDecoder()
  {
    if (d->decoder->isOpen())
      return true;

    if (0 != d->client)
      return d->decoder->open(d->client->peekBlock());

    return false;
  }

  void FlowManager::outputAndDecodeWhileEitherPossible()
  {
    while (output() || decode())
      ;
  }

  bool FlowManager::lowWater() const
  {
    if (d->buffering)
      return false;

    return (d->client->bytesAvailable() < d->client->maxBufferSize() / 10);
  }

  bool FlowManager::buffersEmpty() const
  {
    return (0 == d->client->bytesAvailable());
  }

  bool FlowManager::buffersFull() const
  {
    return
      (0 == d->output->writeBufferAvailable())
      &&
      (0 == d->decoder->writeBufferAvailable())
      &&
      ((d->client->bytesAvailable() >= d->client->maxBufferSize()))
      ;
  }

  bool FlowManager::canOutput() const
  {
    bool ret =
      (0 != d->output->writeBufferAvailable())
      &&
      (0 != d->decoder->bytesAvailable())
      ;

    //qDebug("FlowManager::canOutput: %d", ret);

    return ret;
  }

  bool FlowManager::canDecode() const
  {
    bool ret = 
      (0 != d->client)
      &&
      (0 != d->decoder->writeBufferAvailable())
      &&
      (0 != d->client->bytesAvailable())
      ;

    //qDebug("FlowManager::canDecode: %d", ret);

    return ret;
  }

  void FlowManager::slotRedirected(const QUrl & url)
  {
    StationInfo newStationInfo(d->client->stationInfo());
    newStationInfo.setUrl(url);

    delete d->client;

    d->client = new Client(newStationInfo, this);

    connectToClient();

    emit(lookingUpHost(url.host()));

    d->client->play();
  }
}
