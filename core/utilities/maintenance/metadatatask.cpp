/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2013-08-09
 * Description : Thread actions task for metadata synchronizer.
 *
 * Copyright (C) 2013-2017 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "metadatatask.h"

// Local includes

#include "collectionscanner.h"
#include "metadatahub.h"
#include "digikam_debug.h"

namespace Digikam
{

class MetadataTask::Private
{
public:

    Private()
    {
        cancel    = false;
        direction = MetadataSynchronizer::WriteFromDatabaseToFile;
        tagsOnly  = false;
    }

    bool                                cancel;
    bool                                tagsOnly;

    ImageInfo                           item;
    MetadataSynchronizer::SyncDirection direction;
};

// -------------------------------------------------------

MetadataTask::MetadataTask()
    : ActionJob(),
      d(new Private)
{
}

MetadataTask::~MetadataTask()
{
    slotCancel();
    delete d;
}

void MetadataTask::setItem(const ImageInfo& item, MetadataSynchronizer::SyncDirection dir)
{
    d->item      = item;
    d->direction = dir;
}

void MetadataTask::setTagsOnly(bool value)
{
    d->tagsOnly = value;
}
void MetadataTask::slotCancel()
{
    d->cancel = true;
}

void MetadataTask::run()
{
    if (d->cancel)
    {
        return;
    }

    if (d->direction == MetadataSynchronizer::WriteFromDatabaseToFile)
    {
        MetadataHub fileHub;

        // read in from database
        fileHub.load(d->item);

        // write out to file DMetadata
        if(d->tagsOnly)
        {
            fileHub.writeTags(d->item.filePath());
        }
        else
        {
            fileHub.write(d->item.filePath(), MetadataHub::WRITE_ALL, true);
        }
    }
    else // MetadataSynchronizer::ReadFromFileToDatabase
    {
        CollectionScanner scanner;
        scanner.scanFile(d->item, CollectionScanner::Rescan);
    }

    emit signalFinished(QImage());
    emit signalDone();
}

}  // namespace Digikam
