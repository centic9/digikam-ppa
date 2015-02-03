/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-04-14
 * Description : Load and cache tag thumbnails
 *
 * Copyright (C) 2006-2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#include "albumthumbnailloader.moc"

// C++ includes

#include <cmath>

// Qt includes

#include <QEvent>
#include <QList>
#include <QMap>
#include <QPainter>
#include <QPixmap>

// KDE includes

#include <kapplication.h>
#include <kiconloader.h>

// Local includes

#include "album.h"
#include "albummanager.h"
#include "albumsettings.h"
#include "metadatasettings.h"
#include "thumbnailloadthread.h"
#include "thumbnailsize.h"

namespace Digikam
{

typedef QMap<QString, QList<int> > PathAlbumMap;
typedef QMap<int, QPixmap> AlbumThumbnailMap;

class AlbumThumbnailLoader::Private
{
public:

    Private()
    {
        iconSize             = AlbumSettings::instance()->getTreeViewIconSize();
        minBlendSize         = 20;
        iconAlbumThumbThread = 0;
        iconTagThumbThread   = 0;
    }

    int                  iconSize;
    int                  minBlendSize;

    ThumbnailLoadThread* iconTagThumbThread;
    ThumbnailLoadThread* iconAlbumThumbThread;

    PathAlbumMap         pathAlbumMap;

    AlbumThumbnailMap    thumbnailMap;
};

class AlbumThumbnailLoaderCreator
{
public:

