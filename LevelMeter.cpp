// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include <qpainter.h>

#include "LevelMeter.h"

LevelMeter::LevelMeter
(
  unsigned int  min,
  unsigned int  max,
  unsigned int  val,
  unsigned int  historySize,
  Orientation   o,
  QWidget     * parent,
  const char  * name
)
  : QWidget(parent, name, WRepaintNoErase | WResizeNoErase | WPaintUnclipped),
    min_                (min),
    max_                (max),
    value_              (val),
    historySize_        (historySize),
    peakValue_          (0),
    oldPeakValue_       (0),
    orientation_        (o),
    valueColor_         (black),
    emptyColor_         (QColor(215, 242, 42)),
    peakColor_          (black)
{
  setBackgroundMode(NoBackground);

  if (orientation_ == Horizontal)
    setSizePolicy
      (QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred));
  else
    setSizePolicy
      (QSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding));

  if (min_ >= max_)
  {
    qWarning("LevelMeter ctor: min value is >= max !");
    qWarning("Setting min to 0 and max to 99");
    min_ = 0;
    max_ = 99;
  }

  if (value_ < min_)
  {
    qWarning("LevelMeter ctor: value is < min - clipping");
    value_ = min_;
  }
  else if (value_ > max_)
  {
    qWarning("LevelMeter ctor: value is > max - clipping");
    value_ = min_;
  }

  buf_.resize(size());
}

LevelMeter::~LevelMeter()
{
}

void LevelMeter::setMinValue(unsigned int i)
{
  if (i > max_)
  {
    qWarning("LevelMeter::setMinValue(%d): value is > max - clipping", i);
    min_ = max_ - 1;
  }
  else
  {
    min_ = i;
  }

  drawContents();
}

void LevelMeter::setMaxValue(unsigned int i)
{
  if (i < max_)
  {
    qWarning("LevelMeter::setMaxValue(%d): value is < min - clipping", i);
    min_ = max_ - 1;
  }
  max_ = i;
  drawContents();
}

void LevelMeter::setValue(unsigned int i)
{
  if (i < min_)
  {
    qWarning("LevelMeter::setValue(%d): clipping to %d", i, min_);
    i = min_;
  }
  else if (i > max_)
  {
    qWarning("LevelMeter::setValue(%d): clipping to %d", i, max_);
    i = max_;
  }

  if (value_ != i)
  {
    value_ = i;

    if (value_ > peakValue_)
      peakValue_ = value_;

    if (peakValue_ > oldPeakValue_ || peakAge_ > historySize_)
    {
      oldPeakValue_ = peakValue_;
      peakValue_ = 0;
      peakAge_ = 0;
    }
    else
    {
      ++peakAge_;
    }
  }

  drawContents();
}

void LevelMeter::setValueColor(const QColor & c)
{
  valueColor_ = c;
  drawContents();
}

void LevelMeter::setEmptyColor(const QColor & c)
{
  emptyColor_ = c;
  drawContents();
}

void LevelMeter::setPeakColor(const QColor & c)
{
  peakColor_ = c;
  drawContents();
}

void LevelMeter::drawContents()
{
  if (width() < 20 || height() < 20)
    return;

  QPainter p(&buf_);

  const unsigned int border = 4;
  const unsigned int cellSize = 3;

  p.fillRect(rect(), emptyColor_);

  unsigned int l =
    (Horizontal == orientation_)
    ?
    (width() - border * 2 - cellSize * 2)
    :
    (height() - border * 2 - cellSize * 2)
    ;

  unsigned int valueEnd = static_cast<unsigned int>((value_ / float(max_)) * l);
  QColor noValueColor = emptyColor_.dark(120);

  for (unsigned int i = 0; i < l; i += cellSize * 2)
  {
    if (Horizontal == orientation_)
      p.fillRect
        (
          border + cellSize + i,
          border,
          cellSize,
          height() - border * 2,
          i < valueEnd ? valueColor_ : noValueColor
        );
    else
      p.fillRect
        (
          border,
          height() - (border + cellSize + i),
          width() - border * 2,
          cellSize,
          i < valueEnd ? valueColor_ : noValueColor
        );
  }

  unsigned int peak =
    static_cast<unsigned int>
    ((oldPeakValue_ / float(max_)) * l) / (cellSize*2) * (cellSize*2);

  if (Horizontal == orientation_)
    p.fillRect
      (
        border + cellSize + peak,
        border,
        cellSize,
        height() - border * 2,
        valueColor_
      );
  else
    p.fillRect
      (
        border,
        height() - (border + cellSize + peak),
        width() - border * 2,
        cellSize,
        valueColor_
      );

  bitBlt(this, 0, 0, &buf_);
}

void LevelMeter::paintEvent(QPaintEvent *)
{
  drawContents();
}

void LevelMeter::resizeEvent(QResizeEvent *)
{
  buf_.resize(size());
  drawContents();
}

QSize LevelMeter::sizeHint() const
{
  if (orientation_ == Horizontal)
    return QSize(128, 24);
  else
    return QSize(24, 128);
}

QSize LevelMeter::minimumSizeHint() const
{
  if (orientation_ == Horizontal)
    return QSize(64, 24);
  else
    return QSize(24, 64);
}

