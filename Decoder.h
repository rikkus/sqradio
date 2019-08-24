// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_DECODER_H
#define SQRADIO_DECODER_H

#include <qobject.h>
#include <qcstring.h>
#include <qstring.h>
#include <qmap.h>

#include "Enum.h"

namespace SqRadio
{
  class Decoder : public QObject
  {
    Q_OBJECT

    public:

      Decoder(QObject * parent);

      virtual ~Decoder();

      bool open(const QByteArray &);
      bool decode(const QByteArray &);

      bool isOpen() const;
      void close();

      QByteArray readBlock(unsigned int maxBytes);

      Q_ULONG bufferFill() const;
      Q_ULONG bufferSize() const;

      Q_ULONG bytesAvailable() const;

      Q_ULONG writeBufferAvailable() const;
      Q_ULONG readBufferAvailable() const;

    protected slots:

      void slotReadyRead();
      void slotReadyWrite();

    protected:

#ifndef SQRADIO_EMBEDDED
      bool event(QEvent *);
#endif // SQRADIO_EMBEDDED
      void checkIfReadyAndEmit();

    signals:

      void readyForData();
      void readyRead();

      void streamInfo(QMap<QString, QString>);

    private:

      class Private;
      Private * d;
  };
}

#endif // SQRADIO_DECODER_H
