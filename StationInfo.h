// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_STATION_INFO_H
#define SQRADIO_STATION_INFO_H

#include <qstring.h>
#include <qurl.h>
#include <qfile.h>

namespace SqRadio
{
  class StationInfo
  {
    public:

      StationInfo();

      StationInfo
        (
          const QString & name,
          const QUrl    & url
        );

      StationInfo(const StationInfo & other);
      StationInfo & operator = (const StationInfo &);

      virtual ~StationInfo();

      QString name()      const;
      QUrl    url()       const;

      QString filename()  const;

      static QString filename(const QUrl &);
      static QString stationDirPath();

      void setName  (const QString &);
      void setUrl   (const QUrl &);

      bool load();
      bool load(const QString & path);
      bool load(QFile &);
      bool save() const;

      QString asXML() const;

      bool isNull() const;

      bool createDir() const;

    private:

      class Private;
      Private * d;
  };
}

#endif // SQRADIO_STATION_INFO_H
