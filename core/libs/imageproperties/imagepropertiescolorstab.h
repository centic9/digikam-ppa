/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2004-11-17
 * Description : a tab to display colors information of images
 *
 * Copyright (C) 2004-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef IMAGEPROPERTIESCOLORSTAB_H
#define IMAGEPROPERTIESCOLORSTAB_H

// Qt includes

#include <QWidget>
#include <QByteArray>

// KDE includes

#include <kurl.h>
#include <ktabwidget.h>

// Local includes

#include "dimg.h"
#include "digikam_export.h"

class QRect;

namespace Digikam
{

class DImg;
class LoadingDescription;

class DIGIKAM_EXPORT ImagePropertiesColorsTab : public KTabWidget
{
    Q_OBJECT

public:

    explicit ImagePropertiesColorsTab(QWidget* const parent);
    ~ImagePropertiesColorsTab();

    void setData(const KUrl& url=KUrl(), const QRect& selectionArea = QRect(), DImg* const img=0);

    void setSelection(const QRect& selectionArea);

    void readSettings(const KConfigGroup& group);
    void writeSettings(KConfigGroup& group);

private:

    void loadImageFromUrl(const KUrl& url);
    void updateInformation();
    void updateStatistics();
    void getICCData();

private Q_SLOTS:

    void slotRefreshOptions();
    void slotHistogramComputationFailed();
    void slotChannelChanged();
    void slotScaleChanged();
    void slotRenderingChanged(int rendering);
    void slotMinValueChanged(int);
    void slotMaxValueChanged(int);

    void slotUpdateInterval(int min, int max);
    void slotUpdateIntervalFromRGB(int min, int max);
    void slotUpdateIntervRange(int range);

    void slotLoadImageFromUrlComplete(const LoadingDescription& loadingDescription, const DImg& img);
    void slotMoreCompleteLoadingAvailable(const LoadingDescription& oldLoadingDescription,
                                          const LoadingDescription& newLoadingDescription);

private:

    class Private;
    Private* const d;
};

}  // namespace Digikam

#endif /* IMAGEPROPERTIESCOLORSTAB_H */
