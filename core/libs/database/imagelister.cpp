/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2007-03-20
 * Description : Listing information from database.
 *
 * Copyright (C) 2005      by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2007-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2007-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "imagelister.h"

// C++ includes

#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cerrno>
#include <limits>

// Qt includes

#include <QFile>
#include <QFileInfo>
#include <QDataStream>
#include <QRegExp>
#include <QDir>

// KDE includes

#include <kglobal.h>
#include <klocale.h>
#include <kcomponentdata.h>
#include <kmimetype.h>
#include <kdebug.h>

// Local includes

#include "albumdb.h"
#include "databaseaccess.h"
#include "databasebackend.h"
#include "collectionmanager.h"
#include "collectionlocation.h"
#include "imagequerybuilder.h"
#include "dmetadata.h"
#include "haariface.h"
#include "sqlquery.h"
#include "tagscache.h"
#include "imagetagpair.h"

namespace Digikam
{

/**
 * Used by QSet
 */
uint qHash(const ImageListerRecord& key)
{
    return key.imageID;
}
/*
 * The binary field for file size is only 32 bit.
 * If the value fits, we pass it. If it does not, we pass -1,
 * and the receiver shall get the full number itself
 */
static inline int toInt32BitSafe(const QList<QVariant>::const_iterator& it)
{
    qlonglong v = (*it).toLongLong();

    if (v > std::numeric_limits<int>::max() || v < 0)
    {
        return -1;
    }

    return (int)v;
}

// ---------------------------------------------------------------------------------

class ImageLister::Private
{

public:

    Private()
    {
        recursive               = true;
        listOnlyAvailableImages = true;
        allowExtraValues        = false;
    }

