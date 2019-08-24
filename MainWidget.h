// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_MAIN_WIDGET_H
#define SQRADIO_MAIN_WIDGET_H

#include <qwidget.h>
#include <qcstring.h>

#include "Enum.h"

namespace SqRadio
{
  class StationInfo;

  class MainWidget : public QWidget
  {
    Q_OBJECT

    public:
    
      MainWidget(QWidget * parent);

      virtual ~MainWidget();

      void open(const StationInfo &);
      bool haveStationInfo() const;

    public slots:

      void slotPlay();
      void slotStop();

    protected slots:

      void slotToggleBufferView(bool);
      void slotToggleLevelMeterView(bool);

      void slotInputBufferPercent(unsigned int);
      void slotOutputBufferPercent(unsigned int);
      void slotLeftLevel(unsigned int);
      void slotRightLevel(unsigned int);

      void slotStreamInfo(QMap<QString, QString>);
      void slotTrackInfo(QMap<QString, QString>);

      void slotLookingUpHost(const QString &);
      void slotConnectingToHost(const QString &);
      void slotStreamingFromHost(const QString &);

      void slotStopped();
      void slotPlaying();
      void slotBuffering();
 
    protected:

      void updateTextDisplay();

    signals:

      void title(const QString &);
      void inputReceived();

    private:

      class Private;
      Private * d;
  };
}

#endif // SQRADIO_MAIN_WIDGET_H
