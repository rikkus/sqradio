// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include "OutputDriverFactory.h"
#include "OutputDriver.h"
#include "OutputDriverALSA.h"
#include "OutputDriverOSS.h"
#include "OutputDriverESD.h"
#include "OutputDriverNULL.h"
#include "OutputDriverPortAudio.h"

namespace SqRadio
{
  namespace OutputDriverFactory
  {
    QStringList list()
    {
      QStringList ret;

      ret << "null";

#ifdef SQRADIO_USE_DRIVER_ALSA
      ret << "alsa";
#endif

#ifdef SQRADIO_USE_DRIVER_ESD
      ret << "esd";
#endif

#ifdef SQRADIO_USE_DRIVER_OSS
      ret << "oss";
#endif

#ifdef SQRADIO_USE_DRIVER_OSS
      ret << "oss";
#endif

      return ret;
    }

    OutputDriver * create(const QString & _name)
    {
      QString name(_name.lower());

      if        ("null"      == name) { return new OutputDriverNULL;      }

#ifdef SQRADIO_USE_DRIVER_ALSA
      else if   ("alsa"     == name)  { return new OutputDriverALSA;      }
#endif

#ifdef SQRADIO_USE_DRIVER_ESD
      else if   ("esd"      == name)  { return new OutputDriverESD;       }
#endif

#ifdef SQRADIO_USE_DRIVER_OSS
      else if   ("oss"      == name)  { return new OutputDriverOSS;       }
#endif

#ifdef SQRADIO_USE_DRIVER_OSS
      else if   ("portaudio"== name)  { return new OutputDriverPortAudio; }
#endif

      else
      {
        qDebug("No driver called %s", name.ascii());
        return 0;
      }
    }
  }
}

