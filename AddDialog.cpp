// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlabel.h>

#include "AddDialog.h"

namespace SqRadio
{
  class AddDialog::Private
  {
    public:

      QLineEdit   * nameLineEdit;
      QLineEdit   * urlLineEdit;
      QPushButton * addButton;
      QPushButton * cancelButton;
  };

  StationInfo AddDialog::add(QWidget * parent)
  {
    AddDialog addDialog(parent, 0, true /* modal */);

    if (QDialog::Accepted == addDialog.exec())
      return addDialog.stationInfo();
    else
      return StationInfo();
  }

  AddDialog::AddDialog(QWidget * parent, const char * name, bool modal)
    : QDialog(parent, name, modal)
  {
    setCaption(tr("Add station - sqradio"));

    d = new Private;

    d->nameLineEdit   = new QLineEdit(this);
    d->urlLineEdit    = new QLineEdit(this);
    d->addButton      = new QPushButton(tr("&Add"), this);
    d->cancelButton   = new QPushButton(tr("&Cancel"), this);

    d->addButton->setAutoDefault(true);
    d->addButton->setDefault(true);

    QVBoxLayout * layout = new QVBoxLayout(this, 6, 4);

    QGridLayout * nameLineEditLayout = new QGridLayout(layout, 2, 2);

    nameLineEditLayout->addWidget(new QLabel(tr("Name:"), this),  0, 0);
    nameLineEditLayout->addWidget(d->nameLineEdit,                0, 1);
    nameLineEditLayout->addWidget(new QLabel(tr("URL:"), this),   1, 0);
    nameLineEditLayout->addWidget(d->urlLineEdit,                 1, 1);

    QHBoxLayout * buttonLayout = new QHBoxLayout(layout, 4);

    buttonLayout->addStretch(1);

    buttonLayout->addWidget(d->addButton);
    buttonLayout->addWidget(d->cancelButton);

    connect
      (
        d->addButton,
        SIGNAL(clicked()),
        SLOT(slotAdd())
      );

    connect
      (
        d->cancelButton,
        SIGNAL(clicked()),
        SLOT(slotCancel())
      );
  }

  AddDialog::~AddDialog()
  {
    delete d;
  }

  void AddDialog::slotAdd()
  {
    accept();
  }

  void AddDialog::slotCancel()
  {
    reject();
  }

  StationInfo AddDialog::stationInfo() const
  {
    return StationInfo
      (
        d->nameLineEdit->text(),
        QUrl(d->urlLineEdit->text())
      );
  }
}

