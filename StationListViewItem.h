// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_STATION_LIST_VIEW_ITEM_H
#define SQRADIO_STATION_LIST_VIEW_ITEM_H

#include <qlistview.h>

#include "StationInfo.h"

namespace SqRadio
{
  class StationListViewItem : public QListViewItem
  {
    public:

      StationListViewItem(const StationInfo &, QListView * parent);

      virtual ~StationListViewItem();

      StationInfo stationInfo() const;

    private:

      StationListViewItem(const StationListViewItem &);
      StationListViewItem & operator = (const StationListViewItem &);

      class Private;
      Private * d;
  };
}

#endif // SQRADIO_STATION_LIST_VIEW_ITEM_H
