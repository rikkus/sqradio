// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_THROBBER_H
#define SQRADIO_THROBBER_H

#include <qtoolbutton.h>

namespace SqRadio
{
  class Throbber : public QToolButton
  {
    Q_OBJECT

    public:

      Throbber(QWidget * parent = 0, const char * name = 0);

      QSize sizeHint() const;
      QSize minimumSizeHint() const;

    public slots:

      void slotThrob();

    protected:

      virtual void drawButton(QPainter *);

    private:

      int r, g, b;
  };
}

#endif // SQRADIO_THROBBER_H
