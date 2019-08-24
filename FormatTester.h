// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_FORMAT_TESTER_H
#define SQRADIO_FORMAT_TESTER_H

#include <qcstring.h>

namespace SqRadio
{
  class FormatTester
  {
    public:

      FormatTester();
      virtual ~FormatTester();

      bool accepted() const;

    protected:

      void setAccepted(bool b);

    private:

      class Private;
      Private * d;
  };
}

#endif // SQRADIO_FORMAT_TESTER_H
