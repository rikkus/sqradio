// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_UTILS_H
#define SQRADIO_UTILS_H

#include <qvaluelist.h>
#include <qcstring.h>
#include <qstring.h>

namespace SqRadio
{
  QValueList<unsigned int> levelList(const QByteArray &);
  QString qSocketErrorToString(int);
  QByteArray & fadeOut(QByteArray &, double startPower = 1.f, double endPower = 0.f);
}

#endif // SQRADIO_UTILS_H
