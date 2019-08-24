// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include <qlistview.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qdir.h>
#include <qstringlist.h>

#include "OpenDialog.h"
#include "AddDialog.h"
#include "StationListViewItem.h"

namespace SqRadio
{
  class OpenDialog::Private
  {
    public:

      StationInfo stationInfo;

      QListView   * listView;

      QPushButton * addButton;
      QPushButton * editButton;
      QPushButton * removeButton;
      QPushButton * openButton;
      QPushButton * cancelButton;
  };

  StationInfo OpenDialog::getStationInfo(QWidget * parent)
  {
    OpenDialog d(parent, 0, true /* modal */);

    if (QDialog::Accepted == d.exec())
      return d.stationInfo();
    else
      return StationInfo();
  }

  OpenDialog::OpenDialog(QWidget * parent, const char * name, bool modal)
    : QDialog(parent, name, modal)
  {
    setCaption(tr("Open stream - sqradio"));

    d = new Private;

    d->listView     = new QListView(this);

    d->listView->setAllColumnsShowFocus(true);

    d->cancelButton = new QPushButton(tr("&Cancel"), this);
    d->openButton   = new QPushButton(tr("&Open"), this);
    d->addButton    = new QPushButton(tr("&Add"), this);
    d->editButton   = new QPushButton(tr("&Edit"), this);
    d->removeButton = new QPushButton(tr("&Remove"), this);

    d->openButton->setAutoDefault(true);
    d->openButton->setDefault(true);

    d->listView->addColumn(tr("Name"));
    d->listView->addColumn(tr("URL"));

    QVBoxLayout * layout = new QVBoxLayout(this, 6, 4);

    QHBoxLayout * listLayout = new QHBoxLayout(layout);

    listLayout->addWidget(d->listView);

    QVBoxLayout * listButtonLayout = new QVBoxLayout(listLayout);

    listButtonLayout->addWidget(d->addButton);
    listButtonLayout->addWidget(d->editButton);
    listButtonLayout->addWidget(d->removeButton);

    listButtonLayout->addStretch(1);

    QHBoxLayout * buttonLayout = new QHBoxLayout(layout);

    buttonLayout->addStretch(1);

    buttonLayout->addWidget(d->openButton);
    buttonLayout->addWidget(d->cancelButton);

    connect
      (
        d->addButton,
        SIGNAL(clicked()),
        SLOT(slotAdd())
      );

    connect
      (
        d->editButton,
        SIGNAL(clicked()),
        SLOT(slotEdit())
      );

    connect
      (
        d->removeButton,
        SIGNAL(clicked()),
        SLOT(slotRemove())
      );

    connect
      (
        d->openButton,
        SIGNAL(clicked()),
        SLOT(slotOpen())
      );

    connect
      (
        d->cancelButton,
        SIGNAL(clicked()),
        SLOT(slotCancel())
      );

    connect
      (
        d->listView,
        SIGNAL(selectionChanged()),
        SLOT(slotUpdateButtons())
      );

    connect
      (
        d->listView,
        SIGNAL(doubleClicked(QListViewItem *)),
        SLOT(slotDoubleClicked(QListViewItem *))
      );

    loadStationInfo();
    updateButtons();
  }

  OpenDialog::~OpenDialog()
  {
    delete d;
  }

  void OpenDialog::loadStationInfo()
  {
    d->listView->clear();

    QString stationDirPath(StationInfo::stationDirPath());

    QDir stationDir(stationDirPath);

    if (!stationDir.exists())
    {
      qDebug("No dir: %s", stationDirPath.ascii());
      return;
    }

    QStringList fileList(stationDir.entryList(QDir::Files | QDir::Readable));

    QStringList::ConstIterator it;

    for (it = fileList.begin(); it != fileList.end(); ++it)
      load(stationDirPath + *it);

    updateButtons();
  }

  void OpenDialog::load(const QString & stationPath)
  {
    StationInfo stationInfo;

    if (!stationInfo.load(stationPath))
    {
      qDebug("Couldn't load station info from path");
      return;
    }

    if (!stationInfo.load())
    {
      qDebug("Can't load station info");
      return;
    }

    new StationListViewItem(stationInfo, d->listView);
  }

  StationInfo OpenDialog::stationInfo() const
  {
    return d->stationInfo;
  }

  void OpenDialog::slotOpen()
  {
    QListViewItem * selectedItem = d->listView->selectedItem();

    if (0 == selectedItem)
    {
      qDebug("Bug: nothing was selected");
      return;
    }

    StationListViewItem * i = static_cast<StationListViewItem *>(selectedItem);

    d->stationInfo = i->stationInfo();

    accept();
  }

  void OpenDialog::slotCancel()
  {
    reject();
  }

  void OpenDialog::slotAdd()
  {
    StationInfo stationInfo = AddDialog::add(this);

    if (!stationInfo.isNull())
    {
      if (!stationInfo.save())
      {
        qDebug("Couldn't save new station info");
        return;
      }

      new StationListViewItem(stationInfo, d->listView);
      updateButtons();
    }
  }

  void OpenDialog::slotEdit()
  {
    qDebug("slotEdit: STUB");
  }

  void OpenDialog::slotRemove()
  {
    QListViewItem * selectedItem = d->listView->selectedItem();

    if (0 == selectedItem)
    {
      qDebug("Bug: nothing was selected");
      return;
    }

    StationListViewItem * i = static_cast<StationListViewItem *>(selectedItem);

    QString stationPath =
      StationInfo::stationDirPath() + i->stationInfo().filename();

    if (!QDir().remove(stationPath))
      qDebug("Can't remove %s", stationPath.ascii());

    delete i;
    updateButtons();
  }

  void OpenDialog::slotDoubleClicked(QListViewItem * clickedItem)
  {
    if (0 == clickedItem)
      return;

    StationListViewItem * i = static_cast<StationListViewItem *>(clickedItem);

    d->stationInfo = i->stationInfo();

    accept();
  }

  void OpenDialog::updateButtons()
  {
    bool haveSelectedItem = (0 != d->listView->selectedItem());

    d->openButton   ->setEnabled(haveSelectedItem);
    d->editButton   ->setEnabled(haveSelectedItem);
    d->removeButton ->setEnabled(haveSelectedItem);
  }

  void OpenDialog::slotUpdateButtons()
  {
    updateButtons();
  }
}
