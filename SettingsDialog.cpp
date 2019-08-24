// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>

#include "SettingsDialog.h"

namespace SqRadio
{
  class SettingsDialog::Private
  {
    public:

      QLineEdit * mpegDecoderLineEdit;
      QLineEdit * vorbisDecoderLineEdit;

      QPushButton * okButton;
      QPushButton * cancelButton;
  };

  SettingsDialog::SettingsDialog
    (
      QWidget     * parent,
      const char  * name,
      bool          modal
    )
    : QDialog(parent, name, modal)
  {
    d = new Private;

    d->mpegDecoderLineEdit   = new QLineEdit(this);
    d->vorbisDecoderLineEdit = new QLineEdit(this);
    d->okButton              = new QPushButton(tr("&Ok"), this);
    d->cancelButton          = new QPushButton(tr("&Cancel"), this);

    QVBoxLayout * layout = new QVBoxLayout(this, 6, 4);

    QGridLayout * decoderLayout = new QGridLayout(layout, 2, 2);

    decoderLayout->addWidget(new QLabel(tr("MP3 decoder:"), this), 0, 0);
    decoderLayout->addWidget(d->mpegDecoderLineEdit, 0, 1);
    decoderLayout->addWidget(new QLabel(tr("Ogg Vorbis decoder:"), this), 1, 0);
    decoderLayout->addWidget(d->vorbisDecoderLineEdit, 1, 1);

    QHBoxLayout * buttonLayout = new QHBoxLayout(layout);

    buttonLayout->addStretch(1);

    buttonLayout->addWidget(d->okButton);
    buttonLayout->addWidget(d->cancelButton);

    connect
      (
        d->okButton,
        SIGNAL(clicked()),
        SLOT(slotOk())
      );

    connect
      (
        d->cancelButton,
        SIGNAL(clicked()),
        SLOT(slotCancel())
      );
  }

  SettingsDialog::~SettingsDialog()
  {
    delete d;
  }

  void SettingsDialog::slotOk()
  {
    accept();
  }

  void SettingsDialog::slotCancel()
  {
    reject();
  }
}

