/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2003-01-17
 * Description : Haar Database interface
 *
 * Copyright (C) 2003      by Ricardo Niederberger Cabral <nieder at mail dot ru>
 * Copyright (C) 2009-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2009-2011 by Andi Clemens <andi dot clemens at gmail dot com>
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

#include "haariface.h"

// C++ includes

#include <fstream>
#include <cmath>
#include <cstring>

// Qt includes

#include <QByteArray>
#include <QDataStream>
#include <QImage>
#include <QImageReader>
#include <QMap>

// KDE includes

#include <kurl.h>
#include <kdebug.h>

// Local includes

#include "jpegutils.h"
#include "dimg.h"
#include "imageinfo.h"
#include "databaseaccess.h"
#include "databasetransaction.h"
#include "albumdb.h"
#include "databasebackend.h"
#include "searchxml.h"
#include "sqlquery.h"

using namespace std;

namespace Digikam
{

typedef QMap<qlonglong, Haar::SignatureData> SignatureCache;

/** This class encapsulates the Haar signature in a QByteArray
 *  that can be stored as a BLOB in the database.
 *
 *  Reading and writing is done in a platform-independent manner, which
 *  induces a certain overhead, but which is necessary IMO.
 */
class DatabaseBlob
{
public:

    enum
    {
        Version = 1
    };

public:

    DatabaseBlob()
    {
    }

    /** Read the QByteArray into the Haar::SignatureData.
     */
    void read(const QByteArray& array, Haar::SignatureData* const data)
    {
        QDataStream stream(array);

        // check version
        qint32 version;
        stream >> version;

        if (version != Version)
        {
            kError() << "Unsupported binary version of Haar Blob in database";
            return;
        }

        stream.setVersion(QDataStream::Qt_4_3);

        // read averages
        for (int i=0; i<3; ++i)
        {
            stream >> data->avg[i];
        }

        // read coefficients
        for (int i=0; i<3; ++i)
        {
            for (int j=0; j<Haar::NumberOfCoefficients; ++j)
            {
                stream >> data->sig[i][j];
            }
        }
    }

    QByteArray write(Haar::SignatureData* const data)
    {
        QByteArray array;
        array.reserve(sizeof(qint32) + 3*sizeof(double) + 3*sizeof(qint32)*Haar::NumberOfCoefficients);
        QDataStream stream(&array, QIODevice::WriteOnly);
        stream.setVersion(QDataStream::Qt_4_3);

        // write version
        stream << (qint32)Version;

        // write averages
        for (int i=0; i<3; ++i)
        {
            stream << data->avg[i];
        }

        // write coefficients
        for (int i=0; i<3; ++i)
        {
            for (int j=0; j<Haar::NumberOfCoefficients; ++j)
            {
                stream << data->sig[i][j];
            }
        }

        return array;
    }
};

// -----------------------------------------------------------------------------------------------------

class HaarIface::Private
{
public:

    Private()
    {
        data                       = 0;
        bin                        = 0;
        signatureCache             = 0;
        useSignatureCache          = false;

        signatureQuery             = QString("SELECT M.imageid, 0, M.matrix "
                                             " FROM ImageHaarMatrix AS M "
                                             "    INNER JOIN Images ON Images.id=M.imageid "
                                             " WHERE Images.status=1; ");

        signatureByAlbumRootsQuery = QString("SELECT M.imageid, Albums.albumRoot, M.matrix "
                                             " FROM ImageHaarMatrix AS M "
                                             "    INNER JOIN Images ON Images.id=M.imageid "
                                             "    INNER JOIN Albums ON Albums.id=Images.album"
                                             " WHERE Images.status=1;");
    }

    ~Private()
    {
        delete data;
        delete bin;
        delete signatureCache;
    }

    void createLoadingBuffer()
    {
        if (!data)
        {
            data = new Haar::ImageData;
        }
    }

    void createWeightBin()
    {
        if (!bin)
        {
            bin = new Haar::WeightBin;
        }
    }

