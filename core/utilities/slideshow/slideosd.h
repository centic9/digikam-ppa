/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2014-09-18
 * Description : slideshow OSD widget
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

#ifndef OSD_WIDGET_H
#define OSD_WIDGET_H

// Qt includes

#include <QWidget>

// KDE includes

#include <kurl.h>

// Local includes

#include "slideshowsettings.h"

class QEvent;

namespace Digikam
{

class SlideShow;
class SlideToolBar;

class SlideOSD : public QWidget
{
    Q_OBJECT

public:

    SlideOSD(const SlideShowSettings& settings, SlideShow* const parent = 0);
    ~SlideOSD();

    void setCurrentInfo(const SlidePictureInfo& info, const KUrl& url);

    void pause(bool b);
    bool isPaused() const;

    SlideToolBar* toolBar() const;

private Q_SLOTS:

    void slotProgressTimer();
    void slotSlideTimer();

private:

    bool eventFilter(QObject* obj, QEvent* ev);

private:

    class Private;
    Private* const d;
};

}  // namespace Digikam

#endif  // OSD_WIDGET_H
