// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include <qaction.h>
#include <qtoolbar.h>
#include <qapplication.h>
#include <qlayout.h>
#include <qstatusbar.h>
#include <qsettings.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>

#include "Throbber.h"
#include "Pixmaps.h"
#include "MainWindow.h"
#include "MainWidget.h"
#include "OpenDialog.h"
#include "StationInfo.h"

namespace SqRadio
{
  class MainWindow::Private
  {
    public:

      MainWidget  * mainWidget;

      QToolBar    * toolBar;

      QAction     * openAction;

      QAction     * playAction;
      QAction     * stopAction;

      QAction     * toggleBufferViewAction;
      QAction     * toggleLevelMeterViewAction;

//      Throbber    * throbber;

      QPopupMenu  * streamMenu;
      QPopupMenu  * controlMenu;
      QPopupMenu  * viewMenu;
      QPopupMenu  * helpMenu;
  };
    
  MainWindow::MainWindow(QWidget * parent)
    : QMainWindow(parent)
  {
    d = new Private;

    setCaption(tr("No stream - sqradio"));

    d->mainWidget = new MainWidget(this);

    setCentralWidget(d->mainWidget);

    d->toolBar = new QToolBar(this);

    d->streamMenu   = new QPopupMenu(this);
    d->controlMenu  = new QPopupMenu(this);
    d->viewMenu     = new QPopupMenu(this);
    d->helpMenu     = new QPopupMenu(this);

    initActions();

    d->streamMenu->insertItem(tr("&Quit"), qApp, SLOT(quit()), CTRL+Key_Q);

    menuBar()->insertItem(tr("&Stream"), d->streamMenu);
    menuBar()->insertItem(tr("&Control"), d->controlMenu);
    menuBar()->insertItem(tr("&View"), d->viewMenu);
    menuBar()->insertItem(tr("&Help"), d->helpMenu);

//    d->throbber = new Throbber(d->toolBar);
//    statusBar()->addWidget(d->throbber, 0, true /* permanent */);

    connect
      (
        d->mainWidget,
        SIGNAL(title(const QString &)),
        SLOT(slotStreamTitle(const QString &))
      );

#if 0
    connect
      (
        d->mainWidget,
        SIGNAL(inputReceived()),
        d->throbber,
        SLOT(slotThrob())
      );
#endif

    QSettings settings;

    resize
      (
        settings.readNumEntry("/sqradio/MainWindow/Width"),
        settings.readNumEntry("/sqradio/MainWindow/Height")
      );

//    statusBar()->message(tr("Welcome to sqradio"), 4000);
  }

  MainWindow::~MainWindow()
  {
    delete d;
  }

  void MainWindow::initActions()
  {
    d->openAction = new QAction
      (
        tr("Open"),
        QIconSet(QPixmap((const char **)open_xpm)),
        tr("&Open"),
        CTRL+Key_O,
        this
      );

    d->playAction = new QAction
      (
        tr("Play"),
        QIconSet(QPixmap((const char **)play_xpm)),
        tr("&Play"),
        CTRL+Key_P,
        this
      );

    d->stopAction = new QAction
      (
        tr("Stop"),
        QIconSet(QPixmap((const char **)stop_xpm)),
        tr("&Stop"),
        CTRL+Key_S,
        this
      );

    d->toggleBufferViewAction = new QAction
      (
        tr("Toggle buffer view"),
        QIconSet(QPixmap((const char **)toggle_xpm)),
        tr("Toggle &buffer view"),
        0,
        this,
        0,
        true // Toggle action.
      );

    d->toggleLevelMeterViewAction = new QAction
      (
        tr("Toggle level meter view"),
        QIconSet(QPixmap((const char **)toggle_levels_xpm)),
        tr("Toggle &level meter view"),
        0,
        this,
        0,
        true // Toggle action.
      );
 
    QAction * aboutAction = new QAction
      (
        tr("About"),
        tr("&About sqradio"),
        0,
        this
      );

    d->openAction->addTo(d->toolBar);
    d->openAction->addTo(d->streamMenu);
    d->playAction->addTo(d->toolBar);
    d->playAction->addTo(d->controlMenu);
    d->stopAction->addTo(d->toolBar);
    d->stopAction->addTo(d->controlMenu);

    d->toolBar->addSeparator();

    d->toggleBufferViewAction->addTo(d->toolBar);
    d->toggleBufferViewAction->addTo(d->viewMenu);
    d->toggleLevelMeterViewAction->addTo(d->toolBar);
    d->toggleLevelMeterViewAction->addTo(d->viewMenu);
    aboutAction->addTo(d->helpMenu);

    connect
      (
        d->openAction,
        SIGNAL(activated()),
        SLOT(slotOpen())
      );

    connect
      (
        d->playAction,
        SIGNAL(activated()),
        SLOT(slotPlay())
      );

    connect
      (
        d->stopAction,
        SIGNAL(activated()),
        d->mainWidget,
        SLOT(slotStop())
      );

    connect
      (
        d->toggleBufferViewAction,
        SIGNAL(toggled(bool)),
        d->mainWidget,
        SLOT(slotToggleBufferView(bool))
      );

    connect
      (
        d->toggleLevelMeterViewAction,
        SIGNAL(toggled(bool)),
        d->mainWidget,
        SLOT(slotToggleLevelMeterView(bool))
      );

    connect
      (
        aboutAction,
        SIGNAL(activated()),
        SLOT(slotAbout())
      );
  }

  void MainWindow::closeEvent(QCloseEvent *)
  {
    QSettings settings;

    settings.writeEntry("/sqradio/MainWindow/Width",  width());
    settings.writeEntry("/sqradio/MainWindow/Height", height());

    qApp->quit();
  }

  void MainWindow::slotOpen()
  {
    StationInfo stationInfo = OpenDialog::getStationInfo(this);

    if (stationInfo.isNull())
      return;

    d->mainWidget->open(stationInfo);
  }

  void MainWindow::slotStreamTitle(const QString & s)
  {
    setCaption(tr("%1 - sqradio").arg(s));
  }

  void MainWindow::slotAbout()
  {
    QMessageBox::about
      (
        this,
        tr("About sqradio"),
        tr
        (
          "<p>sqradio is a player for streaming Internet radio.</p>"
          "<p>Supported formats: MP3, Ogg Vorbis.</p>"
          "<p>sqradio was written by Rik Hemsley &lt;rik@kde.org&gt;.</p>"
        )
      );
  }

  void MainWindow::slotPlay()
  {
    if (d->mainWidget->haveStationInfo())
      d->mainWidget->slotPlay();
    else
      slotOpen();
  }
}
