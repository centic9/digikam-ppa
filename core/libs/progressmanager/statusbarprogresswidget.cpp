/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2012-01-13
 * Description : progress manager
 *
 * Copyright (C) 2007-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2004      by Till Adam <adam at kde dot org>
 * Copyright (C) 2004      by David Faure <faure at kde dot org>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "statusbarprogresswidget.moc"

// Qt includes

#include <QEvent>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QProgressBar>
#include <QPushButton>
#include <QStackedWidget>
#include <QTimer>

// KDE includes

#include <klocale.h>
#include <kiconloader.h>
#include <kdebug.h>

// Local includes

#include "progressview.h"
#include "progressmanager.h"

namespace Digikam
{

class StatusbarProgressWidget::Private
{
public:

    enum Mode
    {
        None,
        Progress
    };

public:

    Private() :
        mode(None),
        bShowButton(true),
        pProgressBar(0),
        pLabel(0),
        pButton(0),
        box(0),
        stack(0),
        currentItem(0),
        progressView(0),
        delayTimer(0),
        busyTimer(0),
        cleanTimer(0)
    {
    }

    uint            mode;
    bool            bShowButton;

    QProgressBar*   pProgressBar;
    QLabel*         pLabel;
    QPushButton*    pButton;

    QBoxLayout*     box;
    QStackedWidget* stack;
    ProgressItem*   currentItem;
    ProgressView*   progressView;
    QTimer*         delayTimer;
    QTimer*         busyTimer;
    QTimer*         cleanTimer;
};

StatusbarProgressWidget::StatusbarProgressWidget(ProgressView* const progressView, QWidget* const parent, bool button)
    : QFrame(parent), d(new Private)
{
    d->progressView      = progressView;
    d->bShowButton       = button;

    int w  = fontMetrics().width(" 999.9 kB/s 00:00:01 ") + 8;
    d->box = new QHBoxLayout(this);
    d->box->setMargin(0);
    d->box->setSpacing(0);

    d->pButton        = new QPushButton(this);
    d->pButton->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
    QPixmap smallIcon = SmallIcon("go-up");
    d->pButton->setIcon(smallIcon);
    d->box->addWidget(d->pButton);
    d->stack          = new QStackedWidget(this);
    int maximumHeight = qMax(smallIcon.height(), fontMetrics().height());
    d->stack->setMaximumHeight(maximumHeight);
    d->box->addWidget(d->stack);

    d->pButton->setToolTip(i18n("Open detailed progress dialog"));

    d->pProgressBar = new QProgressBar(this);
    d->pProgressBar->installEventFilter(this);
    d->pProgressBar->setMinimumWidth(w);
    d->stack->insertWidget(1, d->pProgressBar);

    d->pLabel = new QLabel(i18n("No active process"), this);
    d->pLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    d->pLabel->installEventFilter(this);
    d->pLabel->setMinimumWidth(w);
    d->stack->insertWidget(2, d->pLabel);
    d->pButton->setMaximumHeight(maximumHeight);
    setMinimumWidth(minimumSizeHint().width());

    setMode();

    // -------------------------------------------------------------------------------------------------

    connect(d->pButton, SIGNAL(clicked()),
            progressView, SLOT(slotToggleVisibility()));

    connect(ProgressManager::instance(), SIGNAL(progressItemAdded(ProgressItem*)),
            this, SLOT(slotProgressItemAdded(ProgressItem*)));

    connect(ProgressManager::instance(), SIGNAL(progressItemCompleted(ProgressItem*)),
            this, SLOT(slotProgressItemCompleted(ProgressItem*)));

    connect(ProgressManager::instance(), SIGNAL(progressItemUsesBusyIndicator(ProgressItem*,bool)),
            this, SLOT(updateBusyMode()));

    connect(progressView, SIGNAL(visibilityChanged(bool)),
            this, SLOT(slotProgressViewVisible(bool)));

    d->delayTimer = new QTimer(this);
    d->delayTimer->setSingleShot(true);

    connect(d->delayTimer, SIGNAL(timeout()),
            this, SLOT(slotShowItemDelayed()));

    d->cleanTimer = new QTimer(this);
    d->cleanTimer->setSingleShot(true);

    connect(d->cleanTimer, SIGNAL(timeout()),
            this, SLOT(slotClean()));
}

StatusbarProgressWidget::~StatusbarProgressWidget()
{
    delete d;
}

// There are three cases: no progressitem, one progressitem (connect to it directly),
// or many progressitems (display busy indicator). Let's call them 0,1,N.
// In slot..Added we can only end up in 1 or N.
// In slot..Removed we can end up in 0, 1, or we can stay in N if we were already.

void StatusbarProgressWidget::updateBusyMode()
{
    connectSingleItem(); // if going to 1 item

    if (d->currentItem)
    {
        // Exactly one item
        delete d->busyTimer;
        d->busyTimer = 0;
        d->delayTimer->start( 1000 );
    }
    else
    {
        // N items
        if (!d->busyTimer)
        {
            d->busyTimer = new QTimer(this);

            connect(d->busyTimer, SIGNAL(timeout()),
                    this, SLOT(slotBusyIndicator()));

            d->delayTimer->start(1000);
        }
    }
}

void StatusbarProgressWidget::slotProgressItemAdded(ProgressItem* item)
{
    if (item->parent())
        return; // we are only interested in top level items

    updateBusyMode();
}

void StatusbarProgressWidget::slotProgressItemCompleted(ProgressItem* item)
{
    if (item && item->parent())
        return; // we are only interested in top level items

    connectSingleItem(); // if going back to 1 item

    if (ProgressManager::instance()->isEmpty())
    {
        // No item
        // Done. In 5s the progress-widget will close, then we can clean up the statusbar
        d->cleanTimer->start(5000);
    }
    else if (d->currentItem)
    {
        // Exactly one item
        delete d->busyTimer;
        d->busyTimer = 0;
        activateSingleItemMode();
    }
}

void StatusbarProgressWidget::connectSingleItem()
{
    if (d->currentItem)
    {
        disconnect(d->currentItem, SIGNAL(progressItemProgress(ProgressItem*,uint)),
                   this, SLOT(slotProgressItemProgress(ProgressItem*,uint)));
        d->currentItem = 0;
    }

    d->currentItem = ProgressManager::instance()->singleItem();

    if (d->currentItem)
    {
        connect(d->currentItem, SIGNAL(progressItemProgress(ProgressItem*,uint)),
                this, SLOT(slotProgressItemProgress(ProgressItem*,uint)));
    }
}

void StatusbarProgressWidget::activateSingleItemMode()
{
    d->pProgressBar->setMaximum(100);
    d->pProgressBar->setValue(d->currentItem->progress());
    d->pProgressBar->setTextVisible(true);
}

void StatusbarProgressWidget::slotShowItemDelayed()
{
    bool noItems = ProgressManager::instance()->isEmpty();

    if (d->currentItem)
    {
        activateSingleItemMode();
    }
    else if (!noItems)
    {
        // N items
        d->pProgressBar->setMaximum(0);
        d->pProgressBar->setTextVisible(false);

        if (d->busyTimer)
            d->busyTimer->start(100);
    }

    if (!noItems && d->mode == Private::None)
    {
        d->mode = Private::Progress;
        setMode();
    }
}

void StatusbarProgressWidget::slotBusyIndicator()
{
    int p = d->pProgressBar->value();
    d->pProgressBar->setValue(p + 10);
}

void StatusbarProgressWidget::slotProgressItemProgress(ProgressItem* item, unsigned int value)
{
    if (item != d->currentItem) // single item mode; discard others
    {
        return;
    }

    d->pProgressBar->setValue(value);
}

void StatusbarProgressWidget::setMode()
{
    switch (d->mode)
    {
        case Private::None:
        {
            if (d->bShowButton)
            {
                d->pButton->hide();
            }

            d->stack->show();
            d->stack->setCurrentWidget(d->pLabel);

            break;
        }
        case Private::Progress:
        {
            d->stack->show();
            d->stack->setCurrentWidget(d->pProgressBar);

            if (d->bShowButton)
            {
                d->pButton->show();
            }

            break;
        }
    }
}

void StatusbarProgressWidget::slotClean()
{
    // check if a new item showed up since we started the timer. If not, clear
    if (ProgressManager::instance()->isEmpty())
    {
        d->pProgressBar->setValue(0);
        //d->pLabel->clear();
        d->mode = Private::None;
        setMode();
    }
}

bool StatusbarProgressWidget::eventFilter(QObject*, QEvent* ev)
{
    if (ev->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* const e = (QMouseEvent*)ev;

        if (e->button() == Qt::LeftButton && d->mode != Private::None)
        {
            // toggle view on left mouse button
            // Consensus seems to be that we should show/hide the fancy dialog when the user
            // clicks anywhere in the small one.
            d->progressView->slotToggleVisibility();
            return true;
        }
    }
    return false;
}

void StatusbarProgressWidget::slotProgressViewVisible(bool b)
{
    // Update the hide/show button when the detailed one is shown/hidden
    if (b)
    {
        d->pButton->setIcon(SmallIcon("go-down"));
        d->pButton->setToolTip(i18n("Hide detailed progress window"));
        setMode();
    }
    else
    {
        d->pButton->setIcon(SmallIcon("go-up"));
        d->pButton->setToolTip(i18n("Show detailed progress window"));
    }
}

} // namespace Digikam