    bool recursive;
    bool listOnlyAvailableImages;
    bool allowExtraValues;
};

ImageLister::ImageLister()
    : d(new Private)
{
}

ImageLister::~ImageLister()
{
    delete d;
}

void ImageLister::setRecursive(bool recursive)
{
    d->recursive = recursive;
}

void ImageLister::setListOnlyAvailable(bool listOnlyAvailable)
{
    d->listOnlyAvailableImages = listOnlyAvailable;
}

void ImageLister::setAllowExtraValues(bool useExtraValue)
{
    d->allowExtraValues = useExtraValue;
}

KIO::TransferJob* ImageLister::startListJob(const DatabaseUrl& url, int extraValue)
{
    QByteArray ba;
    QDataStream ds(&ba, QIODevice::WriteOnly);
    ds << url;

    kDebug() << "Query Kioslave Url:" << url.prettyUrl();
    
    if (extraValue != -1)
    {
        ds << extraValue;
    }

    return (new KIO::SpecialJob(url, ba));
}

void ImageLister::list(ImageListerReceiver* const receiver, const DatabaseUrl& url)
{
    if (url.isAlbumUrl())
    {
        int albumRootId = url.albumRootId();
        QString album   = url.album();
        listAlbum(receiver, albumRootId, album);
    }
    else if (url.isTagUrl())
    {
        listTag(receiver, url.tagIds());
    }
    else if (url.isDateUrl())
    {
        listDateRange(receiver, url.startDate(), url.endDate());
    }
    else if (url.isMapImagesUrl())
    {
        double lat1, lat2, lon1, lon2;
        url.areaCoordinates(&lat1, &lat2, &lon1, &lon2);
        listAreaRange(receiver, lat1, lat2, lon1, lon2);
    }
}

void ImageLister::listAlbum(ImageListerReceiver* const receiver, int albumRootId, const QString& album)
{
    if (d->listOnlyAvailableImages)
    {
        if (!CollectionManager::instance()->locationForAlbumRootId(albumRootId).isAvailable())
        {
            return;
        }
    }

    QList<QVariant> albumIds;

    if (d->recursive)
    {
        QList<int> intAlbumIds = DatabaseAccess().db()->getAlbumAndSubalbumsForPath(albumRootId, album);

        if (intAlbumIds.isEmpty())
        {
            return;
        }

        foreach(int id, intAlbumIds)
        {
            albumIds << id;
        }
    }
    else
    {
        int albumId = DatabaseAccess().db()->getAlbumForPath(albumRootId, album, false);

        if (albumId == -1)
        {
            return;
        }

        albumIds << albumId;
    }

    QList<QVariant> values;

    QString query = "SELECT DISTINCT Images.id, Images.name, Images.album, "
                    "       ImageInformation.rating, Images.category, "
                    "       ImageInformation.format, ImageInformation.creationDate, "
                    "       Images.modificationDate, Images.fileSize, "
                    "       ImageInformation.width, ImageInformation.height "
                    " FROM Images "
                    "       INNER JOIN ImageInformation ON Images.id=ImageInformation.imageid "
                    " WHERE Images.status=1 AND ";

    if (d->recursive)
    {
        // SQLite allows no more than 999 parameters
        const int maxParams = DatabaseAccess().backend()->maximumBoundValues();
        for (int i=0; i<albumIds.size(); i++)
        {
            QString q           = query;
            QList<QVariant> ids =  (albumIds.size() <= maxParams) ? albumIds : albumIds.mid(i, maxParams);
            i                   += ids.count();

            QList<QVariant> v;
            DatabaseAccess  access;
            q += "Images.album IN (";
            access.db()->addBoundValuePlaceholders(q, ids.size());
            q += ");";
            access.backend()->execSql(q, ids, &v);

            values += v;
        }
    }
    else
    {
        DatabaseAccess access;
        query += "Images.album = ?;";
        access.backend()->execSql(query, albumIds, &values);
    }

    int width, height;

    for (QList<QVariant>::const_iterator it = values.constBegin(); it != values.constEnd();)
    {
        ImageListerRecord record;
        record.imageID           = (*it).toLongLong();
        ++it;
        record.name              = (*it).toString();
        ++it;
        record.albumID           = (*it).toInt();
        ++it;
        record.rating            = (*it).toInt();
        ++it;
        record.category          = (DatabaseItem::Category)(*it).toInt();
        ++it;
        record.format            = (*it).toString();
        ++it;
        record.creationDate      = (*it).isNull() ? QDateTime()
                                   : QDateTime::fromString((*it).toString(), Qt::ISODate);
        ++it;
        record.modificationDate  = (*it).isNull() ? QDateTime()
                                   : QDateTime::fromString((*it).toString(), Qt::ISODate);
        ++it;
        record.fileSize          = toInt32BitSafe(it);
        ++it;
        width                    = (*it).toInt();
        ++it;
        height                   = (*it).toInt();
        ++it;

        record.imageSize         = QSize(width, height);

        record.albumRootID = albumRootId;

        receiver->receive(record);
    }
}

void ImageLister::listTag(ImageListerReceiver* const receiver, QList<int> tagIds)
{
    QSet<ImageListerRecord> records;
    QList<int>::iterator it;
    for(it = tagIds.begin(); it != tagIds.end(); ++it)
    {
        QList<QVariant>         values;
        QMap<QString, QVariant> parameters;
        parameters.insert(":tagPID", *it);
        parameters.insert(":tagID",  *it);

        DatabaseAccess access;

        if (d->recursive)
        {
            access.backend()->execDBAction(access.backend()->getDBAction(QString("listTagRecursive")), parameters, &values);
        }
        else
        {
            access.backend()->execDBAction(access.backend()->getDBAction(QString("listTag")), parameters, &values);
        }

        QSet<int> albumRoots = albumRootsToList();

        int width, height;

        for (QList<QVariant>::const_iterator it = values.constBegin(); it != values.constEnd();)
        {
            ImageListerRecord record;

            record.imageID           = (*it).toLongLong();
            ++it;
            record.name              = (*it).toString();
            ++it;
            record.albumID           = (*it).toInt();
            ++it;
            record.albumRootID       = (*it).toInt();
            ++it;
            record.rating            = (*it).toInt();
            ++it;
            record.category          = (DatabaseItem::Category)(*it).toInt();
            ++it;
            record.format            = (*it).toString();
            ++it;
            record.creationDate      = (*it).isNull() ? QDateTime()
                                    : QDateTime::fromString((*it).toString(), Qt::ISODate);
            ++it;
            record.modificationDate  = (*it).isNull() ? QDateTime()
                                    : QDateTime::fromString((*it).toString(), Qt::ISODate);
            ++it;
            record.fileSize          = toInt32BitSafe(it);
            ++it;
            width                    = (*it).toInt();
            ++it;
            height                   = (*it).toInt();
            ++it;

            if (d->listOnlyAvailableImages && !albumRoots.contains(record.albumRootID))
            {
                continue;
            }

            record.imageSize         = QSize(width, height);

            records.insert(record);
        }
    }

    for(QSet<ImageListerRecord>::iterator it = records.begin(); it != records.end(); ++it)
    {
        receiver->receive(*it);
    }

}

void ImageLister::listFaces(ImageListerReceiver* const receiver, int personId)
{
    QList<qlonglong> list;
    QList<QVariant>  values;
    DatabaseAccess   access;

    access.backend()->execSql(QString("SELECT Images.id "
                                      " FROM Images "
                                      "       INNER JOIN ImageInformation ON Images.id=ImageInformation.imageid "
                                      "       INNER JOIN Albums ON Albums.id="+
                                      QString::number(personId)+
                                      " WHERE Images.status=1 "
                                      " ORDER BY Albums.id;"),
                              &values);

    QListIterator<QVariant> it(values);

    while (it.hasNext())
    {
        TagsCache* cache;
        cache = TagsCache::instance();

        ImageTagPair pair(list.last(), cache->tagForPath("/People/Unknown"));
        QList<QString> nameList = pair.values("face");

        // push the image into the list every time a face with the name is found in the image
        int count = nameList.count(cache->tagName(personId));

        for (int i = 0; i < count; ++i)
        {
            list += it.next().toLongLong();
        }
    }

    listFromIdList(receiver, list);
}

void ImageLister::listDateRange(ImageListerReceiver* const receiver, const QDate& startDate, const QDate& endDate)
{
    QList<QVariant> values;

    {
        DatabaseAccess access;
        access.backend()->execSql(QString("SELECT DISTINCT Images.id, Images.name, Images.album, "
                                          "       Albums.albumRoot, "
                                          "       ImageInformation.rating, Images.category, "
                                          "       ImageInformation.format, ImageInformation.creationDate, "
                                          "       Images.modificationDate, Images.fileSize, "
                                          "       ImageInformation.width, ImageInformation.height "
                                          " FROM Images "
                                          "       INNER JOIN ImageInformation ON Images.id=ImageInformation.imageid "
                                          "       INNER JOIN Albums ON Albums.id=Images.album "
                                          " WHERE Images.status=1 "
                                          "   AND ImageInformation.creationDate < ? "
                                          "   AND ImageInformation.creationDate >= ? "
                                          " ORDER BY Albums.id;"),
                                  QDateTime(endDate).toString(Qt::ISODate),
                                  QDateTime(startDate).toString(Qt::ISODate),
                                  &values);
    }

    QSet<int> albumRoots = albumRootsToList();
    int       width, height;

    for (QList<QVariant>::const_iterator it = values.constBegin(); it != values.constEnd();)
    {
        ImageListerRecord record;

        record.imageID           = (*it).toLongLong();
        ++it;
        record.name              = (*it).toString();
        ++it;
        record.albumID           = (*it).toInt();
        ++it;
        record.albumRootID       = (*it).toInt();
        ++it;
        record.rating            = (*it).toInt();
        ++it;
        record.category          = (DatabaseItem::Category)(*it).toInt();
        ++it;
        record.format            = (*it).toString();
        ++it;
        record.creationDate      = (*it).isNull() ? QDateTime()
                                   : QDateTime::fromString((*it).toString(), Qt::ISODate);
        ++it;
        record.modificationDate  = (*it).isNull() ? QDateTime()
                                   : QDateTime::fromString((*it).toString(), Qt::ISODate);
        ++it;
        record.fileSize          = toInt32BitSafe(it);
        ++it;
        width                    = (*it).toInt();
        ++it;
        height                   = (*it).toInt();
        ++it;

        if (d->listOnlyAvailableImages && !albumRoots.contains(record.albumRootID))
        {
            continue;
        }

        record.imageSize         = QSize(width, height);

        receiver->receive(record);
    }
}

void ImageLister::listAreaRange(ImageListerReceiver* const receiver, double lat1, double lat2, double lon1, double lon2)
{
    QList<QVariant> values;
    QList<QVariant> boundValues;
    boundValues << lat1 << lat2 << lon1 << lon2;

    kDebug() << "Listing area" << lat1 << lat2 << lon1 << lon2;

    DatabaseAccess access;

    access.backend()->execSql(QString("SELECT DISTINCT Images.id, "
                                      "       Albums.albumRoot, ImageInformation.rating, ImageInformation.creationDate, "
                                      "       ImagePositions.latitudeNumber, ImagePositions.longitudeNumber "
                                      " FROM Images "
                                      "       INNER JOIN ImageInformation ON Images.id=ImageInformation.imageid "
                                      "       INNER JOIN Albums ON Albums.id=Images.album "
                                      "       INNER JOIN ImagePositions   ON Images.id=ImagePositions.imageid "
                                      " WHERE Images.status=1 "
                                      "   AND (ImagePositions.latitudeNumber>? AND ImagePositions.latitudeNumber<?) "
                                      "   AND (ImagePositions.longitudeNumber>? AND ImagePositions.longitudeNumber<?);"),
                              boundValues,
                              &values);


    kDebug() << "Results:" << values.size() / 14;

    QSet<int> albumRoots = albumRootsToList();
    double    lat, lon;

    for (QList<QVariant>::const_iterator it = values.constBegin(); it != values.constEnd();)
    {
        ImageListerRecord record(d->allowExtraValues ? ImageListerRecord::ExtraValueFormat : ImageListerRecord::TraditionalFormat);

        record.imageID           = (*it).toLongLong();
        ++it;
        record.albumRootID       = (*it).toInt();
        ++it;

        record.rating            = (*it).toInt();
        ++it;
        record.creationDate      = (*it).toDateTime();
        ++it;
        lat                      = (*it).toDouble();
        ++it;
        lon                      = (*it).toDouble();
        ++it;

        if (d->listOnlyAvailableImages && !albumRoots.contains(record.albumRootID))
        {
            continue;
        }

        record.extraValues       << lat << lon;

        receiver->receive(record);
    }
}

void ImageLister::listSearch(ImageListerReceiver* const receiver, const QString& xml, int limit)
{
    if (xml.isEmpty())
    {
        return;
    }

    QList<QVariant> boundValues;
    QList<QVariant> values;
    QString sqlQuery;
    QString errMsg;

    // query head
    sqlQuery = "SELECT DISTINCT Images.id, Images.name, Images.album, "
               "       Albums.albumRoot, "
               "       ImageInformation.rating, Images.category, "
               "       ImageInformation.format, ImageInformation.creationDate, "
               "       Images.modificationDate, Images.fileSize, "
               "       ImageInformation.width, ImageInformation.height, "
               "       ImagePositions.latitudeNumber, ImagePositions.longitudeNumber "
               " FROM Images "
               "       INNER JOIN ImageInformation ON Images.id=ImageInformation.imageid "
               "       LEFT  JOIN ImageMetadata    ON Images.id=ImageMetadata.imageid "
               "       LEFT  JOIN VideoMetadata    ON Images.id=VideoMetadata.imageid "
               "       LEFT  JOIN ImagePositions   ON Images.id=ImagePositions.imageid "
               "       INNER JOIN Albums           ON Albums.id=Images.album "
               "WHERE Images.status=1 AND ( ";

    // query body
    ImageQueryBuilder   builder;
    ImageQueryPostHooks hooks;

    sqlQuery += builder.buildQuery(xml, &boundValues, &hooks);

    if (limit > 0)
    {
        sqlQuery += QString(" ) LIMIT %1; ").arg(limit);
    }
    else
    {
        sqlQuery += " );";
    }

    kDebug() << "Search query:\n" << sqlQuery << "\n" << boundValues;

    bool executionSuccess;
    {
        DatabaseAccess access;
        executionSuccess = access.backend()->execSql(sqlQuery, boundValues, &values);

        if (!executionSuccess)
        {
            errMsg = access.backend()->lastError();
        }
    }

    if (!executionSuccess)
    {
        receiver->error(errMsg);
        return;
    }

    kDebug() << "Search result:" << values.size();

    QSet<int> albumRoots = albumRootsToList();
    int       width, height;
    double    lat,lon;

    for (QList<QVariant>::const_iterator it = values.constBegin(); it != values.constEnd();)
    {
        ImageListerRecord record;

        record.imageID           = (*it).toLongLong();
        ++it;
        record.name              = (*it).toString();
        ++it;
        record.albumID           = (*it).toInt();
        ++it;
        record.albumRootID       = (*it).toInt();
        ++it;
        record.rating            = (*it).toInt();
        ++it;
        record.category          = (DatabaseItem::Category)(*it).toInt();
        ++it;
        record.format            = (*it).toString();
        ++it;
        record.creationDate      = (*it).isNull() ? QDateTime()
                                   : QDateTime::fromString((*it).toString(), Qt::ISODate);
        ++it;
        record.modificationDate  = (*it).isNull() ? QDateTime()
                                   : QDateTime::fromString((*it).toString(), Qt::ISODate);
        ++it;
        record.fileSize          = toInt32BitSafe(it);
        ++it;
        width                    = (*it).toInt();
        ++it;
        height                   = (*it).toInt();
        ++it;
        lat                      = (*it).toDouble();
        ++it;
        lon                      = (*it).toDouble();
        ++it;

        if (d->listOnlyAvailableImages && !albumRoots.contains(record.albumRootID))
        {
            continue;
        }

        if (!hooks.checkPosition(lat, lon))
        {
            continue;
        }

        record.imageSize         = QSize(width, height);

        receiver->receive(record);
    }
}

void ImageLister::listImageTagPropertySearch(ImageListerReceiver* const receiver, const QString& xml)
{
    if (xml.isEmpty())
    {
        return;
    }

    QList<QVariant> boundValues;
    QList<QVariant> values;
    QString sqlQuery;
    QString errMsg;

    // Currently, for optimization, this does not allow a general-purpose search,
    // ImageMetadata and ImagePositions are not joined and hooks are ignored.

    // query head
    sqlQuery = "SELECT DISTINCT Images.id, Images.name, Images.album, "
               "       Albums.albumRoot, "
               "       ImageInformation.rating, Images.category, "
               "       ImageInformation.format, ImageInformation.creationDate, "
               "       Images.modificationDate, Images.fileSize, "
               "       ImageInformation.width,  ImageInformation.height, "
               "       ImageTagProperties.value, ImageTagProperties.property, ImageTagProperties.tagid "
               " FROM Images "
               "       INNER JOIN ImageTagProperties ON ImageTagProperties.imageid=Images.id "
               "       INNER JOIN ImageInformation ON Images.id=ImageInformation.imageid "
               "       INNER JOIN Albums           ON Albums.id=Images.album "
               "WHERE Images.status=1 AND ( ";

    // query body
    ImageQueryBuilder builder;
    ImageQueryPostHooks hooks;
    builder.setImageTagPropertiesJoined(true); // ImageTagProperties added by INNER JOIN
    sqlQuery += builder.buildQuery(xml, &boundValues, &hooks);
    sqlQuery += " );";

    kDebug() << "Search query:\n" << sqlQuery << "\n" << boundValues;

    bool executionSuccess;
    {
        DatabaseAccess access;
        executionSuccess = access.backend()->execSql(sqlQuery, boundValues, &values);

        if (!executionSuccess)
        {
            errMsg = access.backend()->lastError();
        }
    }

    if (!executionSuccess)
    {
        receiver->error(errMsg);
        return;
    }

    kDebug() << "Search result:" << values.size();

    QSet<int> albumRoots = albumRootsToList();

    int width, height;

    for (QList<QVariant>::const_iterator it = values.constBegin(); it != values.constEnd();)
    {
        ImageListerRecord record(d->allowExtraValues ? ImageListerRecord::ExtraValueFormat : ImageListerRecord::TraditionalFormat);

        record.imageID           = (*it).toLongLong();
        ++it;
        record.name              = (*it).toString();
        ++it;
        record.albumID           = (*it).toInt();
        ++it;
        record.albumRootID       = (*it).toInt();
        ++it;
        record.rating            = (*it).toInt();
        ++it;
        record.category          = (DatabaseItem::Category)(*it).toInt();
        ++it;
        record.format            = (*it).toString();
        ++it;
        record.creationDate      = (*it).isNull() ? QDateTime()
                                   : QDateTime::fromString((*it).toString(), Qt::ISODate);
        ++it;
        record.modificationDate  = (*it).isNull() ? QDateTime()
                                   : QDateTime::fromString((*it).toString(), Qt::ISODate);
        ++it;
        record.fileSize          = toInt32BitSafe(it);
        ++it;
        width                    = (*it).toInt();
        ++it;
        height                   = (*it).toInt();
        ++it;
        // sync the following order with the places where it's read, e.g., DatabaseFace
        record.extraValues      << (*it); // value
        ++it;
        record.extraValues      << (*it); // property
        ++it;
        record.extraValues      << (*it); // tag id
        ++it;

        if (d->listOnlyAvailableImages && !albumRoots.contains(record.albumRootID))
        {
            continue;
        }

        record.imageSize         = QSize(width, height);

        receiver->receive(record);
    }
}

void ImageLister::listHaarSearch(ImageListerReceiver* const receiver, const QString& xml)
{
    SearchXmlReader reader(xml);
    reader.readToFirstField();

    if (reader.fieldName() != "similarity")
    {
        receiver->error("Unsupported field name \"" + reader.fieldName() + "\" in Haar search");
        return;
    }

    QStringRef type             = reader.attributes().value("type");
    QStringRef numResultsString = reader.attributes().value("numberofresults");
    QStringRef thresholdString  = reader.attributes().value("threshold");
    QStringRef sketchTypeString = reader.attributes().value("sketchtype");

    double threshold                 = 0.9;
    int numberOfResults              = 20;
    HaarIface::SketchType sketchType = HaarIface::ScannedSketch;

    if (!numResultsString.isNull())
    {
        numberOfResults = qMax(numResultsString.toString().toInt(), 1);
    }

    if (!thresholdString.isNull())
    {
        threshold = qMax(thresholdString.toString().toDouble(), 0.1);
    }

    if (sketchTypeString == "handdrawn")
    {
        sketchType = HaarIface::HanddrawnSketch;
    }
    else
    {
        sketchType = HaarIface::ScannedSketch;
    }

    QList<qlonglong> list;

    if (type == "signature")
    {
        QString sig = reader.value();
        HaarIface iface;

        if (d->listOnlyAvailableImages)
        {
            iface.setAlbumRootsToSearch(albumRootsToList());
        }

        list = iface.bestMatchesForSignature(sig, numberOfResults, sketchType);
    }
    else if (type == "imageid")
    {
        qlonglong id = reader.valueToLongLong();
        HaarIface iface;

        if (d->listOnlyAvailableImages)
        {
            iface.setAlbumRootsToSearch(albumRootsToList());
        }

        list = iface.bestMatchesForImageWithThreshold(id, threshold, sketchType);
    }

    listFromIdList(receiver, list);
}

void ImageLister::listFromIdList(ImageListerReceiver* const receiver, const QList<qlonglong>& imageIds)
{
    QList<QVariant> values;
    QString         errMsg;
    bool            executionSuccess = true;

    {
        /*
        // Unfortunately, we need to convert to QVariant
        QList<QVariant> variantIdList;
        foreach(const qlonglong& id, imageIds)
        {
            variantIdList << id;
        }

        DatabaseAccess access;
        QSqlQuery query = access.backend()->prepareQuery(QString(
                    "SELECT DISTINCT Images.id, Images.name, Images.album, "
                    "       ImageInformation.rating, ImageInformation.creationDate, "
                    "       Images.modificationDate, Images.fileSize, "
                    "       ImageInformation.width, ImageInformation.height "
                    " FROM Images "
                    "       LEFT JOIN ImageInformation ON Images.id=ImageInformation.imageid "
                    " WHERE Images.id = ?;"
                                                                ));

        query.addBindValue(variantIdList);
        executionSuccess = query.execBatch
        */
        DatabaseAccess access;
        SqlQuery query = access.backend()->prepareQuery(QString(
                             "SELECT DISTINCT Images.id, Images.name, Images.album, "
                             "       Albums.albumRoot, "
                             "       ImageInformation.rating, Images.category, "
                             "       ImageInformation.format, ImageInformation.creationDate, "
                             "       Images.modificationDate, Images.fileSize, "
                             "       ImageInformation.width, ImageInformation.height "
                             " FROM Images "
                             "       LEFT JOIN ImageInformation ON Images.id=ImageInformation.imageid "
                             "       LEFT JOIN Albums ON Albums.id=Images.album "
                             " WHERE Images.status=1 AND Images.id = ?;"
                         ));

        foreach(const qlonglong& id, imageIds)
        {
            query.bindValue(0, id);
            executionSuccess = access.backend()->exec(query);

            if (!executionSuccess)
            {
                errMsg = access.backend()->lastError();
                break;
            }

            // append results to list
            values << access.backend()->readToList(query);
        }

    }

    if (!executionSuccess)
    {
        receiver->error(errMsg);
        return;
    }

    int width, height;

    for (QList<QVariant>::const_iterator it = values.constBegin(); it != values.constEnd();)
    {
        ImageListerRecord record;

        record.imageID           = (*it).toLongLong();
        ++it;
        record.name              = (*it).toString();
        ++it;
        record.albumID           = (*it).toInt();
        ++it;
        record.albumRootID       = (*it).toInt();
        ++it;
        record.rating            = (*it).toInt();
        ++it;
        record.category          = (DatabaseItem::Category)(*it).toInt();
        ++it;
        record.format            = (*it).toString();
        ++it;
        record.creationDate      = (*it).isNull() ? QDateTime()
                                   : QDateTime::fromString((*it).toString(), Qt::ISODate);
        ++it;
        record.modificationDate  = (*it).isNull() ? QDateTime()
                                   : QDateTime::fromString((*it).toString(), Qt::ISODate);
        ++it;
        record.fileSize          = toInt32BitSafe(it);
        ++it;
        width                    = (*it).toInt();
        ++it;
        height                   = (*it).toInt();
        ++it;

        record.imageSize         = QSize(width, height);

        receiver->receive(record);
    }
}

QSet<int> ImageLister::albumRootsToList() const
{
    if (!d->listOnlyAvailableImages)
    {
        return QSet<int>();    // invalid value, all album roots shall be listed
    }

    QList<CollectionLocation> locations = CollectionManager::instance()->allAvailableLocations();
    QSet<int>                 ids;

    foreach(const CollectionLocation& location, locations)
    {
        ids << location.id();
    }

    return ids;
}

QString ImageLister::tagSearchXml(const DatabaseUrl& url, const QString& type, bool includeChildTags) const
{
    int tagId = url.tagId();

    if (type == "faces")
    {
        SearchXmlWriter writer;

        writer.writeGroup();
        writer.setDefaultFieldOperator(SearchXml::Or);

        QStringList properties;
        properties << "autodetectedFace";
        properties << "tagRegion";

        foreach(const QString& property, properties)
        {
            writer.writeField("imagetagproperty", includeChildTags ? SearchXml::InTree : SearchXml::Equal);

            if (tagId != -1)
            {
                writer.writeAttribute("tagid", QString::number(tagId));
            }

            writer.writeValue(property);
            writer.finishField();
        }


        /*
        if (flags & TagAssigned && tagId)
        {
            writer.writeField("tagid", SearchXml::Equal);
            writer.writeValue(tagId);
            writer.finishField();
        }
        */

        writer.finishGroup();

        return writer.xml();
    }
    else
    {
        return QString();
    }
}

}  // namespace Digikam
