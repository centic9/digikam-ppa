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

#ifndef DIGIKAM_KIPIUPLOADWIDGET_H
#define DIGIKAM_KIPIUPLOADWIDGET_H

// Libkipi includes

#include <KIPI/ImageCollection>
#include <KIPI/UploadWidget>

class QWidget;

namespace Digikam
{
class KipiInterface;

class KipiUploadWidget : public KIPI::UploadWidget
{
    Q_OBJECT

public:

    explicit KipiUploadWidget(KipiInterface* const iface, QWidget* const parent = 0);
    ~KipiUploadWidget();

    KIPI::ImageCollection selectedImageCollection() const;

private Q_SLOTS:

    void slotSelectionChanged();

private:

    class Private;
    Private* const d;
};

}  // namespace Digikam

#endif  // DIGIKAM_KIPIUPLOADWIDGET_H
