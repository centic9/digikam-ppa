/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2012-01-31
 * Description : maintenance manager settings
 *
 * Copyright (C) 2012-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "maintenancesettings.h"

namespace Digikam
{

MaintenanceSettings::MaintenanceSettings()
{
    wholeAlbums        = true;
    wholeTags          = true;
    useMutiCoreCPU     = false;

    newItems           = false;

    thumbnails         = false;
    scanThumbs         = false;

    fingerPrints       = false;
    scanFingerPrints   = false;

    duplicates         = false;
    similarity         = 90;

    faceManagement     = false;

    qualitySort        = false;
    qualityScanMode    = true;   // NOTE: turn on by default to prevent clearing whole Pick Labels from Collection

    metadataSync       = false;
    syncDirection      = MetadataSynchronizer::WriteFromDatabaseToFile;
}

MaintenanceSettings::~MaintenanceSettings()
{
}

//! kDebug() stream operator. Writes property @a s to the debug output in a nicely formatted way.
QDebug operator<<(QDebug dbg, const MaintenanceSettings& s)
{
    dbg.nospace() << endl;
    dbg.nospace() << "wholeAlbums         : " << s.wholeAlbums << endl;
    dbg.nospace() << "wholeTags           : " << s.wholeTags << endl;
    dbg.nospace() << "Albums              : " << s.albums.count() << endl;
    dbg.nospace() << "Tags                : " << s.tags.count() << endl;
    dbg.nospace() << "useMutiCoreCPU      : " << s.useMutiCoreCPU << endl;
    dbg.nospace() << "newItems            : " << s.newItems << endl;
    dbg.nospace() << "thumbnails          : " << s.thumbnails << endl;
    dbg.nospace() << "scanThumbs          : " << s.scanThumbs << endl;
    dbg.nospace() << "fingerPrints        : " << s.fingerPrints << endl;
    dbg.nospace() << "scanFingerPrints    : " << s.scanFingerPrints << endl;
    dbg.nospace() << "duplicates          : " << s.duplicates << endl;
    dbg.nospace() << "similarity          : " << s.similarity << endl;
    dbg.nospace() << "faceManagement      : " << s.faceManagement << endl;
    dbg.nospace() << "faceScannedHandling : " << s.faceSettings.alreadyScannedHandling << endl;
    dbg.nospace() << "qualitySort         : " << s.qualitySort << endl;
    dbg.nospace() << "quality             : " << s.quality << endl;
    dbg.nospace() << "qualityScanMode     : " << s.qualityScanMode << endl;
    dbg.nospace() << "metadataSync        : " << s.metadataSync << endl;
    dbg.nospace() << "syncDirection       : " << s.syncDirection << endl;
    return dbg.space();
}

}  // namespace Digikam
