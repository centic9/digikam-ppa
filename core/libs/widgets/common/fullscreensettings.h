/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2013-04-29
 * Description : a full screen settings widget
 *
 * Copyright (C) 2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef FULLSCREENSETTINGS_H
#define FULLSCREENSETTINGS_H

// Qt includes

#include <QGroupBox>

// KDE includes

#include <kconfiggroup.h>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT FullScreenSettings : public QGroupBox
{

public:

    explicit FullScreenSettings(int options, QWidget* const parent);
    virtual ~FullScreenSettings();

    void readSettings(const KConfigGroup& group);
    void saveSettings(KConfigGroup& group);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif /* FULLSCREENSETTINGS_H */
