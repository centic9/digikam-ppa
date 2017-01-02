/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2011-01-06
 * Description : Helper class for geomap interaction
 *
 * Copyright (C) 2011 by Michael G. Hansen <mike at mghansen dot de>
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

#ifndef GPSIMAGEINFO_H
#define GPSIMAGEINFO_H

// Qt includes

#include <QDateTime>
#include <QUrl>

// Local includes

#include "geocoordinates.h"
#include "groupstate.h"

namespace GeoIface
{
    class MapWidget;
}

namespace Digikam
{

class GPSImageInfo
{
public:

    GPSImageInfo();
    ~GPSImageInfo();

public:

    static GPSImageInfo fromIdCoordinatesRatingDateTime(const qlonglong p_id, const GeoIface::GeoCoordinates& p_coordinates,
                                                        const int p_rating, const QDateTime& p_creationDate);

public:

    qlonglong                   id;
    GeoIface::GeoCoordinates    coordinates;
    int                         rating;
    QDateTime                   dateTime;
    QUrl                        url;

    typedef QList<GPSImageInfo> List;
};

} /* namespace Digikam */

Q_DECLARE_METATYPE(Digikam::GPSImageInfo)

#endif /* GPSIMAGEINFO_H */
