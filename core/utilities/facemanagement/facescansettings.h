/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2010-10-09
 * Description : Face scan settings
 *
 * Copyright (C) 2010-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef FACESCANSETTINGS_H
#define FACESCANSETTINGS_H

// Local includes

#include "album.h"

namespace Digikam
{

class Album;

class FaceScanSettings
{
public:

    enum ScanTask
    {
        Detect,
        DetectAndRecognize,
        RecognizeMarkedFaces,
        RetrainAll,
        BenchmarkDetection,
        BenchmarkRecognition
    };

    // for detect and recognize
    enum AlreadyScannedHandling
    {
        Skip,
        Merge,
        Rescan
    };

public:

    FaceScanSettings()
    {
        useFullCpu             = true;
        accuracy               = 80;
        task                   = Detect;
        alreadyScannedHandling = Skip;
    }

    // processing power
    bool                   useFullCpu;

    // detection
    double                 accuracy;

    // albums to scan
    AlbumList              albums;

    ScanTask               task;

    AlreadyScannedHandling alreadyScannedHandling;
};

} // namespace Digikam

#endif // FACESCANSETTINGS_H
