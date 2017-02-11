/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2009-03-04
 * Description : a tool to adjust date time stamp of images
 *
 * Copyright (C) 2009-2017 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef TIMEADJUST_H
#define TIMEADJUST_H

// Local includes

#include "batchtool.h"

namespace Digikam
{
class TimeAdjustSettings;

class TimeAdjust : public BatchTool
{
    Q_OBJECT

public:

    explicit TimeAdjust(QObject* const parent = 0);
    ~TimeAdjust();

    BatchToolSettings defaultSettings();

    BatchTool* clone(QObject* const parent=0) const { return new TimeAdjust(parent); };

    void registerSettingsWidget();

private:

    bool toolOperations();

private Q_SLOTS:

    void slotAssignSettings2Widget();
    void slotSettingsChanged();

private:

    TimeAdjustSettings* m_taWidget;
    int                 m_changeSettings;
};

}  // namespace Digikam

#endif /* TIMEADJUST_H */
