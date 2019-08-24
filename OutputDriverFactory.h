// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_OUTPUT_DRIVER_FACTORY_H
#define SQRADIO_OUTPUT_DRIVER_FACTORY_H

#include <qstringlist.h>

namespace SqRadio
{
  class OutputDriver;

  namespace OutputDriverFactory
  {
    QStringList list();
    OutputDriver * create(const QString & name);
  }
}

#endif // SQRADIO_OUTPUT_DRIVER_FACTORY_H
