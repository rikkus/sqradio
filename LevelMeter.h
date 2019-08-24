// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#ifndef LEVEL_METER_H
#define LEVEL_METER_H

#include <qwidget.h>
#include <qpixmap.h>

class QPopupMenu;

class LevelMeter : public QWidget
{
  Q_OBJECT

  Q_PROPERTY(QColor valueColor       READ valueColor       WRITE setValueColor)
  Q_PROPERTY(QColor emptyColor       READ emptyColor       WRITE setEmptyColor)
  Q_PROPERTY(QColor peakColor        READ peakColor        WRITE setPeakColor)

  public:

    LevelMeter
      (
       unsigned int   min           = 0,
       unsigned int   max           = 99,
       unsigned int   val           = 0,
       unsigned int   historySize   = 20,
       Orientation    o             = Vertical,
       QWidget      * parent        = 0,
       const char   * name          = 0
      );

    virtual ~LevelMeter();

    void setValueColor          (const QColor &);
    void setEmptyColor          (const QColor &);
    void setPeakColor           (const QColor &);

    void setMinValue      (unsigned int);
    void setMaxValue      (unsigned int);
    void setHistorySize   (unsigned int);

    QColor valueColor       ()  const  { return valueColor_;        }
    QColor emptyColor       ()  const  { return emptyColor_;        }
    QColor peakColor        ()  const  { return peakColor_;         }

    unsigned int minValue      ()  const  { return min_;               }
    unsigned int maxValue      ()  const  { return max_;               }
    unsigned int value         ()  const  { return value_;             }
    unsigned int historySize   ()  const  { return historySize_;       }

    QSize sizeHint          () const;
    QSize minimumSizeHint   () const;

    bool showPeak() const;
    void setShowPeak(bool);

  public slots:

    void setValue(unsigned int);

  protected:

    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);

    virtual void drawContents();

  private:

    LevelMeter(const LevelMeter &);
    LevelMeter & operator = (const LevelMeter &);

    unsigned int min_;
    unsigned int max_;
    unsigned int value_;
    unsigned int historySize_;
    unsigned int peakValue_;
    unsigned int oldPeakValue_;
    unsigned int peakAge_;

    Orientation orientation_;

    QColor valueColor_;
    QColor emptyColor_;
    QColor peakColor_;

    QPixmap buf_;
};

#endif // LEVEL_METER_H
