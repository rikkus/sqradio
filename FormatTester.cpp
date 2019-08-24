// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include "FormatTester.h"

namespace SqRadio
{
  class FormatTester::Private
  {
    public:

      bool accepted;
  };

  FormatTester::FormatTester()
  {
    d = new Private;
    d->accepted = false;
  }

  FormatTester::~FormatTester()
  {
    delete d;
  }

  bool FormatTester::accepted() const
  {
    return d->accepted;
  }

  void FormatTester::setAccepted(bool b)
  {
    d->accepted = b;
  }
}

