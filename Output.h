// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_OUTPUT_H
#define SQRADIO_OUTPUT_H

#include <qobject.h>
#include <qcstring.h>

namespace SqRadio
{
  class Output : public QObject
  {
    Q_OBJECT

    public:

      Output(QObject * parent);

      virtual ~Output();

      bool play(const QByteArray &);

      Q_ULONG bufferFill() const;
      Q_ULONG bufferSize() const;

      Q_ULONG bytesAvailable() const;

      Q_ULONG writeBufferAvailable() const;

      QByteArray readBlock(unsigned int maxBytes);

      void pause();
      void unpause();

    protected slots:

      void slotReadyRead();
      void slotReadyWrite();

    protected:

      void checkIfReadyAndEmit();
      bool event(QEvent *);

      unsigned int _bufferPercent() const;

    signals:

      void readyRead();
      void readyForData();
      void feedback(const QByteArray &);
      void bufferPercent(unsigned int);

    private:

      class Private;
      Private * d;
  };
}

#endif // SQRADIO_OUTPUT_H
