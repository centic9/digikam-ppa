/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2014-09-18
 * Description : slideshow image widget
 *
 * Copyright (C) 2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef SLIDEIMAGE_H
#define SLIDEIMAGE_H

// Qt includes

#include <QWidget>

// KDE includes

#include <kurl.h>

// Local includes

#include "digikam_export.h"
#include "loadingdescription.h"

class PreviewSettings;

namespace Digikam
{

class DIGIKAM_EXPORT SlideImage : public QWidget
{
    Q_OBJECT

public:

    explicit SlideImage(QWidget* const parent = 0);
    ~SlideImage();

    void setPreviewSettings(const PreviewSettings& settings);
    void setLoadUrl(const KUrl& url);
    void setPreloadUrl(const KUrl& url);

Q_SIGNALS:

    void signalImageLoaded(bool);

private Q_SLOTS:

    void slotGotImagePreview(const LoadingDescription&, const DImg&);

private:

    void paintEvent(QPaintEvent*);
    void updatePixmap();

private:

    class Private;
    Private* const d;
};

}  // namespace Digikam

#endif /* SLIDEIMAGE_H */
