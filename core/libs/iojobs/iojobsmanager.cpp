/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2015-06-15
 * Description : Manager for creating and starting IO jobs threads
 *
 * Copyright (C) 2015 by Mohamed Anwer <m dot anwer at gmx dot com>
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

#include "iojobsmanager.h"

namespace Digikam
{

class IOJobsManagerCreator
{
public:

    IOJobsManager object;
};

Q_GLOBAL_STATIC(IOJobsManagerCreator, creator)

// ----------------------------------------------

IOJobsManager::IOJobsManager()
{
}

IOJobsManager* IOJobsManager::instance()
{
    return& creator->object;
}

IOJobsThread* IOJobsManager::startCopy(const QList<QUrl>& srcsList, const QUrl& destAlbum)
{
    IOJobsThread* const thread = new IOJobsThread(this);
    thread->copy(srcsList, destAlbum);
    thread->start();

    return thread;
}

IOJobsThread* IOJobsManager::startMove(const QList<QUrl>& srcsList, const QUrl& destAlbum)
{
    IOJobsThread* const thread = new IOJobsThread(this);
    thread->move(srcsList, destAlbum);
    thread->start();

    return thread;
}

IOJobsThread* IOJobsManager::startDelete(const QList<QUrl>& filesToDelete, bool useTrash)
{
    IOJobsThread* const thread = new IOJobsThread(this);
    thread->del(filesToDelete, useTrash);
    thread->start();

    return thread;
}

IOJobsThread* IOJobsManager::startRenameFile(const QUrl& srcToRename, const QUrl& newUrl)
{
    IOJobsThread* const thread = new IOJobsThread(this);
    thread->renameFile(srcToRename, newUrl);
    thread->start();

    return thread;
}

IOJobsThread *IOJobsManager::startDTrashItemsListingForCollection(const QString& collectionPath)
{
    IOJobsThread* const thread = new IOJobsThread(this);
    thread->listDTrashItems(collectionPath);
    thread->start();

    return thread;
}

IOJobsThread* IOJobsManager::startRestoringDTrashItems(const DTrashItemInfoList& trashItemsList)
{
    IOJobsThread* const thread = new IOJobsThread(this);
    thread->restoreDTrashItems(trashItemsList);
    thread->start();

    return thread;
}

IOJobsThread* IOJobsManager::startDeletingDTrashItems(const DTrashItemInfoList& trashItemsList)
{
    IOJobsThread* const thread = new IOJobsThread(this);
    thread->deleteDTrashItems(trashItemsList);
    thread->start();

    return thread;
}

} // namespace Digikam
