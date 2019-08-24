#include "Enum.h"

namespace SqRadio
{
  Format format(const QByteArray & buf)
  {
    if (buf.size() < 4)
    {
      qDebug("format(): Buffer size is too damn small");
      return Vorbis;
    }

    if
      (
        'O' == buf[0]
        &&
        'g' == buf[1]
        &&
        'g' == buf[2]
        &&
        'S' == buf[3]
      )
        return Vorbis;

    return MP3;
  }
}

