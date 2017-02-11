/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2008-09-24
 * Description : DNG save settings widgets
 *
 * Copyright (C) 2008-2017 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DNGSETTINGS_H
#define DNGSETTINGS_H

// Qt includes

#include <QWidget>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT DNGSettings : public QWidget
{
    Q_OBJECT

public:

    DNGSettings(QWidget* const parent=0);
    ~DNGSettings();

    void setCompressLossLess(bool b);
    bool compressLossLess() const;

    void setPreviewMode(int mode);
    int  previewMode() const;

    void setBackupOriginalRawFile(bool b);
    bool backupOriginalRawFile() const;

    void setDefaultSettings();

Q_SIGNALS:

    void signalSettingsChanged();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif /* DNGSETTINGS_H */
