// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2003 Rik Hemsley (rikkus) <rik@kde.org>

#include "StatusView.h"
#include "Pixmaps.h"

#include <qlabel.h>
#include <qtimer.h>
#include <qlayout.h>
#include <qpixmap.h>

namespace SqRadio
{
  class StatusView::Private
  {
    public:

      QTimer * bufferingTimer;

      unsigned int bufferingPixmapNumber;

      QLabel * picLabel;
      QLabel * textLabel;

      Status::Value status;
  };

  StatusView::StatusView(QWidget * parent, const char * name)
    : QWidget(parent, name)
  {
    d = new Private;

    d->bufferingPixmapNumber = 0;

    d->bufferingTimer = new QTimer(this);

    connect
      (
        d->bufferingTimer,
        SIGNAL(timeout()),
        SLOT(slotUpdateBufferingPixmap())
      );

    d->picLabel   = new QLabel(this);
    d->textLabel  = new QLabel(this);

    QColorGroup cg(d->textLabel->colorGroup());

    cg.setColor(QColorGroup::Foreground, Qt::green);
    cg.setColor(QColorGroup::Text, Qt::green);
    cg.setColor(QColorGroup::Background, Qt::black);

    QPalette newPalette(cg, cg, cg);

    setPalette(newPalette);
    d->picLabel->setPalette(newPalette);
    d->textLabel->setPalette(newPalette);

    QHBoxLayout * layout = new QHBoxLayout(this, 4, 4);

    layout->addWidget(d->picLabel, 0);
    layout->addWidget(d->textLabel, 1);

    d->status = Status::Playing;
    setStatus(Status::Stopped);
  }

  StatusView::~StatusView()
  {
    delete d;
  }

  void StatusView::setStatus(Status::Value newStatus)
  {
    if (newStatus == d->status)
      return;

    d->status = newStatus;

    switch (d->status)
    {
      case Status::Stopped:
        d->bufferingTimer->stop();
        d->picLabel->setPixmap(QPixmap((const char **)stopped_xpm));
        d->textLabel->setText(tr("Stopped"));
        break;

      case Status::Contacting:
        d->bufferingTimer->stop();
        d->picLabel->setPixmap(QPixmap((const char **)contacting_xpm));
        d->textLabel->setText(tr("Contacting host ..."));
        break;

      case Status::Buffering:
        if (!d->bufferingTimer->isActive())
        {
          d->bufferingTimer->start(0);
          d->textLabel->setText(tr("Buffering ..."));
        }
        break;

      case Status::Playing:
        d->bufferingTimer->stop();
        d->picLabel->setPixmap(QPixmap((const char **)playing_xpm));
        d->textLabel->setText(tr("Playing"));
        break;

      default:
        qDebug("StatusView::setStatus(): WTF?");
        break;
    }
  }

  void StatusView::slotUpdateBufferingPixmap()
  {
    if (3 == d->bufferingPixmapNumber)
      d->bufferingPixmapNumber = 0;
    else
      ++d->bufferingPixmapNumber;

    switch (d->bufferingPixmapNumber)
    {
      case 0:
        d->picLabel->setPixmap(QPixmap((const char **)buffering0_xpm));
        break;

      case 1:
        d->picLabel->setPixmap(QPixmap((const char **)buffering1_xpm));
        break;

      case 2:
        d->picLabel->setPixmap(QPixmap((const char **)buffering2_xpm));
        break;

      case 3:
        d->picLabel->setPixmap(QPixmap((const char **)buffering3_xpm));
        break;

      default:
        qDebug("StatusView::slotUpdateBufferingPixmap(): WTF?");
        break;
    }
  }
}

