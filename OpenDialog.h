// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_OPEN_DIALOG_H
#define SQRADIO_OPEN_DIALOG_H

#include <qdialog.h>

#include "StationInfo.h"

class QListViewItem;

namespace SqRadio
{
  class OpenDialog : public QDialog
  {
    Q_OBJECT

    public:

      static StationInfo getStationInfo(QWidget * parent);

      OpenDialog
        (
          QWidget     * parent  = 0,
          const char  * name    = 0,
          bool          modal   = false
        );

      virtual ~OpenDialog();

      StationInfo stationInfo() const;

    protected slots:

      void slotAdd();
      void slotEdit();
      void slotRemove();
      void slotOpen();
      void slotCancel();
      void slotUpdateButtons();
      void slotDoubleClicked(QListViewItem *);

    protected:

      void loadStationInfo();
      void load(const QString & stationFilePath);

      void updateButtons();
 
    private:

      class Private;
      Private * d;
  };
}

#endif // SQRADIO_OPEN_DIALOG_H
