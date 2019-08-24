// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_STATUS_VIEW_H
#define SQRADIO_STATUS_VIEW_H

#include <qwidget.h>

#include "Enum.h"

namespace SqRadio
{
  class StatusView : public QWidget
  {
    Q_OBJECT

    public:

      StatusView(QWidget * parent, const char * name = 0);

      virtual ~StatusView();

      void setStatus(Status::Value);

    protected slots:

      void slotUpdateBufferingPixmap();

    private:

      class Private;
      Private * d;
  };
}

#endif // SQRADIO_STATUS_VIEW_H
