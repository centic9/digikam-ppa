/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2008-26-02
 * Description : a widget to select an image collection
 *               to upload new items using digiKam album folder views
 *
 * Copyright (C) 2008-2017 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009      by Luka Renko <lure at kubuntu dot org>
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

#include "kipiuploadwidget.h"

// Qt includes

#include <QVBoxLayout>
#include <QPushButton>
#include <QApplication>
#include <QStyle>

// Libkipi includes

#include <libkipi_version.h>

// Local includes

#include "album.h"
#include "albumselectwidget.h"
#include "kipiinterface.h"
#include "kipiimagecollection.h"
#include "applicationsettings.h"

namespace Digikam
{

class KipiUploadWidget::Private
{
public:

    Private() :
        albumSel(0),
        iface(0)
    {
    }

    AlbumSelectWidget* albumSel;

    KipiInterface*     iface;
};

KipiUploadWidget::KipiUploadWidget(KipiInterface* const iface, QWidget* const parent)
    : KIPI::UploadWidget(parent),
      d(new Private)
{
    d->iface                = iface;
    QVBoxLayout* const vlay = new QVBoxLayout(this);
    d->albumSel             = new AlbumSelectWidget(this);
    vlay->addWidget(d->albumSel);
    vlay->setContentsMargins(QMargins());
    vlay->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));

    connect(d->albumSel, SIGNAL(itemSelectionChanged()),
            this, SLOT(slotSelectionChanged()));
}

KipiUploadWidget::~KipiUploadWidget()
{
    delete d;
}

KIPI::ImageCollection KipiUploadWidget::selectedImageCollection() const
{
    KIPI::ImageCollection collection;

    if (d->iface)
    {
        QString ext                = ApplicationSettings::instance()->getAllFileFilter();
        PAlbum* const currentAlbum = d->albumSel->currentAlbum();

        if (currentAlbum)
        {
            collection = new KipiImageCollection(KipiImageCollection::AllItems, currentAlbum, ext);
        }
    }

    return collection;
}

void KipiUploadWidget::slotSelectionChanged()
{
    PAlbum* const currentAlbum = d->albumSel->currentAlbum();

    // TODO is this the desired semantic?
    if (!currentAlbum || (currentAlbum->isRoot()))
    {
        emit selectionChanged();
    }
}

}  // namespace Digikam
