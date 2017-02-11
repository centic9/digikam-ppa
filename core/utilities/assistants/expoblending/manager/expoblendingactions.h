/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2009-11-13
 * Description : a plugin to blend bracketed images.
 *
 * Copyright (C) 2009-2017 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2015      by Benjamin Girault, <benjamin dot girault at gmail dot com>
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

#ifndef EXPOBLENDINGACTIONS_H
#define EXPOBLENDINGACTIONS_H

// Qt includes

#include <QString>
#include <QImage>
#include <QMetaType>
#include <QMap>

// Local includes

#include "enfusesettings.h"

namespace Digikam
{

enum ExpoBlendingAction
{
    EXPOBLENDING_NONE = 0,
    EXPOBLENDING_IDENTIFY,
    EXPOBLENDING_PREPROCESSING,
    EXPOBLENDING_ENFUSEPREVIEW,
    EXPOBLENDING_ENFUSEFINAL,
    EXPOBLENDING_LOAD
};

class ExpoBlendingItemPreprocessedUrls
{
public:

    ExpoBlendingItemPreprocessedUrls()
    {
    };

    ExpoBlendingItemPreprocessedUrls(const QUrl& preprocessed, const QUrl& preview)
        : preprocessedUrl(preprocessed),
          previewUrl(preview)
    {
    };

    virtual ~ExpoBlendingItemPreprocessedUrls()
    {
    };

    QUrl preprocessedUrl;              // Can be original file or aligned version, depending of user choice.
    QUrl previewUrl;                   // The JPEG preview version, accordingly of preprocessedUrl constent.
};

typedef QMap<QUrl, ExpoBlendingItemPreprocessedUrls> ExpoBlendingItemUrlsMap;   // Map between original Url and processed temp Urls.

class ExpoBlendingActionData
{

public:

    ExpoBlendingActionData()
        : starting(false),
          success(false),
          action(EXPOBLENDING_NONE)
    {
    }

    bool               starting;
    bool               success;

    QString            message;

    QImage             image;

    QList<QUrl>        inUrls;
    QList<QUrl>        outUrls;

    EnfuseSettings     enfuseSettings;

    ExpoBlendingItemUrlsMap        preProcessedUrlsMap;

    ExpoBlendingAction action;
};

}  // namespace Digikam

Q_DECLARE_METATYPE(Digikam::ExpoBlendingActionData)
Q_DECLARE_METATYPE(Digikam::ExpoBlendingItemPreprocessedUrls)

#endif /* EXPOBLENDINGACTIONS_H */
