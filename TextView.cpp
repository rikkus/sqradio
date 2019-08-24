// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include <qprocess.h>

#include "TextView.h"

namespace SqRadio
{
  class TextView::Private
  {
    public:

      int dummy;
  };

  TextView::TextView(QWidget * parent, const char * name)
    : QTextBrowser(parent, name)
  {
    d = new Private;
  }

  TextView::~TextView()
  {
    delete d;
  }

  void TextView::setSource(const QString & s)
  {
    QProcess p;
    p.addArgument("kfmclient");
    p.addArgument("openURL");
    p.addArgument(s);
    p.launch(QByteArray());
  }
}

