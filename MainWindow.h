// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_MAIN_WINDOW_H
#define SQRADIO_MAIN_WINDOW_H

#include <qmainwindow.h>

namespace SqRadio
{
  class MainWindow : public QMainWindow
  {
    Q_OBJECT

    public:
    
      MainWindow(QWidget * parent = 0);

      virtual ~MainWindow();

    protected slots:

      void slotOpen();
      void slotPlay();
      void slotStreamTitle(const QString &);
      void slotAbout();

    protected:

      virtual void closeEvent(QCloseEvent *);

      void initActions();

    private:

      class Private;
      Private * d;
  };
}

#endif // SQRADIO_MAIN_WINDOW_H
