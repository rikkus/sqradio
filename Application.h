// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_APPLICATION_H
#define SQRADIO_APPLICATION_H

#include <qapplication.h>

namespace SqRadio
{
  class Application : public QApplication
  {
    public:
    
      Application(int argc, char ** argv);
      virtual ~Application();

    private:

      class Private;
      Private * d;
  };
}

#endif //SQRADIO_APPLICATION_H
