// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include <cstdlib>
#include <qpainter.h>

#include "Throbber.h"

namespace SqRadio
{
  Throbber::Throbber(QWidget * parent, const char * name)
    : QToolButton(parent, name),
      r(0),
      g(0),
      b(0)
  {
  }

  void Throbber::slotThrob()
  {
    r = 1 + int(255.0 * rand() / (RAND_MAX + 1.0));
    g = 1 + int(255.0 * rand() / (RAND_MAX + 1.0));
    b = 1 + int(255.0 * rand() / (RAND_MAX + 1.0));

    update();
  }

  void Throbber::drawButton(QPainter * p)
  {
    p->fillRect(0, 0, width(), height(), QColor(r, g, b));
  }

  QSize Throbber::sizeHint() const
  {
    return QSize(16, 16);
  }

  QSize Throbber::minimumSizeHint() const
  {
    return QSize(16, 16);
  }
}