    void setSignatureCacheEnabled(bool cache, const QSet<qlonglong>& imageIds)
    {
        setSignatureCacheEnabled(cache);

        // stop here if we disable cached signatures
        if (!cache || imageIds.isEmpty())
        {
            return;
        }

        // Remove all ids from the fully created signatureCache that are not needed for the duplicates search.
        // This is usually faster then starting a query for every single id in imageIds.
        for (SignatureCache::iterator it = signatureCache->begin();
             it != signatureCache->end(); )
        {
            if (!imageIds.contains(it.key()))
            {
                it = signatureCache->erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    void setSignatureCacheEnabled(bool cache)
    {
        delete signatureCache;
        signatureCache = 0;

        if (cache)
        {
            signatureCache = new SignatureCache();
        }
        useSignatureCache = cache;

        // stop here if we disable cached signatures
        if (!cache)
        {
            return;
        }

        // Variables for data read from DB
        DatabaseAccess      access;
        DatabaseBlob        blob;
        qlonglong           imageid;
        Haar::SignatureData targetSig;

        // reference for easier access
        SignatureCache& signatureCache = *this->signatureCache;

        SqlQuery query = access.backend()->prepareQuery(signatureQuery);

        if (!access.backend()->exec(query))
        {
            return;
        }

        while (query.next())
        {
            imageid = query.value(0).toLongLong();
            blob.read(query.value(2).toByteArray(), &targetSig);
            signatureCache[imageid] = targetSig;
        }
    }

    bool             useSignatureCache;
    Haar::ImageData* data;
    Haar::WeightBin* bin;
    SignatureCache*  signatureCache;
    QString          signatureQuery;
    QString          signatureByAlbumRootsQuery;
    QSet<int>        albumRootsToSearch;
};

HaarIface::HaarIface()
    : d(new Private())
{
}

HaarIface::~HaarIface()
{
    delete d;
}

void HaarIface::setAlbumRootsToSearch(QList<int> albumRootIds)
{
    setAlbumRootsToSearch(albumRootIds.toSet());
}

void HaarIface::setAlbumRootsToSearch(QSet<int> albumRootIds)
{
    d->albumRootsToSearch = albumRootIds;
}

int HaarIface::preferredSize()
{
    return Haar::NumberOfPixels;
}

bool HaarIface::indexImage(const QString& filename)
{
    QImage image = loadQImage(filename);

    if (image.isNull())
    {
        return false;
    }

    return indexImage(filename, image);
}

bool HaarIface::indexImage(const QString& filename, const QImage& image)
{
    ImageInfo info = ImageInfo::fromLocalFile(filename);

    if (info.isNull())
    {
        return false;
    }

    return indexImage(info.id(), image);
}

bool HaarIface::indexImage(const QString& filename, const DImg& image)
{
    ImageInfo info = ImageInfo::fromLocalFile(filename);

    if (info.isNull())
    {
        return false;
    }

    return indexImage(info.id(), image);
}

bool HaarIface::indexImage(qlonglong imageid, const QImage& image)
{
    if (image.isNull())
    {
        return false;
    }

    d->createLoadingBuffer();
    d->data->fillPixelData(image);

    return indexImage(imageid);
}

bool HaarIface::indexImage(qlonglong imageid, const DImg& image)
{
    if (image.isNull())
    {
        return false;
    }

    d->createLoadingBuffer();
    d->data->fillPixelData(image);

    return indexImage(imageid);
}

// private method: d->data has been filled
bool HaarIface::indexImage(qlonglong imageid)
{
    Haar::Calculator haar;
    haar.transform(d->data);

    Haar::SignatureData sig;
    haar.calcHaar(d->data, &sig);

    DatabaseAccess access;

    // Store main entry
    {
        // prepare blob
        DatabaseBlob blob;
        QByteArray array = blob.write(&sig);

        access.backend()->execSql(QString("REPLACE INTO ImageHaarMatrix "
                                          " (imageid, modificationDate, uniqueHash, matrix) "
                                          " SELECT id, modificationDate, uniqueHash, ? "
                                          "  FROM Images WHERE id=?; "),
                                  array, imageid);
    }

    return true;
}

QString HaarIface::signatureAsText(const QImage& image)
{
    d->createLoadingBuffer();
    d->data->fillPixelData(image);

    Haar::Calculator haar;
    haar.transform(d->data);
    Haar::SignatureData sig;
    haar.calcHaar(d->data, &sig);

    DatabaseBlob blob;
    QByteArray array = blob.write(&sig);

    return array.toBase64();
}

QList<qlonglong> HaarIface::bestMatchesForImage(const QImage& image, int numberOfResults, SketchType type)
{
    d->createLoadingBuffer();
    d->data->fillPixelData(image);

    Haar::Calculator haar;
    haar.transform(d->data);
    Haar::SignatureData sig;
    haar.calcHaar(d->data, &sig);

    return bestMatches(&sig, numberOfResults, type);
}

QList<qlonglong> HaarIface::bestMatchesForImage(qlonglong imageid, int numberOfResults, SketchType type)
{
    Haar::SignatureData sig;

    if (!retrieveSignatureFromDB(imageid, &sig))
    {
        return QList<qlonglong>();
    }

    return bestMatches(&sig, numberOfResults, type);
}

QList<qlonglong> HaarIface::bestMatchesForImageWithThreshold(qlonglong imageid, double requiredPercentage,
                                                             SketchType type)
{
    if ( !d->useSignatureCache || (d->signatureCache->isEmpty() && d->useSignatureCache) )
    {
        Haar::SignatureData sig;

        if (!retrieveSignatureFromDB(imageid, &sig))
        {
            return QList<qlonglong>();
        }

        return bestMatchesWithThreshold(&sig, requiredPercentage, type);
    }
    else
    {
        // reference for easier access
        SignatureCache& signatureCache = *d->signatureCache;
        Haar::SignatureData& sig       = signatureCache[imageid];
        return bestMatchesWithThreshold(&sig, requiredPercentage, type);
    }
}

QList<qlonglong> HaarIface::bestMatchesForFile(const QString& filename, int numberOfResults, SketchType type)
{
    QImage image = loadQImage(filename);

    if (image.isNull())
    {
        return QList<qlonglong>();
    }

    return bestMatchesForImage(image, numberOfResults, type);
}

QList<qlonglong> HaarIface::bestMatchesForSignature(const QString& signature, int numberOfResults, SketchType type)
{
    QByteArray bytes = QByteArray::fromBase64(signature.toAscii());

    DatabaseBlob blobReader;
    Haar::SignatureData sig;
    blobReader.read(bytes, &sig);

    return bestMatches(&sig, numberOfResults, type);
}

QList<qlonglong> HaarIface::bestMatches(Haar::SignatureData* const querySig, int numberOfResults, SketchType type)
{
    QMap<qlonglong, double> scores = searchDatabase(querySig, type);

    // Find out the best matches, those with the lowest score
    // We make use of the feature that QMap keys are sorted in ascending order
    // Of course, images can have the same score, so we need a multi map
    QMultiMap<double, qlonglong> bestMatches;
    bool                         initialFill = false;
    double                       score, worstScore, bestScore;
    qlonglong                    id;

    for (QMap<qlonglong, double>::const_iterator it = scores.constBegin(); it != scores.constEnd(); ++it)
    {
        score = it.value();
        id    = it.key();

        if (!initialFill)
        {
            // as long as the maximum number of results is not reached, just fill up the map
            bestMatches.insert(score, id);
            initialFill = (bestMatches.size() >= numberOfResults);
        }
        else
        {
            // find the last entry, the one with the highest (=worst) score
            QMap<double, qlonglong>::iterator last = bestMatches.end();
            --last;
            worstScore = last.key();

            // if the new entry has a higher score, put it in the list and remove that last one
            if (score < worstScore)
            {
                bestMatches.erase(last);
                bestMatches.insert(score, id);
            }
            else if (score == worstScore)
            {
                bestScore = bestMatches.begin().key();

                // if the score is identical for all entries, increase the maximum result number
                if (score == bestScore)
                {
                    bestMatches.insert(score, id);
                }
            }
        }
    }

/*
    for (QMap<double, qlonglong>::iterator it = bestMatches.begin(); it != bestMatches.end(); ++it)
        kDebug() << it.key() << it.value();
*/

    return bestMatches.values();
}

QList<qlonglong> HaarIface::bestMatchesWithThreshold(Haar::SignatureData* const querySig, double requiredPercentage, SketchType type)
{
    QMap<qlonglong, double> scores = searchDatabase(querySig, type);
    double lowest, highest;
    getBestAndWorstPossibleScore(querySig, type, &lowest, &highest);

    double range         = highest - lowest;
    double requiredScore = lowest + range * (1.0 - requiredPercentage);

    QMultiMap<double, qlonglong> bestMatches;
    double score, percentage;
    qlonglong id;

    for (QMap<qlonglong, double>::const_iterator it = scores.constBegin(); it != scores.constEnd(); ++it)
    {
        score = it.value();
        id    = it.key();

        if (score <= requiredScore)
        {
            percentage = 1.0 - (score - lowest) / range;
            bestMatches.insert(percentage, id);
        }
    }

    // Debug output
    if (bestMatches.count() > 1)
    {
        kDebug() << "Duplicates with id and score:";

        for (QMultiMap<double, qlonglong>::const_iterator it = bestMatches.constBegin(); it != bestMatches.constEnd(); ++it)
        {
            kDebug() << it.value() << QString::number(it.key() * 100)+QChar('%');
        }
    }

    // We may want to return the map itself, or a list with pairs id - percentage
    return bestMatches.values();
}

/// This method is the core functionality: It assigns a score to every image in the db
QMap<qlonglong, double> HaarIface::searchDatabase(Haar::SignatureData* const querySig, SketchType type)
{
    d->createWeightBin();

    // The table of constant weight factors applied to each channel and bin
    Haar::Weights weights((Haar::Weights::SketchType)type);

    // layout the query signature for fast lookup
    Haar::SignatureMap queryMapY, queryMapI, queryMapQ;
    queryMapY.fill(querySig->sig[0]);
    queryMapI.fill(querySig->sig[1]);
    queryMapQ.fill(querySig->sig[2]);
    Haar::SignatureMap* queryMaps[3] = { &queryMapY, &queryMapI, &queryMapQ };

    // Map imageid -> score. Lowest score is best.
    // any newly inserted value will be initialized with a score of 0, as required
    QMap<qlonglong, double> scores;

    // Variables for data read from DB
    DatabaseAccess      access;
    DatabaseBlob        blob;
    qlonglong           imageid;
    Haar::SignatureData targetSig;

    // reference for easier access
    SignatureCache& signatureCache = *d->signatureCache;

    bool filterByAlbumRoots = !d->albumRootsToSearch.isEmpty();

    // if no cache is used or the cache signature map is empty, query the database
    if ( !d->useSignatureCache || (signatureCache.isEmpty() && d->useSignatureCache) )
    {
        QString queryText;

        if (filterByAlbumRoots)
        {
            queryText = d->signatureByAlbumRootsQuery;
        }
        else
        {
            queryText = d->signatureQuery;
        }

        SqlQuery query = access.backend()->prepareQuery(queryText);

        if (!access.backend()->exec(query))
        {
            return scores;
        }

        // We don't use DatabaseBackend's convenience calls, as the result set is large
        // and we try to avoid copying in a temporary QList<QVariant>
        int albumRootId = 0;

        while (query.next())
        {
            imageid = query.value(0).toLongLong();

            if (filterByAlbumRoots)
            {
                albumRootId = query.value(1).toInt();

                if (!d->albumRootsToSearch.contains(albumRootId))
                {
                    continue;
                }
            }

            blob.read(query.value(2).toByteArray(), &targetSig);

            if (d->useSignatureCache)
            {
                signatureCache[imageid] = targetSig;
            }

            double&              score = scores[imageid];
            Haar::SignatureData& qSig  = *querySig;
            Haar::SignatureData& tSig  = targetSig;

            score = calculateScore(qSig, tSig, weights, queryMaps);
        }
    }
    // read cached signature map if possible
    else
    {
        foreach(const qlonglong& imageid, signatureCache.keys())
        {
            double& score              = scores[imageid];
            Haar::SignatureData& qSig  = *querySig;
            Haar::SignatureData& tSig  = signatureCache[imageid];

            score = calculateScore(qSig, tSig, weights, queryMaps);
        }
    }

    return scores;
}

QImage HaarIface::loadQImage(const QString& filename)
{
    // NOTE: Can be optimized using DImg.

    QImage image;

    if (JPEGUtils::isJpegImage(filename))
    {
        // use fast jpeg loading
        if (!JPEGUtils::loadJPEGScaled(image, filename, Haar::NumberOfPixels))
        {
            // try QT now.
            if (!image.load(filename))
            {
                return QImage();
            }
        }
    }
    else
    {
        // use default QT image loading
        if (!image.load(filename))
        {
            return QImage();
        }
    }

    return image;
}

bool HaarIface::retrieveSignatureFromDB(qlonglong imageid, Haar::SignatureData* const sig)
{
    QList<QVariant> values;
    DatabaseAccess().backend()->execSql(QString("SELECT matrix FROM ImageHaarMatrix WHERE imageid=?"),
                                        imageid, &values);

    if (values.isEmpty())
    {
        return false;
    }

    DatabaseBlob blob;

    blob.read(values.first().toByteArray(), sig);
    return true;
}

void HaarIface::getBestAndWorstPossibleScore(Haar::SignatureData* const sig, SketchType type,
        double* const lowestAndBestScore, double* const highestAndWorstScore)
{
    Haar::Weights weights((Haar::Weights::SketchType)type);
    double score = 0;

    // In the first step, the score is initialized with the weighted color channel averages.
    // We don't know the target channel average here, we only now its not negative => assume 0
    for (int channel=0; channel<3; ++channel)
    {
        score += weights.weightForAverage(channel) * fabs( sig->avg[channel] /*- targetSig.avg[channel]*/ );
    }

    *highestAndWorstScore = score;

    // Next consideration: The lowest possible score is reached if the signature is identical.
    // The first step (see above) will result in 0 - skip it.
    // In the second step, for every coefficient in the sig that have query and target in common,
    // so in our case all 3*40, subtract the specifically assigned weighting.
    score = 0;

    for (int channel=0; channel<3; ++channel)
    {
        Haar::Idx* coefs = sig->sig[channel];

        for (int coef = 0; coef < Haar::NumberOfCoefficients; ++coef)
        {
            score -= weights.weight(d->bin->binAbs(coefs[coef]), channel);
        }
    }

    *lowestAndBestScore = score;
}

void HaarIface::rebuildDuplicatesAlbums(const QList<int>& albums2Scan, const QList<int>& tags2Scan,
                                        double requiredPercentage, HaarProgressObserver* const observer)
{
    // Carry out search. This takes long.
    QMap< qlonglong, QList<qlonglong> > results = findDuplicatesInAlbumsAndTags(albums2Scan,
            tags2Scan,
            requiredPercentage, observer);

    // Build search XML from the results. Store list of ids of similar images.
    QMap<QString, QString> queries;

    for (QMap< qlonglong, QList<qlonglong> >::const_iterator it = results.constBegin(); it != results.constEnd(); ++it)
    {
        SearchXmlWriter writer;
        writer.writeGroup();
        writer.writeField("imageid", SearchXml::OneOf);
        writer.writeValue(it.value());
        writer.finishField();
        writer.finishGroup();
        writer.finish();
        // Use the id of the first duplicate as name of the search
        queries.insert(QString::number(it.key()), writer.xml());
    }

    // Write search albums to database
    {
        DatabaseAccess access;
        DatabaseTransaction transaction(&access);

        // delete all old searches
        access.db()->deleteSearches(DatabaseSearch::DuplicatesSearch);

        // create new groups
        for (QMap<QString, QString>::const_iterator it = queries.constBegin(); it != queries.constEnd(); ++it)
        {
            access.db()->addSearch(DatabaseSearch::DuplicatesSearch, it.key(), it.value());
        }
    }
}

QMap< qlonglong, QList<qlonglong> > HaarIface::findDuplicatesInAlbums(const QList<int>& albums2Scan,
                                                                      double requiredPercentage,
                                                                      HaarProgressObserver* const observer)
{
    QSet<qlonglong> idList;

    // Get all items DB id from all albums and all collections
    foreach(int albumId, albums2Scan)
    {
        idList.unite(DatabaseAccess().db()->getItemIDsInAlbum(albumId).toSet());
    }

    return findDuplicates(idList, requiredPercentage, observer);
}

QMap< qlonglong, QList<qlonglong> > HaarIface::findDuplicatesInAlbumsAndTags(const QList<int>& albums2Scan,
                                                                             const QList<int>& tags2Scan,
                                                                             double requiredPercentage,
                                                                             HaarProgressObserver* const observer)
{
    QSet<qlonglong> idList;

    // Get all items DB id from all albums and all collections
    foreach(int albumId, albums2Scan)
    {
        idList.unite(DatabaseAccess().db()->getItemIDsInAlbum(albumId).toSet());
    }

    // Get all items DB id from all tags
    foreach(int albumId, tags2Scan)
    {
        idList.unite(DatabaseAccess().db()->getItemIDsInTag(albumId).toSet());
    }

    return findDuplicates(idList, requiredPercentage, observer);
}

QMap< qlonglong, QList<qlonglong> > HaarIface::findDuplicates(const QSet<qlonglong>& images2Scan,
                                                              double requiredPercentage,
                                                              HaarProgressObserver* const observer)
{
    QMap< qlonglong, QList<qlonglong> > resultsMap;
    QSet<qlonglong>::const_iterator     it;
    QList<qlonglong>                    bestMatchesList;
    QSet<qlonglong>                     resultsCandidates;

    int                                 total        = 0;
    int                                 progress     = 0;
    int                                 progressStep = 20;

    if (observer)
    {
        total        = images2Scan.count();
        progressStep = qMax(progressStep, total / 100);
        observer->totalNumberToScan(total);
    }

    // create signature cache map for fast lookup
    d->setSignatureCacheEnabled(true, images2Scan);

    for (it = images2Scan.constBegin(); it != images2Scan.constEnd(); ++it)
    {
        if (!resultsCandidates.contains(*it))
        {
            // find images with required similarity
            bestMatchesList = bestMatchesForImageWithThreshold(*it, requiredPercentage, ScannedSketch);

            if (!bestMatchesList.isEmpty())
            {
                // the list will usually contain one image: the original. Filter out.
                if (!(bestMatchesList.count() == 1 && bestMatchesList.first() == *it))
                {
                    resultsMap.insert(*it, bestMatchesList);
                    resultsCandidates << *it;
                    resultsCandidates.unite(bestMatchesList.toSet());
                }
            }
        }

        // if an imageid is not a results candidate, remove it from the cached signature map as well,
        // to greatly improve speed
        if (!resultsCandidates.contains(*it))
        {
            d->signatureCache->remove(*it);
        }

        ++progress;

        if (observer && (progress == total || progress % progressStep == 0))
        {
            observer->processedNumber(progress);
        }
    }

    // make sure the progress bar is really set to 100% when search is finished
    if (observer)
    {
        observer->processedNumber(total);
    }

    // disable cache
    d->setSignatureCacheEnabled(false);

    return resultsMap;
}

double HaarIface::calculateScore(Haar::SignatureData& querySig, Haar::SignatureData& targetSig,
                                 Haar::Weights& weights, Haar::SignatureMap** const queryMaps)
{
    double score = 0.0;

    // Step 1: Initialize scores with average intensity values of all three channels
    for (int channel = 0; channel < 3; ++channel)
    {
        score += weights.weightForAverage(channel) * fabs( querySig.avg[channel] - targetSig.avg[channel] );
    }

    // Step 2: Decrease the score if query and target have significant coefficients in common
    Haar::Idx* sig               = 0;
    Haar::SignatureMap* queryMap = 0;
    int x                        = 0;

    for (int channel = 0; channel < 3; ++channel)
    {
        sig      = targetSig.sig[channel];
        queryMap = queryMaps[channel];

        for (int coef = 0; coef < Haar::NumberOfCoefficients; ++coef)
        {
            // x is a pixel index, either positive or negative, 0..16384
            x = sig[coef];

            // If x is a significant coefficient with the same sign in the query signature as well,
            // decrease the score (lower is better)
            // Note: both method calls called with x accept positive or negative values
            if ((*queryMap)[x])
            {
                score -= weights.weight(d->bin->binAbs(x), channel);
            }
        }
    }

    return score;
}

}  // namespace Digikam
