// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef SQRADIO_BUFFER_ITEM_H
#define SQRADIO_BUFFER_ITEM_H

#include <qlist.h>
#include <qthread.h>

namespace SqRadio
{
  class BufferItem
  {
    public:

      BufferItem();
      ~BufferItem();

      static uint maxLength()        { return maxLength_;    }

      char *   buffer()              { return buffer_;        }
      uint     length()        const { return length_;        }
      int      time()          const { return time_;          }
      int      trackLength()   const { return trackLength_;  }
      QString  trackID()       const { return trackID_;      }
      int      bitRate()       const { return bitRate_;      }
      int      sampleRate()    const { return sampleRate_;    }
      int      channels()      const { return channels_;      }
      int      id()            const { return id_;            }

      void setLength(uint l)              { length_        = l;  }
      void setTime(int t)                 { time_          = t;  }
      void setTrackLength(int l)          { trackLength_  = l;  }
      void setTrackID(const QString & s)  { trackID_      = s;  }
      void setBitRate(int r)              { bitRate_      = r;  }
      void setSampleRate(int r)           { sampleRate_    = r;  }
      void setChannels(int c)             { channels_      = c;  }

    private:

      static uint maxLength_;

      char    * buffer_;
      uint      length_;
      int       time_;
      int       trackLength_;
      int       bitRate_;
      int       sampleRate_;
      int       channels_;

      QString   trackID_;

      int       id_;
      static int ID_;
  };
}

#endif // SQRADIO_BUFFER_ITEM_H
