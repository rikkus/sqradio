// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_ADD_DIALOG_H
#define SQRADIO_ADD_DIALOG_H

#include <qurl.h>
#include <qstring.h>
#include <qdialog.h>

#include "StationInfo.h"

namespace SqRadio
{
  class AddDialog : public QDialog
  {
    Q_OBJECT

    public:

      AddDialog(QWidget * parent, const char * name = 0, bool modal = false);

      virtual ~AddDialog();

      static StationInfo add(QWidget * parent);

      StationInfo stationInfo() const;

    protected slots:

      void slotAdd();
      void slotCancel();

    private:

      class Private;
      Private * d;
  };
}

#endif // SQRADIO_ADD_DIALOG_H
