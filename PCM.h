// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_PCM_H
#define SQRADIO_PCM_H

namespace SqRadio
{
  class PCM
  {
    public:

      class Format
      {
        public:

          enum ByteOrder  { BigEndian, LittleEndian };
          enum Signedness { Signed, Unsigned };

          Format()
            : channelCount    (0),
              bytesPerSample  (0),
              signedness      (Signed),
              byteOrder       (BigEndian),
              rate            (0)
          {
          }

          Format
            (
              unsigned int    _channelCount,
              unsigned int    _bytesPerSample,
              Signedness      _signedness,
              ByteOrder       _byteOrder,
              unsigned int    _rate
            )
            : channelCount    (_channelCount),
              bytesPerSample  (_bytesPerSample),
              signedness      (_signedness),
              byteOrder       (_byteOrder),
              rate            (_rate)
          {
            // Empty.
          }

          bool operator == (const Format & other) const
          {
            return
              (channelCount   == other.channelCount)
              &&
              (bytesPerSample == other.bytesPerSample)
              &&
              (signedness     == other.signedness)
              &&
              (byteOrder      == other.byteOrder)
              &&
              (rate           == other.rate)
              ;
          }

          unsigned int    channelCount;
          unsigned int    bytesPerSample;
          Signedness      signedness;
          ByteOrder       byteOrder;
          unsigned int    rate;
      };

      PCM();

      PCM
      (
        Format          format,
        unsigned int    sampleCount
      );

      PCM
      (
        Format          format,
        const char    * data,
        unsigned int    sampleCount
      );

      PCM(const PCM &);

      PCM & operator = (const PCM &);

      ~PCM();

            Format        format()      const;
            unsigned int  sampleCount() const;
            unsigned int  size()        const;
      const char *        data()        const;
            char *        data();

      void setFormat(Format);

    private:

      class Private;
      Private * d;
  };
}
#endif // SQRADIO_PCM_H
