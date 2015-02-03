/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2008-12-10
 * Description : album icon view tool tip
 *
 * Copyright (C) 2008-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2013      by Michael G. Hansen <mike at mghansen dot de>
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

#include "tooltipfiller.h"

// Qt includes

#include <QDateTime>
#include <QBuffer>
#include <QTextDocument>

// KDE includes

#include <kdebug.h>
#include <klocale.h>
#include <kfileitem.h>
#include <kglobal.h>
#include <kdeversion.h>

// Local includes

#include "albummanager.h"
#include "collectionlocation.h"
#include "collectionmanager.h"
#include "applicationsettings.h"
#include "album.h"
#include "databaseinfocontainers.h"
#include "dimgfiltermanager.h"
#include "ditemtooltip.h"
#include "filteraction.h"
#include "imageinfo.h"
#include "imagepropertiestab.h"
#include "colorlabelwidget.h"
#include "picklabelwidget.h"
#include "albumthumbnailloader.h"
#include "thumbnailsize.h"

namespace Digikam
{

QString ToolTipFiller::imageInfoTipContents(const ImageInfo& info)
{
    QString              str;
    ApplicationSettings* const settings = ApplicationSettings::instance();
    DToolTipStyleSheet   cnt(settings->getToolTipsFont());

    ImageCommonContainer commonInfo     = info.imageCommonContainer();
    ImageMetadataContainer photoInfo    = info.imageMetadataContainer();
    VideoMetadataContainer videoInfo    = info.videoMetadataContainer();
    QString tip                         = cnt.tipHeader;

    // -- File properties ----------------------------------------------

    if (settings->getToolTipsShowFileName()  ||
        settings->getToolTipsShowFileDate()  ||
        settings->getToolTipsShowFileSize()  ||
        settings->getToolTipsShowImageType() ||
        settings->getToolTipsShowImageDim()  ||
        settings->getToolTipsShowImageAR())

    {
        tip += cnt.headBeg + i18n("File Properties") + cnt.headEnd;

        if (settings->getToolTipsShowFileName())
        {
            tip += cnt.cellBeg + i18nc("filename",
                                       "Name:") + cnt.cellMid;
            tip += commonInfo.fileName + cnt.cellEnd;
        }

        if (settings->getToolTipsShowFileDate())
        {
            QDateTime modifiedDate = commonInfo.fileModificationDate;
            str                    = KGlobal::locale()->formatDateTime(modifiedDate, KLocale::ShortDate, true);
            tip                   += cnt.cellBeg + i18n("Date:") + cnt.cellMid + str + cnt.cellEnd;
        }

        if (settings->getToolTipsShowFileSize())
        {
            tip                   += cnt.cellBeg + i18n("Size:") + cnt.cellMid;
            QString localeFileSize = KGlobal::locale()->formatNumber(commonInfo.fileSize, 0);
            str                    = i18n("%1 (%2)", KIO::convertSize(commonInfo.fileSize), localeFileSize);
            tip                   += str + cnt.cellEnd;
        }

        QSize dims;

        if (settings->getToolTipsShowImageType())
        {
            tip += cnt.cellBeg + i18n("Type:") + cnt.cellMid + commonInfo.format + cnt.cellEnd;
        }

        if (settings->getToolTipsShowImageDim())
        {
            if (commonInfo.width == 0 || commonInfo.height == 0)
            {
                str = i18nc("unknown / invalid image dimension",
                            "Unknown");
            }
            else
            {
                QString mpixels;
                mpixels.setNum(commonInfo.width*commonInfo.height/1000000.0, 'f', 2);
                str = i18nc("width x height (megapixels Mpx)", "%1x%2 (%3Mpx)",
                            commonInfo.width, commonInfo.height, mpixels);
            }

            tip += cnt.cellBeg + i18n("Dimensions:") + cnt.cellMid + str + cnt.cellEnd;
        }

       if (settings->getToolTipsShowImageAR())
        {
            if (!ImagePropertiesTab::aspectRatioToString(commonInfo.width, commonInfo.height, str))
            {
                str = i18nc("unknown / invalid image aspect ratio",
                            "Unknown");
            }

            tip += cnt.cellBeg + i18n("Aspect Ratio:") + cnt.cellMid + str + cnt.cellEnd;
        }

    }

    // -- Photograph Info ----------------------------------------------------

    if (settings->getToolTipsShowPhotoMake()  ||
        settings->getToolTipsShowPhotoDate()  ||
        settings->getToolTipsShowPhotoFocal() ||
        settings->getToolTipsShowPhotoExpo()  ||
        settings->getToolTipsShowPhotoMode()  ||
        settings->getToolTipsShowPhotoFlash() ||
        settings->getToolTipsShowPhotoWB())
    {
        if (!photoInfo.allFieldsNull)
        {
            QString metaStr;
            tip += cnt.headBeg + i18n("Photograph Properties") + cnt.headEnd;

            if (settings->getToolTipsShowPhotoMake())
            {
                ImagePropertiesTab::shortenedMakeInfo(photoInfo.make);
                ImagePropertiesTab::shortenedModelInfo(photoInfo.model);

                str = QString("%1 / %2").arg(photoInfo.make.isEmpty() ? cnt.unavailable : photoInfo.make)
                      .arg(photoInfo.model.isEmpty() ? cnt.unavailable : photoInfo.model);

                if (str.length() > cnt.maxStringLength)
                {
                    str = str.left(cnt.maxStringLength-3) + "...";
                }

                metaStr += cnt.cellBeg + i18n("Make/Model:") + cnt.cellMid + Qt::escape(str) + cnt.cellEnd;
            }

            if (settings->getToolTipsShowPhotoDate())
            {
                if (commonInfo.creationDate.isValid())
                {
                    str = KGlobal::locale()->formatDateTime(commonInfo.creationDate, KLocale::ShortDate, true);

                    if (str.length() > cnt.maxStringLength)
                    {
                        str = str.left(cnt.maxStringLength-3) + "...";
                    }

                    metaStr += cnt.cellBeg + i18nc("creation date of the image",
                                                   "Created:") + cnt.cellMid + Qt::escape(str) + cnt.cellEnd;
                }
                else
                {
                    metaStr += cnt.cellBeg + i18nc("creation date of the image",
                                                   "Created:") + cnt.cellMid + Qt::escape(cnt.unavailable) + cnt.cellEnd;
                }
            }

            if (settings->getToolTipsShowPhotoFocal())
            {
                str = photoInfo.aperture.isEmpty() ? cnt.unavailable : photoInfo.aperture;

                if (photoInfo.focalLength35.isEmpty())
                {
                    str += QString(" / %1").arg(photoInfo.focalLength.isEmpty() ? cnt.unavailable : photoInfo.focalLength);
                }
                else
                {
                    str += QString(" / %1").arg(i18n("%1 (%2)",photoInfo.focalLength,photoInfo.focalLength35));
                }

                if (str.length() > cnt.maxStringLength)
                {
                    str = str.left(cnt.maxStringLength-3) + "...";
                }

                metaStr += cnt.cellBeg + i18n("Aperture/Focal:") + cnt.cellMid + Qt::escape(str) + cnt.cellEnd;
            }

            if (settings->getToolTipsShowPhotoExpo())
            {
                str = QString("%1 / %2").arg(photoInfo.exposureTime.isEmpty() ? cnt.unavailable : photoInfo.exposureTime)
                      .arg(photoInfo.sensitivity.isEmpty() ? cnt.unavailable : i18n("%1 ISO",photoInfo.sensitivity));

                if (str.length() > cnt.maxStringLength)
                {
                    str = str.left(cnt.maxStringLength-3) + "...";
                }

                metaStr += cnt.cellBeg + i18n("Exposure/Sensitivity:") + cnt.cellMid + Qt::escape(str) + cnt.cellEnd;
            }

            if (settings->getToolTipsShowPhotoMode())
            {
                if (photoInfo.exposureMode.isEmpty() && photoInfo.exposureProgram.isEmpty())
                {
                    str = cnt.unavailable;
                }
                else if (!photoInfo.exposureMode.isEmpty() && photoInfo.exposureProgram.isEmpty())
                {
                    str = photoInfo.exposureMode;
                }
                else if (photoInfo.exposureMode.isEmpty() && !photoInfo.exposureProgram.isEmpty())
                {
                    str = photoInfo.exposureProgram;
                }
                else
                {
                    str = QString("%1 / %2").arg(photoInfo.exposureMode).arg(photoInfo.exposureProgram);
                }

                if (str.length() > cnt.maxStringLength)
                {
                    str = str.left(cnt.maxStringLength-3) + "...";
                }

                metaStr += cnt.cellBeg + i18n("Mode/Program:") + cnt.cellMid + Qt::escape(str) + cnt.cellEnd;
            }

            if (settings->getToolTipsShowPhotoFlash())
            {
                str = photoInfo.flashMode.isEmpty() ? cnt.unavailable : photoInfo.flashMode;

                if (str.length() > cnt.maxStringLength)
                {
                    str = str.left(cnt.maxStringLength-3) + "...";
                }

                metaStr += cnt.cellBeg + i18nc("camera flash settings",
                                               "Flash:") + cnt.cellMid + Qt::escape(str) + cnt.cellEnd;
            }

            if (settings->getToolTipsShowPhotoWB())
            {
                str = photoInfo.whiteBalance.isEmpty() ? cnt.unavailable : photoInfo.whiteBalance;

                if (str.length() > cnt.maxStringLength)
                {
                    str = str.left(cnt.maxStringLength-3) + "...";
                }

                metaStr += cnt.cellBeg + i18n("White Balance:") + cnt.cellMid + Qt::escape(str) + cnt.cellEnd;
            }

            tip += metaStr;
        }
    }

    // -- Video Metadata Info ----------------------------------------------------

    if (settings->getToolTipsShowVideoAspectRatio()        ||
        settings->getToolTipsShowVideoDuration()           ||
        settings->getToolTipsShowVideoFrameRate()          ||
        settings->getToolTipsShowVideoVideoCodec()         ||
        settings->getToolTipsShowVideoAudioBitRate()       ||
        settings->getToolTipsShowVideoAudioChannelType()   ||
        settings->getToolTipsShowVideoAudioCompressor())
    {
        if (!videoInfo.allFieldsNull)
        {
            QString metaStr;
            tip += cnt.headBeg + i18n("Video Properties") + cnt.headEnd;

            if (settings->getToolTipsShowVideoAspectRatio())
            {
                str = videoInfo.aspectRatio.isEmpty() ? cnt.unavailable : videoInfo.aspectRatio;

                if (str.length() > cnt.maxStringLength)
                {
                    str = str.left(cnt.maxStringLength-3) + "...";
                }

                metaStr += cnt.cellBeg + i18n("Aspect Ratio:") + cnt.cellMid + Qt::escape(str) + cnt.cellEnd;
            }

            if (settings->getToolTipsShowVideoDuration())
            {
                QString durationString;
                bool ok;
                const double durationDouble = videoInfo.duration.toDouble(&ok);

                if (ok)
                {
                    const QTime durationTime = QTime().addMSecs(durationDouble);
                    durationString = KGlobal::locale()->formatTime(durationTime, true, true);
                }

                str = videoInfo.duration.isEmpty() ? cnt.unavailable : durationString;

                if (str.length() > cnt.maxStringLength)
                {
                    str = str.left(cnt.maxStringLength-3) + "...";
                }

                metaStr += cnt.cellBeg + i18n("Duration:") + cnt.cellMid + Qt::escape(str) + cnt.cellEnd;
            }

            if (settings->getToolTipsShowVideoFrameRate())
            {
                QString frameRateString;
                bool ok;
                const double frameRateDouble = videoInfo.frameRate.toDouble(&ok);

                if (ok)
                {
                    frameRateString = KGlobal::locale()->formatNumber(frameRateDouble);
                }

                str = videoInfo.frameRate.isEmpty() ? cnt.unavailable : frameRateString;

                if (str.length() > cnt.maxStringLength)
                {
                    str = str.left(cnt.maxStringLength-3) + "...";
                }

                metaStr += cnt.cellBeg + i18n("Frame Rate:") + cnt.cellMid + Qt::escape(str) + i18n("fps") + cnt.cellEnd;
            }

            if (settings->getToolTipsShowVideoVideoCodec())
            {
                str = videoInfo.videoCodec.isEmpty() ? cnt.unavailable : videoInfo.videoCodec;

                if (str.length() > cnt.maxStringLength)
                {
                    str = str.left(cnt.maxStringLength-3) + "...";
                }

                metaStr += cnt.cellBeg + i18n("Video Codec:") + cnt.cellMid + Qt::escape(str) + cnt.cellEnd;
            }

            if (settings->getToolTipsShowVideoAudioBitRate())
            {
                QString audioBitRateString = str;
                bool ok;
                const int audioBitRateInt  = videoInfo.audioBitRate.toInt(&ok);

                if (ok)
                {
                    audioBitRateString = KGlobal::locale()->formatNumber(audioBitRateInt, 0);
                }

                str = videoInfo.audioBitRate.isEmpty() ? cnt.unavailable : audioBitRateString;

                if (str.length() > cnt.maxStringLength)
                {
                    str = str.left(cnt.maxStringLength-3) + "...";
                }

                metaStr += cnt.cellBeg + i18n("Audio Bit Rate:") + cnt.cellMid + Qt::escape(str) + cnt.cellEnd;
            }

            if (settings->getToolTipsShowVideoAudioChannelType())
            {
                str = videoInfo.audioChannelType.isEmpty() ? cnt.unavailable : videoInfo.audioChannelType;

                if (str.length() > cnt.maxStringLength)
                {
                    str = str.left(cnt.maxStringLength-3) + "...";
                }

                metaStr += cnt.cellBeg + i18n("Audio Channel Type:") + cnt.cellMid + Qt::escape(str) + cnt.cellEnd;
            }

            if (settings->getToolTipsShowVideoAudioCompressor())
            {
                str = videoInfo.audioCompressor.isEmpty() ? cnt.unavailable : videoInfo.audioCompressor;

                if (str.length() > cnt.maxStringLength)
                {
                    str = str.left(cnt.maxStringLength-3) + "...";
                }

                metaStr += cnt.cellBeg + i18n("Audio Compressor:") + cnt.cellMid + Qt::escape(str) + cnt.cellEnd;
            }

            tip += metaStr;
        }
    }

    // -- digiKam properties  ------------------------------------------

    if (settings->getToolTipsShowAlbumName()    ||
        settings->getToolTipsShowTitles()       ||
        settings->getToolTipsShowComments()     ||
        settings->getToolTipsShowTags()         ||
        settings->getToolTipsShowLabelRating())
    {
        tip += cnt.headBeg + i18n("digiKam Properties") + cnt.headEnd;

        if (settings->getToolTipsShowAlbumName())
        {
            PAlbum* const album = AlbumManager::instance()->findPAlbum(info.albumId());

            if (album)
            {
                tip += cnt.cellSpecBeg + i18n("Album:") + cnt.cellSpecMid + album->albumPath().remove(0, 1) + cnt.cellSpecEnd;
            }
        }

        if (settings->getToolTipsShowTitles())
        {
            str = info.title();

            if (str.isEmpty())
            {
                str = QString("---");
            }

            tip += cnt.cellSpecBeg + i18nc("title of the file",
                                           "Title:") + cnt.cellSpecMid +
                   cnt.breakString(str) + cnt.cellSpecEnd;
        }

        if (settings->getToolTipsShowComments())
        {
            str = info.comment();

            if (str.isEmpty())
            {
                str = QString("---");
            }

            tip += cnt.cellSpecBeg + i18nc("caption of the file",
                                           "Caption:") + cnt.cellSpecMid +
                   cnt.breakString(str) + cnt.cellSpecEnd;
        }

        if (settings->getToolTipsShowTags())
        {
            QStringList tagPaths = AlbumManager::instance()->tagPaths(info.tagIds(), false);
            tagPaths.sort();

            QStringList tagNames = AlbumManager::instance()->tagNames(info.tagIds());
            tagNames.sort();

            QString tags(i18n("Tags:"));

            if (tagPaths.isEmpty())
            {
                tip += cnt.cellSpecBeg + tags + cnt.cellSpecMid + "---" + cnt.cellSpecEnd;
            }
            else
            {
                QString title = tags;
                QString tagText;

                for (int i = 0; i<tagPaths.size(); ++i)
                {
                    tagText = tagPaths.at(i);

                    if (tagText.size() > cnt.maxStringLength)
                    {
                        tagText = tagNames.at(i);
                    }

                    if (tagText.size() > cnt.maxStringLength)
                    {
                        tagText = cnt.elidedText(tagPaths.at(i), Qt::ElideLeft);
                    }

                    tip += cnt.cellSpecBeg + title + cnt.cellSpecMid + tagText + cnt.cellSpecEnd;
                    title.clear();
                }
            }
        }

        if (settings->getToolTipsShowLabelRating())
        {
            str        = PickLabelWidget::labelPickName((PickLabel)info.pickLabel());
            str       += QString(" / ");

            str       += ColorLabelWidget::labelColorName((ColorLabel)info.colorLabel());
            str       += QString(" / ");

            int rating = info.rating();

            if (rating > RatingMin && rating <= RatingMax)
            {
                for (int i=0; i<rating; ++i)
                {
                    str += QChar(0x2730);
                    str += ' ';
                }
            }
            else
            {
                str += QString("---");
            }

            tip += cnt.cellSpecBeg + i18n("Labels:") + cnt.cellSpecMid + str + cnt.cellSpecEnd;
        }
    }

    tip += cnt.tipFooter;

    return tip;
}

QString ToolTipFiller::albumTipContents(PAlbum* const album, int count)
{
    if (!album)
    {
        return QString();
    }

    QString                    str;
    ApplicationSettings* const settings = ApplicationSettings::instance();
    DToolTipStyleSheet         cnt(settings->getToolTipsFont());
    QString                    tip      = cnt.tipHeader;

    if (settings->getToolTipsShowAlbumTitle()      ||
        settings->getToolTipsShowAlbumDate()       ||
        settings->getToolTipsShowAlbumCollection() ||
        settings->getToolTipsShowAlbumCategory()   ||
        settings->getToolTipsShowAlbumCaption())
    {
        tip += cnt.headBeg + i18n("Album Properties") + cnt.headEnd;

        if (settings->getToolTipsShowAlbumTitle())
        {
            tip += cnt.cellBeg + i18n("Name:") + cnt.cellMid;
            tip += album->title() + cnt.cellEnd;
        }

        if (settings->getShowFolderTreeViewItemsCount())
        {
            tip += cnt.cellBeg + i18n("Items:") + cnt.cellMid;
            tip += QString::number(count) + cnt.cellEnd;
        }

        if (settings->getToolTipsShowAlbumCollection())
        {
            tip += cnt.cellBeg + i18n("Collection:") + cnt.cellMid;
            CollectionLocation col = CollectionManager::instance()->locationForAlbumRootId(album->albumRootId());
            tip += !col.isNull() ? col.label() : QString() + cnt.cellEnd;
        }

        if (settings->getToolTipsShowAlbumDate())
        {
            QDate date = album->date();
            str        = KGlobal::locale()->formatDate(date, KLocale::ShortDate);
            tip        += cnt.cellBeg + i18n("Date:") + cnt.cellMid + str + cnt.cellEnd;
        }

        if (settings->getToolTipsShowAlbumCategory())
        {
            str = album->category();

            if (str.isEmpty())
            {
                str = QString("---");
            }

            tip += cnt.cellSpecBeg + i18n("Category:") + cnt.cellSpecMid +
                   cnt.breakString(str) + cnt.cellSpecEnd;
        }

        if (settings->getToolTipsShowAlbumCaption())
        {
            str = album->caption();

            if (str.isEmpty())
            {
                str = QString("---");
            }

            tip += cnt.cellSpecBeg + i18n("Caption:") + cnt.cellSpecMid +
                   cnt.breakString(str) + cnt.cellSpecEnd;
        }


        if (settings->getToolTipsShowAlbumPreview())
        {
            tip += cnt.cellSpecBeg + i18n("Preview:") + cnt.cellSpecMid +
            cnt.imageAsBase64(AlbumThumbnailLoader::instance()->getAlbumThumbnailDirectly(album).toImage()) +
            //cnt.imageAsBase64(AlbumThumbnailLoader::instance()->getAlbumPreviewDirectly(album, ThumbnailSize::Medium)) +
            cnt.cellSpecEnd;
        }
    }

    tip += cnt.tipFooter;

    return tip;
}

QString ToolTipFiller::filterActionTipContents(const FilterAction& action)
{
    if (action.isNull())
    {
        return QString();
    }

    QString            str;
    DToolTipStyleSheet cnt(ApplicationSettings::instance()->getToolTipsFont());
    QString            tip = cnt.tipHeader;

    tip += cnt.headBeg + i18n("Filter") + cnt.headEnd;

    // Displayable name
    tip += cnt.cellBeg + i18n("Name:") + cnt.cellMid
        + DImgFilterManager::instance()->i18nDisplayableName(action) + cnt.cellEnd;

    // Category
    QString reproducible("---");

    switch (action.category())
    {
        case FilterAction::ReproducibleFilter:
            reproducible = i18nc("Image filter reproducible: Yes", "Yes");
            break;
        case FilterAction::ComplexFilter:
            reproducible = i18nc("Image filter reproducible: Partially", "Partially");
            break;
        case FilterAction::DocumentedHistory:
            reproducible = i18nc("Image filter reproducible: No", "No");
            break;
        default:
            break;
    };

    tip += cnt.cellBeg + i18n("Reproducible:") + cnt.cellMid
         + reproducible + cnt.cellEnd;

    // Description

    str = action.description();

    if (str.isEmpty())
    {
        str = QString("---");
    }

    tip += cnt.cellSpecBeg + i18nc("Image filter description", "Description:") + cnt.cellSpecMid
        + cnt.breakString(str) + cnt.cellSpecEnd;

    // Identifier + version
    tip += cnt.cellBeg + i18n("Identifier:") + cnt.cellMid
        + action.identifier() + " (v" + QString::number(action.version()) + ") " + cnt.cellEnd;

    if (action.hasParameters())
    {
        tip += cnt.headBeg + i18n("Technical Parameters") + cnt.headEnd;

        const QHash<QString, QVariant>& params = action.parameters();
        QList<QString> keys                    = params.keys();
        qSort(keys);

        foreach(const QString& key, keys)
        {
            QHash<QString, QVariant>::const_iterator it;

            for (it = params.find(key); it != params.end() && it.key() == key; ++it)
            {
                if (it.key().isEmpty() || !it.value().isValid())
                {
                    continue;
                }

                if (it.key().startsWith(QLatin1String("curveData")))
                {
                    str = i18n("<i>Binary Data</i>");
                }
                else
                {
                    str = it.value().toString();
                }

                if (str.length() > cnt.maxStringLength)
                {
                    str = cnt.elidedText(str, Qt::ElideRight);
                }

                QString key = it.key();
                QChar first = key.at(0);

                if (first.isLower())
                {
                    key.replace(0, 1, first.toUpper());
                }

                tip += cnt.cellBeg + key + cnt.cellMid
                    + str + cnt.cellEnd;
            }
        }
    }

    tip += cnt.tipFooter;

    return tip;
}

}  // namespace Digikam
