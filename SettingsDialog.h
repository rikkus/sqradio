// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_SETTINGS_DIALOG_H
#define SQRADIO_SETTINGS_DIALOG_H

#include <qdialog.h>

namespace SqRadio
{
  class SettingsDialog : public QDialog
  {
    Q_OBJECT

    public:

      SettingsDialog
        (
          QWidget     * parent,
          const char  * name    = 0,
          bool          modal   = false
        );

      ~SettingsDialog();

    protected slots:

      void slotOk();
      void slotCancel();

    private:

      class Private;
      Private * d;
  };
};

#endif // SQRADIO_SETTINGS_DIALOG_H
