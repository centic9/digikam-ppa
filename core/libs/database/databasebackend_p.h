/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2009-06-07
 * Description : Abstract database backend
 *
 * Copyright (C) 2007-2009 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef DATABASEBACKEND_P_H
#define DATABASEBACKEND_P_H


#include "databasecorebackend_p.h"

// Local includes

#include "databasewatch.h"

namespace Digikam
{

class DatabaseWatch;

class DatabaseBackendPrivate : public DatabaseCoreBackendPrivate
{
public:

    explicit DatabaseBackendPrivate(DatabaseBackend* backend)
        : DatabaseCoreBackendPrivate(backend),
          imageChangesetContainer(this),
          imageTagChangesetContainer(this),
          collectionImageChangesetContainer(this),
          albumChangesetContainer(this),
          tagChangesetContainer(this),
          albumRootChangesetContainer(this),
          searchChangesetContainer(this)
    {
        watch = 0;
    }

    DatabaseWatch*       watch;

    void sendToWatch(const ImageChangeset changeset)
    {
        watch->sendImageChange(changeset);
    }
    void sendToWatch(const ImageTagChangeset changeset)
    {
        watch->sendImageTagChange(changeset);
    }
    void sendToWatch(const CollectionImageChangeset changeset)
    {
        watch->sendCollectionImageChange(changeset);
    }
    void sendToWatch(const AlbumChangeset changeset)
    {
        watch->sendAlbumChange(changeset);
    }
    void sendToWatch(const TagChangeset changeset)
    {
        watch->sendTagChange(changeset);
    }
    void sendToWatch(const AlbumRootChangeset changeset)
    {
        watch->sendAlbumRootChange(changeset);
    }
    void sendToWatch(const SearchChangeset changeset)
    {
        watch->sendSearchChange(changeset);
    }

    template <class T>
    class ChangesetContainer
    {
    public:

        explicit ChangesetContainer(DatabaseBackendPrivate* d)
            : d(d)
        {
        }

        void recordChangeset(const T& changeset)
        {
            if (d->isInTransaction)
            {
                changesets << changeset;
            }
            else
            {
                d->sendToWatch(changeset);
            }
        }

        void sendOut()
        {
            foreach(const T& changeset, changesets)
            {
                d->sendToWatch(changeset);
            }
            changesets.clear();
        }

        QList<T>                      changesets;
        DatabaseBackendPrivate* const d;
    };

    ChangesetContainer<ImageChangeset>           imageChangesetContainer;
    ChangesetContainer<ImageTagChangeset>        imageTagChangesetContainer;
    ChangesetContainer<CollectionImageChangeset> collectionImageChangesetContainer;
    ChangesetContainer<AlbumChangeset>           albumChangesetContainer;
    ChangesetContainer<TagChangeset>             tagChangesetContainer;
    ChangesetContainer<AlbumRootChangeset>       albumRootChangesetContainer;
    ChangesetContainer<SearchChangeset>          searchChangesetContainer;

    void transactionFinished()
    {
        DatabaseCoreBackendPrivate::transactionFinished();

        imageChangesetContainer.sendOut();
        imageTagChangesetContainer.sendOut();
        collectionImageChangesetContainer.sendOut();
        albumChangesetContainer.sendOut();
        tagChangesetContainer.sendOut();
        albumRootChangesetContainer.sendOut();
        searchChangesetContainer.sendOut();
    }

};

}  // namespace Digikam

#endif // DATABASEBACKEND_P_H

