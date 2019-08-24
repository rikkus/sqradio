// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include <qfile.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qtextstream.h>
#include <qdom.h>
#include <qstringlist.h>

#include "StationInfo.h"

namespace SqRadio
{
  class StationInfo::Private
  {
    public:

      QString name;
      QUrl    url;
  };

  StationInfo::StationInfo()
  {
    d = new Private;
  }

  StationInfo::StationInfo
    (
      const QString & name,
      const QUrl    & url
    )
  {
    d = new Private;
    d->name = name;
    d->url  = url;
  }

  StationInfo::StationInfo(const StationInfo & other)
  {
    d = new Private;
    d->name = other.d->name;
    d->url  = other.d->url;
  }

  StationInfo & StationInfo::operator = (const StationInfo & other)
  {
    if (this == &other)
      return *this;

    d->name = other.d->name;
    d->url  = other.d->url;

    return *this;
  }

  StationInfo::~StationInfo()
  {
    delete d;
  }

  QString StationInfo::name() const
  {
    return d->name;
  }

  QUrl StationInfo::url() const
  {
    return d->url;
  }

  QString StationInfo::filename() const
  {
    return filename(d->url);
  }

  QString StationInfo::filename(const QUrl & url)
  {
    QString host(url.host());

    long port(url.port());

    // QUrl thinks port is -1 if you don't give it.

    if (-1 == port)
      port = 80;

    QUrl tempUrl(url);

    // encodedPathAndQuery doesn't actually encode the path or query.
    QString pathAndQuery(tempUrl);
    QUrl::encode(pathAndQuery);

    // Encoding an URL replaces #, so we can use it.
    pathAndQuery.replace("/", "#");

    return host + " " + QString::number(port) + " " + pathAndQuery;
  }

  void StationInfo::setName(const QString & name)
  {
    d->name = name;
  }

  void StationInfo::setUrl(const QUrl & u)
  {
    d->url = u;
  }

  bool StationInfo::load()
  {
    QFile stationFile(stationDirPath() + filename());

    if (!stationFile.open(IO_ReadOnly))
    {
      qDebug("Can't open %s", filename().ascii());
      return false;
    }

    return load(stationFile);
  }

  bool StationInfo::save() const
  {
    if (!createDir())
      return false;

    QFile stationFile(stationDirPath() + filename() + " temp");

    if (!stationFile.open(IO_WriteOnly))
    {
      qDebug("Can't open %s", filename().ascii());
      return false;
    }

    QTextStream str(&stationFile);

    str << asXML();

    stationFile.close();

    if (IO_Ok != stationFile.status())
    {
      qDebug("Can't write %s", filename().ascii());
      return false;
    }

    if (!QDir().rename(stationDirPath() + filename() + " temp", stationDirPath() + filename()))
    {
      qDebug("Can't rename `%s' to `%s'", (filename() + " temp").ascii(), filename().ascii());
      return false;
    }

    return true;
  }

  QString StationInfo::stationDirPath()
  {
    return QDir::homeDirPath() + "/.sqradio/stations/";
  }

  bool StationInfo::load(const QString & path)
  {
    QFile stationFile(path);

    if (!stationFile.open(IO_ReadOnly))
    {
      qDebug("Can't open %s", path.ascii());
      return false;
    }

    return load(stationFile);
  }

  bool StationInfo::load(QFile & stationFile)
  {
    QDomDocument doc("station");

    if (!doc.setContent(&stationFile))
    {
      qDebug("Can't read document from %s", filename().ascii());
      return false;
    }

    stationFile.close();

    QDomElement documentElement = doc.documentElement();

    d->name = documentElement.attribute("name");
    d->url  = documentElement.attribute("url");

    return true;
  }

  QString StationInfo::asXML() const
  {
    QDomDocument doc("sqradio_station_info");

    QDomElement root = doc.createElement("station");
    root.setAttribute("name", d->name);
    root.setAttribute("url",  d->url);

    doc.appendChild(root);

    return doc.toString();
  }

  bool StationInfo::isNull() const
  {
    return d->name.isEmpty();
  }

  bool StationInfo::createDir() const
  {
    QDir d(stationDirPath());

    if (d.exists())
      return true;

    QStringList pathNodeList(QStringList::split(QDir::separator(), d.path()));

    QString path;

    QStringList::ConstIterator it;

    for (it = pathNodeList.begin(); it != pathNodeList.end(); ++it)
    {
      path += "/";
      path += *it;

      if (!QFileInfo(path).isDir())
      {
        if (!QDir().mkdir(path))
        {
          qDebug("Can't make dir %s", path.ascii());
          return false;
        }
      }
    }

    return true;
  }
}
