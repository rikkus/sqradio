// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include "Application.h"
#include "MainWindow.h"

namespace SqRadio
{
  class Application::Private
  {
    public:

      MainWindow * mainWindow;
  };

  Application::Application(int argc, char ** argv)
    : QApplication(argc, argv)
  {
    d = new Private;

    d->mainWindow = new MainWindow;

    d->mainWindow->show();
  }

  Application::~Application()
  {
    delete d;
  }
}
