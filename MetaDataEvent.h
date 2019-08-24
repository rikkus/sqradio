// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_META_DATA_EVENT_H
#define SQRADIO_META_DATA_EVENT_H

#include <qevent.h>

#include <qstring.h>

namespace SqRadio
{
  class MetaDataEvent : public QCustomEvent
  {
    public:

      MetaDataEvent
        (
          const QString & artist,
          const QString & album,
          const QString & title
        )
        : QCustomEvent(8070),
          artist_ (artist),
          album_  (album),
          title_  (title)
      {
      }

      QString artist() const
      {
        return artist_;
      }

      QString album() const
      {
        return album_;
      }

      QString title() const
      {
        return title_;
      }

    private:

      QString artist_;
      QString album_;
      QString title_;
  };
}

#endif // SQRADIO_META_DATA_EVENT_H