    AlbumThumbnailLoader object;
};

K_GLOBAL_STATIC(AlbumThumbnailLoaderCreator, creator)

// ---------------------------------------------------------------------------------------------

AlbumThumbnailLoader* AlbumThumbnailLoader::instance()
{
    return &creator->object;
}

AlbumThumbnailLoader::AlbumThumbnailLoader()
    : d(new Private)
{
    connect(this, SIGNAL(signalDispatchThumbnailInternal(int,QPixmap)),
            this, SLOT(slotDispatchThumbnailInternal(int,QPixmap)));

    connect(AlbumManager::instance(), SIGNAL(signalAlbumIconChanged(Album*)),
            this, SLOT(slotIconChanged(Album*)));

    connect(AlbumManager::instance(), SIGNAL(signalAlbumDeleted(Album*)),
            this, SLOT(slotIconChanged(Album*)));
}

AlbumThumbnailLoader::~AlbumThumbnailLoader()
{
    delete d->iconTagThumbThread;
    delete d->iconAlbumThumbThread;
    delete d;
}

void AlbumThumbnailLoader::cleanUp()
{
    delete d->iconTagThumbThread;
    d->iconTagThumbThread = 0;
    delete d->iconAlbumThumbThread;
    d->iconAlbumThumbThread = 0;
}

QPixmap AlbumThumbnailLoader::getStandardTagIcon(RelativeSize relativeSize)
{
    return loadIcon("tag", computeIconSize(relativeSize));
}

QPixmap AlbumThumbnailLoader::getStandardTagRootIcon(RelativeSize relativeSize)
{
    return loadIcon("tag-folder", computeIconSize(relativeSize));
}

QPixmap AlbumThumbnailLoader::getStandardTagIcon(TAlbum* const album, RelativeSize relativeSize)
{
    if (album->isRoot())
    {
        return getStandardTagRootIcon(relativeSize);
    }
    else
    {
        return getStandardTagIcon(relativeSize);
    }
}

QPixmap AlbumThumbnailLoader::getNewTagIcon(RelativeSize relativeSize)
{
    return loadIcon("tag-new", computeIconSize(relativeSize));
}

QPixmap AlbumThumbnailLoader::getStandardAlbumIcon(RelativeSize relativeSize)
{
    return loadIcon("folder", computeIconSize(relativeSize));
}

QPixmap AlbumThumbnailLoader::getStandardAlbumRootIcon(RelativeSize relativeSize)
{
    return loadIcon("folder-image", computeIconSize(relativeSize));
}

QPixmap AlbumThumbnailLoader::getStandardAlbumIcon(PAlbum* const album, RelativeSize relativeSize)
{
    if (album->isRoot() || album->isAlbumRoot())
    {
        return getStandardAlbumRootIcon(relativeSize);
    }
    else
    {
        return getStandardAlbumIcon(relativeSize);
    }
}

int AlbumThumbnailLoader::computeIconSize(RelativeSize relativeSize)
{
    if (relativeSize == SmallerSize)
    {
        // when size was 32 smaller was 20. Scale.
        return lround(20.0 / 32.0 * (double)d->iconSize);
    }

    return d->iconSize;
}

QPixmap AlbumThumbnailLoader::loadIcon(const QString& name, int size)
{
    KIconLoader* const iconLoader = KIconLoader::global();
    return iconLoader->loadIcon(name, KIconLoader::NoGroup, size);
}

bool AlbumThumbnailLoader::getTagThumbnail(TAlbum* const album, QPixmap& icon)
{
    if (!album->icon().isEmpty() && d->iconSize > d->minBlendSize)
    {
        if (album->icon().startsWith('/'))
        {
            KUrl iconKURL;
            iconKURL.setPath(album->icon());
            addUrl(album, iconKURL);
            icon = QPixmap();
            return true;
        }
        else
        {
            icon = loadIcon(album->icon(), d->iconSize);
            return false;
        }
    }
    else
    {
        icon = QPixmap();
        return false;
    }
}

QPixmap AlbumThumbnailLoader::getTagThumbnailDirectly(TAlbum* const album)
{
    if (!album->icon().isEmpty())
    {
        // icon cached?
        AlbumThumbnailMap::const_iterator it = d->thumbnailMap.constFind(album->globalID());

        if (it != d->thumbnailMap.constEnd())
        {
            return *it;
        }

        if (album->icon().startsWith('/'))
        {
            KUrl iconKURL;
            iconKURL.setPath(album->icon());
            addUrl(album, iconKURL);
        }
        else
        {
            QPixmap pixmap = loadIcon(album->icon(), d->iconSize);
            return pixmap;
        }
    }

    return getStandardTagIcon(album);
}

bool AlbumThumbnailLoader::getAlbumThumbnail(PAlbum* const album)
{
    if (!album->icon().isEmpty() && d->iconSize > d->minBlendSize)
    {
        addUrl(album, album->iconKURL());
    }
    else
    {
        return false;
    }

    return true;
}

QPixmap AlbumThumbnailLoader::getAlbumThumbnailDirectly(PAlbum* const album)
{
    if (!album->icon().isEmpty() && d->iconSize > d->minBlendSize)
    {
        // icon cached?
        AlbumThumbnailMap::const_iterator it = d->thumbnailMap.constFind(album->globalID());

        if (it != d->thumbnailMap.constEnd())
        {
            return *it;
        }

        // schedule for loading
        addUrl(album, album->iconKURL());
    }

    return getStandardAlbumIcon(album);
}

void AlbumThumbnailLoader::addUrl(Album* const album, const KUrl& url)
{
    /*
    QPixmap* pix = d->cache->find(album->iconKURL().toLocalFile());
    if (pix)
    return pix;
    */

    // First check cached thumbnails.
    // We use a private cache which is actually a map to be sure to cache _all_ album thumbnails.
    // At startup, this is not relevant, as the views will add their requests in a row.
    // This is to speed up context menu and IE imagedescedit
    AlbumThumbnailMap::const_iterator ttit = d->thumbnailMap.constFind(album->globalID());

    if (ttit != d->thumbnailMap.constEnd())
    {
        // It is not necessary to return cached icon asynchronously - they could be
        // returned by getTagThumbnail already - but this would make the API
        // less elegant, it feels much better this way.
        emit signalDispatchThumbnailInternal(album->globalID(), *ttit);
        return;
    }

    // Check if the URL has already been added
    PathAlbumMap::iterator it = d->pathAlbumMap.find(url.toLocalFile());

    if (it == d->pathAlbumMap.end())
    {
        // use two IOslaves so that tag and album thumbnails are loaded
        // in parallel and not first album, then tag thumbnails
        if (album->type() == Album::TAG)
        {
            if (!d->iconTagThumbThread)
            {
                d->iconTagThumbThread = new ThumbnailLoadThread();
                d->iconTagThumbThread->setThumbnailSize(d->iconSize);
                d->iconTagThumbThread->setSendSurrogatePixmap(false);

                connect(d->iconTagThumbThread,
                        SIGNAL(signalThumbnailLoaded(LoadingDescription,QPixmap)),
                        SLOT(slotGotThumbnailFromIcon(LoadingDescription,QPixmap)),
                        Qt::QueuedConnection);
            }

            // use the asynchronous version - with queued connections, see above
            d->iconTagThumbThread->find(url.toLocalFile());
        }
        else
        {
            if (!d->iconAlbumThumbThread)
            {
                d->iconAlbumThumbThread = new ThumbnailLoadThread();
                d->iconAlbumThumbThread->setThumbnailSize(d->iconSize);
                d->iconAlbumThumbThread->setSendSurrogatePixmap(false);

                connect(d->iconAlbumThumbThread,
                        SIGNAL(signalThumbnailLoaded(LoadingDescription,QPixmap)),
                        SLOT(slotGotThumbnailFromIcon(LoadingDescription,QPixmap)),
                        Qt::QueuedConnection);
            }

            d->iconAlbumThumbThread->find(url.toLocalFile());
        }

        // insert new entry to map, add album globalID
        QList<int> &list = d->pathAlbumMap[url.toLocalFile()];
        list.removeAll(album->globalID());
        list.push_back(album->globalID());
    }
    else
    {
        // only add album global ID to list which is already inserted in map
        (*it).removeAll(album->globalID());
        (*it).push_back(album->globalID());
    }
}

void AlbumThumbnailLoader::setThumbnailSize(int size)
{
    if (d->iconSize == size)
    {
        return;
    }

    d->iconSize = size;

    // clear task list
    d->pathAlbumMap.clear();
    // clear cached thumbnails
    d->thumbnailMap.clear();

    if (d->iconAlbumThumbThread)
    {
        d->iconAlbumThumbThread->stopLoading();
        d->iconAlbumThumbThread->setThumbnailSize(size);
    }

    if (d->iconTagThumbThread)
    {
        d->iconTagThumbThread->stopLoading();
        d->iconTagThumbThread->setThumbnailSize(size);
    }

    emit signalReloadThumbnails();
}

int AlbumThumbnailLoader::thumbnailSize() const
{
    return d->iconSize;
}

void AlbumThumbnailLoader::slotGotThumbnailFromIcon(const LoadingDescription& loadingDescription, const QPixmap& thumbnail)
{
    // We need to find all albums for which the given url has been requested,
    // and emit a signal for each album.

    PathAlbumMap::iterator it = d->pathAlbumMap.find(loadingDescription.filePath);

    if (it != d->pathAlbumMap.end())
    {
        AlbumManager* const manager = AlbumManager::instance();

        if (thumbnail.isNull())
        {
            // Loading failed
            for (QList<int>::const_iterator vit = (*it).constBegin(); vit != (*it).constEnd(); ++vit)
            {
                Album* const album = manager->findAlbum(*vit);

                if (album)
                {
                    emit signalFailed(album);
                }
            }
        }
        else
        {
            // Loading succeeded

            for (QList<int>::const_iterator vit = (*it).constBegin(); vit != (*it).constEnd(); ++vit)
            {
                // look up with global id
                Album* const album = manager->findAlbum(*vit);

                if (album)
                {
                    d->thumbnailMap.insert(album->globalID(), thumbnail);
                    emit signalThumbnail(album, thumbnail);
                }
            }
        }

        d->pathAlbumMap.erase(it);
    }
}

void AlbumThumbnailLoader::slotDispatchThumbnailInternal(int albumID, const QPixmap& thumbnail)
{
    // for cached thumbnails

    AlbumManager* const manager = AlbumManager::instance();
    Album* const album          = manager->findAlbum(albumID);

    if (album)
    {
        if (thumbnail.isNull())
        {
            emit signalFailed(album);
        }
        else
        {
            emit signalThumbnail(album, thumbnail);
        }
    }
}

void AlbumThumbnailLoader::slotIconChanged(Album* album)
{
    if (!album || (album->type() != Album::TAG && album->type() != Album::PHYSICAL))
    {
        return;
    }

    d->thumbnailMap.remove(album->globalID());
}

} // namespace Digikam
