// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_TEXT_VIEW_H
#define SQRADIO_TEXT_VIEW_H

#include <qtextbrowser.h>

namespace SqRadio
{
  class TextView : public QTextBrowser
  {
    Q_OBJECT

    public:

      TextView(QWidget * parent, const char * name = 0);
      virtual ~TextView();

    public slots:

      void setSource(const QString &);

    private:

      class Private;
      Private * d;
  };
}

#endif // SQRADIO_TEXT_VIEW_H
