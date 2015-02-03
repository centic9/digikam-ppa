/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2007-03-20
 * Description : Container for image info objects
 *
 * Copyright (C) 2007-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#include "imageinfolist.h"

// Local includes

#include "imageinfo.h"

namespace Digikam
{

ImageInfoList::ImageInfoList(const QList<qlonglong>& idList)
{
    foreach(qlonglong id, idList)
    {
        append(ImageInfo(id));
    }
}

QList<qlonglong> ImageInfoList::toImageIdList() const
{
    QList<qlonglong> idList;

    foreach(const ImageInfo& info, *this)
    {
        idList << info.id();
    }

    return idList;
}

bool ImageInfoList::namefileLessThan(const ImageInfo &d1, const ImageInfo &d2)
{
    return d1.name().toLower() < d2.name().toLower(); // sort by name
}

// Implementations of batch loading methods: See imageinfo.cpp (next to the corresponding single-item implementation)

} // namespace Digikam
