/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : a plugin to create panorama by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011-2016 by Benjamin Girault <benjamin dot girault at gmail dot com>
 * Copyright (C) 2009-2017 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "panowizard.h"

// Qt includes

#include <QDesktopWidget>
#include <QApplication>
#include <QWindow>

// KDE includes

#include <kconfig.h>
#include <klocalizedstring.h>
#include <kwindowconfig.h>

// Local includes.

#include "panomanager.h"
#include "panointropage.h"
#include "panoitemspage.h"
#include "panopreprocesspage.h"
#include "panooptimizepage.h"
#include "panopreviewpage.h"
#include "panolastpage.h"

namespace Digikam
{

struct PanoWizard::Private
{
    Private()
      : mngr(0),
        introPage(0),
        itemsPage(0),
        preProcessingPage(0),
        optimizePage(0),
        previewPage(0),
        lastPage(0)
    {
    }

    PanoManager*           mngr;
    PanoIntroPage*         introPage;
    PanoItemsPage*         itemsPage;
    PanoPreProcessPage*    preProcessingPage;
    PanoOptimizePage*      optimizePage;
    PanoPreviewPage*       previewPage;
    PanoLastPage*          lastPage;
};

PanoWizard::PanoWizard(PanoManager* const mngr, QWidget* const parent)
    : QWizard(parent),
      d(new Private)
{
    setModal(false);
    setWindowTitle(i18nc("@title:window", "Panorama Creator Wizard"));

    d->mngr              = mngr;
    d->introPage         = new PanoIntroPage(d->mngr, this);
    d->itemsPage         = new PanoItemsPage(d->mngr, this);
    d->preProcessingPage = new PanoPreProcessPage(d->mngr, this);
    d->optimizePage      = new PanoOptimizePage(d->mngr, this);
    d->previewPage       = new PanoPreviewPage(d->mngr, this);
    d->lastPage          = new PanoLastPage(d->mngr, this);

    // ---------------------------------------------------------------

    KConfig config;
    KConfigGroup group = config.group("Panorama Dialog");

    if (group.exists())
    {
        winId();
        KWindowConfig::restoreWindowSize(windowHandle(), group);
        resize(windowHandle()->size());
    }
    else
    {
        QDesktopWidget* const desktop = QApplication::desktop();
        int screen                    = desktop->screenNumber();
        QRect srect                   = desktop->availableGeometry(screen);
        resize(800 <= srect.width()  ? 800 : srect.width(),
               750 <= srect.height() ? 750 : srect.height());
    }

    // ---------------------------------------------------------------

    connect(d->preProcessingPage, SIGNAL(signalPreProcessed(void)),
            this, SLOT(next(void)));

    connect(d->optimizePage, SIGNAL(signalOptimized(void)),
            this, SLOT(next(void)));

    connect(d->previewPage, SIGNAL(signalStitchingFinished(void)),
            this, SLOT(next(void)));

    connect(d->lastPage, SIGNAL(signalCopyFinished(void)),
            this, SLOT(accept(void)));
}

PanoWizard::~PanoWizard()
{
    KConfig config;
    KConfigGroup group = config.group("Panorama Dialog");
    KWindowConfig::saveWindowSize(windowHandle(), group);
    config.sync();

    delete d;
}

} // namespace Digikam
