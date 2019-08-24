// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_FLOW_MANAGER_H
#define SQRADIO_FLOW_MANAGER_H

#include <qobject.h>
#include <qmap.h>
#include <qstring.h>

#include "StationInfo.h"

namespace SqRadio
{
  class FlowManager : public QObject
  {
    Q_OBJECT

    public:

      FlowManager(QObject * parent);
      virtual ~FlowManager();

      void open(const StationInfo &);

      void slotPlay();
      void slotStop();

    protected slots:

      void slotClientHostFound();
      void slotClientConnected();
      void slotClientConnectionClosed();
      void slotClientError(int);
      void slotClientStreamInfo(QMap<QString, QString>);
      void slotDecoderStreamInfo(QMap<QString, QString>);

      void slotOutputFeedback(const QByteArray &);

      void slotOutputReady();
      void slotDecoderReady();
      void slotPCMDataAvailable();
      void slotEncodedDataAvailable();

      void slotFlow();

      void slotRedirected(const QUrl & url);

    protected:

      void connectToClient();

      bool canDecode() const;
      bool canOutput() const;

      bool decode();
      bool output();

      void stopPlaybackStartBuffering();
      void stopBufferingStartPlayback();

      bool openDecoder();
      void outputAndDecodeWhileEitherPossible();

      bool buffersEmpty() const;
      bool buffersFull() const;
      bool lowWater() const;
 
    signals:

      void leftLevel(unsigned int);
      void rightLevel(unsigned int);

      void inputBufferPercent(unsigned int);
      void outputBufferPercent(unsigned int);

      void streamInfo(QMap<QString, QString>);
      void trackInfo(QMap<QString, QString>);

      void lookingUpHost(const QString &);
      void connectingToHost(const QString &);
      void streamingFromHost(const QString &);

      void stopped();
      void playing();
      void buffering();

    private:

      class Private;
      Private * d;
  };
}

#endif // SQRADIO_FLOW_MANAGER_H
