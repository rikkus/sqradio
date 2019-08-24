// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include <qlayout.h>
#include <qtimer.h>
#include <qprogressbar.h>
#include <qlabel.h>
#include <qsettings.h>

#include "Enum.h"
#include "MainWidget.h"
#include "TextView.h"
#include "StationInfo.h"
#include "FlowManager.h"
#include "LevelMeter.h"
#include "StatusView.h"

namespace SqRadio
{
  static const unsigned int MaxPCMBufSize = 64 * 1024;

  class MainWidget::Private
  {
    public:

      FlowManager   * flowManager;

      TextView      * textView;
      QProgressBar  * inputBufferMeter;
      QProgressBar  * outputBufferMeter;
      QWidget       * bufferView;

      QWidget       * levelMeterView;

      LevelMeter    * leftOutputLevelMeter;
      LevelMeter    * rightOutputLevelMeter;

      QByteArray      pcmBuf;
      bool            showBufferView;
      bool            showLevelMeterView;

      StationInfo     stationInfo;

      QMap<QString, QString> streamInfo;
      QMap<QString, QString> trackInfo;

      StatusView    * statusView;
  };
    
  MainWidget::MainWidget(QWidget * parent)
    : QWidget(parent)
  {
    d = new Private;

    d->flowManager = new FlowManager(this);

    d->showBufferView     = false;
    d->showLevelMeterView = true;

    d->textView         = new TextView(this);

    d->textView->setText(tr("No stream playing"));

    d->levelMeterView = new QWidget(this);

    d->leftOutputLevelMeter   =
      new LevelMeter(0, 99, 0, 20, Vertical, d->levelMeterView);

    d->rightOutputLevelMeter  =
      new LevelMeter(0, 99, 0, 20, Vertical, d->levelMeterView);

    QHBoxLayout * levelMeterLayout = new QHBoxLayout(d->levelMeterView);

    levelMeterLayout->addWidget(d->leftOutputLevelMeter);
    levelMeterLayout->addWidget(d->rightOutputLevelMeter);

    d->bufferView     = new QWidget(this);

    d->inputBufferMeter  = new QProgressBar(d->bufferView);

    QLabel * inputBufferFillLabel =
      new QLabel(tr("Input buffer fill:"), d->bufferView);

    d->outputBufferMeter  = new QProgressBar(d->bufferView);

    QLabel * outputBufferFillLabel =
      new QLabel(tr("Output buffer fill:"), d->bufferView);

    d->statusView = new StatusView(this);

    QGridLayout * progressLayout = new QGridLayout(d->bufferView, 2, 2, 6);

    progressLayout->addWidget(inputBufferFillLabel,   0, 0);
    progressLayout->addWidget(d->inputBufferMeter,    0, 1);
    progressLayout->addWidget(outputBufferFillLabel,  1, 0);
    progressLayout->addWidget(d->outputBufferMeter,   1, 1);

    QVBoxLayout * layout = new QVBoxLayout(this);
    QHBoxLayout * topLayout = new QHBoxLayout(layout);

    topLayout->addWidget(d->textView);
    topLayout->addWidget(d->levelMeterView);

    layout->addWidget(d->bufferView);
    layout->addWidget(d->statusView);

    connect
      (
        d->flowManager,
        SIGNAL(streamInfo(QMap<QString, QString>)),
        SLOT(slotStreamInfo(QMap<QString, QString>))
      );

    connect
      (
        d->flowManager,
        SIGNAL(trackInfo(QMap<QString, QString>)),
        SLOT(slotTrackInfo(QMap<QString, QString>))
      );

    connect
      (
        d->flowManager,
        SIGNAL(leftLevel(unsigned int)),
        d->leftOutputLevelMeter,
        SLOT(setValue(unsigned int))
      );

    connect
      (
        d->flowManager,
        SIGNAL(rightLevel(unsigned int)),
        d->rightOutputLevelMeter,
        SLOT(setValue(unsigned int))
      );

    connect
      (
        d->flowManager,
        SIGNAL(lookingUpHost(const QString &)),
        SLOT(slotLookingUpHost(const QString &))
      );

    connect
      (
        d->flowManager,
        SIGNAL(connectingToHost(const QString &)),
        SLOT(slotConnectingToHost(const QString &))
      );

    connect
      (
        d->flowManager,
        SIGNAL(streamingFromHost(const QString &)),
        SLOT(slotStreamingFromHost(const QString &))
      );

    connect
      (
        d->flowManager,
        SIGNAL(stopped()),
        SLOT(slotStopped())
      );

    connect
      (
        d->flowManager,
        SIGNAL(playing()),
        SLOT(slotPlaying())
      );

    connect
      (
        d->flowManager,
        SIGNAL(buffering()),
        SLOT(slotBuffering())
      );

    connect
      (
        d->flowManager,
        SIGNAL(inputBufferPercent(unsigned int)),
        SLOT(slotInputBufferPercent(unsigned int))
      );

    connect
      (
        d->flowManager,
        SIGNAL(outputBufferPercent(unsigned int)),
        SLOT(slotOutputBufferPercent(unsigned int))
      );

    QSettings settings;

    if (settings.readBoolEntry("/sqradio/MainWidget/ShowBufferView", false))
      d->bufferView->show();
    else
      d->bufferView->hide();

    if (settings.readBoolEntry("/sqradio/MainWidget/ShowLevelMeterView", false))
      d->levelMeterView->show();
    else
      d->levelMeterView->hide();
  }

