// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include "StationListViewItem.h"

namespace SqRadio
{
  class StationListViewItem::Private
  {
    public:

      StationInfo stationInfo;
  };

  StationListViewItem::StationListViewItem
    (
      const StationInfo & stationInfo,
      QListView         * parent
    )
    : QListViewItem(parent, stationInfo.name(), stationInfo.url().toString())
  {
    d = new Private;
    d->stationInfo = stationInfo;
  }

  StationListViewItem::~StationListViewItem()
  {
    delete d;
  }

  StationInfo StationListViewItem::stationInfo() const
  {
    return d->stationInfo;
  }
}

