#ifndef SQRADIO_TEST_CLIENT_H
#define SQRADIO_TEST_CLIENT_H

#include <qobject.h>
#include <qurl.h>
#include <qcstring.h>

#include "StationInfo.h"

namespace SqRadio
{
  class TestClient : public QObject
  {
    Q_OBJECT

    public:

      TestClient(const StationInfo & stationInfo, QObject * parent = 0);
      virtual ~TestClient();

      void play();

      StationInfo stationInfo() const;

      QByteArray readBlock(unsigned int maxSize = 4096);

      unsigned int bytesAvailable() const;

    protected slots:

      void slotReadyRead();

    signals:

      void hostFound();
      void connected();
      void connectionClosed();
      void error(int);

      void dataAvailable();

    private:

      class Private;
      Private * d;
  };
}

#endif // SQRADIO_TEST_CLIENT_H