  MainWidget::~MainWidget()
  {
    delete d;
  }

  void MainWidget::slotPlay()
  {
    d->flowManager->slotPlay();
  }

  void MainWidget::slotStop()
  {
    d->flowManager->slotStop();
  }

  void MainWidget::open(const StationInfo & stationInfo)
  {
    d->stationInfo = stationInfo;

    d->flowManager->open(stationInfo);
    d->flowManager->slotPlay();
  }

  void MainWidget::slotInputBufferPercent(unsigned int i)
  {
    d->inputBufferMeter->setProgress(i);
  }

  void MainWidget::slotOutputBufferPercent(unsigned int i)
  {
    d->outputBufferMeter->setProgress(i);
  }

  void MainWidget::slotLeftLevel(unsigned int i)
  {
    d->leftOutputLevelMeter->setValue(i);
  }

  void MainWidget::slotRightLevel(unsigned int i)
  {
    d->leftOutputLevelMeter->setValue(i);
  }

  QString htmlRow(const QString & title, const QString & text)
  {
    return QString
      (
        "<tr>"
          "<td>"
          + title +
          "</td>"
          "<td>"
            "<em>"
            + text +
            "</em>"
          "</td>"
        "</tr>"
      );
  }

  QString htmlURL(const QString & url)
  {
    return QString
      (
        "<a href=\"" + url + "\">" + url + "</a>"
      );
  }

  void MainWidget::slotStreamInfo(QMap<QString, QString> info)
  {
    d->streamInfo = info;

    updateTextDisplay();
  }

  void MainWidget::slotTrackInfo(QMap<QString, QString> info)
  {
    d->trackInfo = info;

    updateTextDisplay();
  }

  void MainWidget::updateTextDisplay()
  {
    QString displayText;

    QString trackTitle =
      d->trackInfo["Title"].isEmpty()
      ?
      d->streamInfo["Title"]
      :
      d->trackInfo["Title"];

    QString streamTitle =
      d->streamInfo["Stream"].isEmpty()
      ?
      d->stationInfo.name()
      :
      d->streamInfo["Stream"]
      ;

    if (trackTitle.isEmpty())
    {
      if (streamTitle.isEmpty())
        emit(title(tr("Unnamed stream")));
      else
        emit(title(streamTitle));
    }
    else
    {
      emit(title(trackTitle));
    }

    displayText += "<qt>";
    displayText += "<table>";

    if (!d->streamInfo["Name"].isEmpty())
      displayText += htmlRow(tr("Stream"), d->streamInfo["Name"]);
    else
      displayText += htmlRow(tr("Stream"), tr("Unnamed"));

    if (!d->streamInfo["URL"].isEmpty())
      displayText += htmlRow(tr("URL"), htmlURL(d->streamInfo["URL"]));

    if (!d->trackInfo["Artist"].isEmpty())
      displayText += htmlRow(tr("Artist"), d->trackInfo["Artist"]);

    if (!d->trackInfo["Album"].isEmpty())
      displayText += htmlRow(tr("Album"), d->trackInfo["Album"]);

    if (!trackTitle.isEmpty())
      displayText += htmlRow(tr("Title"), trackTitle);

    if (!d->streamInfo["Bitrate"].isEmpty())
      displayText += htmlRow(tr("Bitrate"), d->streamInfo["Bitrate"]);

    displayText += "</table>";
    displayText += "</qt>";

    d->textView->setText(displayText);
  }

  void MainWidget::slotToggleBufferView(bool on)
  {
    d->showBufferView = on;

    QSettings settings;
    settings.writeEntry("/sqradio/MainWidget/ShowBufferView", on);

    if (on)
      d->bufferView->show();
    else
      d->bufferView->hide();
  }

  void MainWidget::slotToggleLevelMeterView(bool on)
  {
    d->showLevelMeterView = on;

    QSettings settings;
    settings.writeEntry("/sqradio/MainWidget/ShowLevelMeterView", on);

    if (on)
      d->levelMeterView->show();
    else
      d->levelMeterView->hide();
  }

  void MainWidget::slotLookingUpHost(const QString & s)
  {
    d->statusView->setStatus(Status::Contacting);

    d->textView->clear();

    d->textView->setText
      (
        QString("<p>%1</p>").arg
        (tr("Looking up address for %1").arg(s))
      );
  }

  void MainWidget::slotConnectingToHost(const QString & s)
  {
    d->textView->append
      (
        QString("<p>%1</p>").arg
        (tr("Connecting to %1").arg(s))
      );
  }

  void MainWidget::slotStreamingFromHost(const QString & s)
  {
    d->textView->append
      (
        QString("<p>%1</p>").arg
        (tr("Streaming from %1").arg(s))
      );
  }

  void MainWidget::slotBuffering()
  {
    d->statusView->setStatus(Status::Buffering);
  }

  void MainWidget::slotStopped()
  {
    d->statusView->setStatus(Status::Stopped);
  }

  void MainWidget::slotPlaying()
  {
    d->statusView->setStatus(Status::Playing);
  }

  bool MainWidget::haveStationInfo() const
  {
    return !d->stationInfo.isNull();
  }
}
