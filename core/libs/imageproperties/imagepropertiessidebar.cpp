/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2004-11-17
 * Description : simple image properties side bar (without support
 *               of digiKam database).
 *
 * Copyright (C) 2004-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "imagepropertiessidebar.moc"

// Qt includes

#include <QRect>
#include <QSplitter>
#include <QFileInfo>

// KDE includes

#include <kfileitem.h>
#include <klocale.h>
#include <kconfig.h>
#include <kapplication.h>
#include <kcursor.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <kglobalsettings.h>
#include <kaboutdata.h>
#include <kcomponentdata.h>

// LibKDcraw includes

#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

// Local includes

#include "dimg.h"
#include "dmetadata.h"
#include "imagepropertiestab.h"
#include "imagepropertiesmetadatatab.h"
#include "imagepropertiescolorstab.h"
#include "imagepropertiesversionstab.h"

#ifdef HAVE_KGEOMAP
#include "imagepropertiesgpstab.h"
#endif // HAVE_KGEOMAP

namespace Digikam
{

ImagePropertiesSideBar::ImagePropertiesSideBar(QWidget* const parent,
                                               SidebarSplitter* const splitter,
                                               KMultiTabBarPosition side,
                                               bool mimimizedDefault)
    : Sidebar(parent, splitter, side, mimimizedDefault)
{
    m_image              = 0;
    m_currentRect        = QRect();
    m_dirtyPropertiesTab = false;
    m_dirtyMetadataTab   = false;
    m_dirtyColorTab      = false;
    m_dirtyGpsTab        = false;
    m_dirtyHistoryTab    = false;

    m_propertiesTab      = new ImagePropertiesTab(parent);
    m_metadataTab        = new ImagePropertiesMetaDataTab(parent);
    m_colorTab           = new ImagePropertiesColorsTab(parent);

    // NOTE: Special case with Showfoto which will only be able to load image, not video.
    if (KGlobal::mainComponent().aboutData()->appName() != QString("digikam"))
        m_propertiesTab->setVideoInfoDisable(true);

    appendTab(m_propertiesTab, SmallIcon("document-properties"),   i18n("Properties"));
    appendTab(m_metadataTab,   SmallIcon("exifinfo"),              i18n("Metadata")); // krazy:exclude=iconnames
    appendTab(m_colorTab,      SmallIcon("format-fill-color"),     i18n("Colors"));

#ifdef HAVE_KGEOMAP
    m_gpsTab = new ImagePropertiesGPSTab(parent);
    appendTab(m_gpsTab,        SmallIcon("applications-internet"), i18n("Geolocation"));
#endif // HAVE_KGEOMAP

    connect(this, SIGNAL(signalChangedTab(QWidget*)),
            this, SLOT(slotChangedTab(QWidget*)));

    connect(m_metadataTab, SIGNAL(signalSetupMetadataFilters(int)),
            this, SIGNAL(signalSetupMetadataFilters(int)));
}

ImagePropertiesSideBar::~ImagePropertiesSideBar()
{
}

void ImagePropertiesSideBar::itemChanged(const KUrl& url, const QRect& rect, DImg* const img)
{
    if (!url.isValid())
    {
        return;
    }

    m_currentURL         = url;
    m_currentRect        = rect;
    m_image              = img;
    m_dirtyPropertiesTab = false;
    m_dirtyMetadataTab   = false;
    m_dirtyColorTab      = false;
    m_dirtyGpsTab        = false;
    m_dirtyHistoryTab    = false;

    slotChangedTab( getActiveTab() );
}

void ImagePropertiesSideBar::slotNoCurrentItem()
{
    m_currentURL = KUrl();

    m_propertiesTab->setCurrentURL();
    m_metadataTab->setCurrentURL();
    m_colorTab->setData();

#ifdef HAVE_KGEOMAP
    m_gpsTab->setCurrentURL();
#endif // HAVE_KGEOMAP

    m_dirtyPropertiesTab = false;
    m_dirtyMetadataTab   = false;
    m_dirtyColorTab      = false;
    m_dirtyGpsTab        = false;
    m_dirtyHistoryTab    = false;
}

void ImagePropertiesSideBar::slotImageSelectionChanged(const QRect& rect)
{
    m_currentRect = rect;

    if (m_dirtyColorTab)
    {
        m_colorTab->setSelection(rect);
    }
    else
    {
        slotChangedTab(m_colorTab);
    }
}

void ImagePropertiesSideBar::slotChangedTab(QWidget* tab)
{
    if (!m_currentURL.isValid())
    {
#ifdef HAVE_KGEOMAP
        m_gpsTab->setActive(tab == m_gpsTab);
#endif // HAVE_KGEOMAP
        return;
    }

    setCursor(Qt::WaitCursor);

    if (tab == m_propertiesTab && !m_dirtyPropertiesTab)
    {
        m_propertiesTab->setCurrentURL(m_currentURL);
        setImagePropertiesInformation(m_currentURL);
        m_dirtyPropertiesTab = true;
    }
    else if (tab == m_metadataTab && !m_dirtyMetadataTab)
    {
        m_metadataTab->setCurrentURL(m_currentURL);
        m_dirtyMetadataTab = true;
    }
    else if (tab == m_colorTab && !m_dirtyColorTab)
    {
        m_colorTab->setData(m_currentURL, m_currentRect, m_image);
        m_dirtyColorTab = true;
    }
#ifdef HAVE_KGEOMAP
    else if (tab == m_gpsTab && !m_dirtyGpsTab)
    {
        m_gpsTab->setCurrentURL(m_currentURL);
        m_dirtyGpsTab = true;
    }

    m_gpsTab->setActive(tab == m_gpsTab);
#endif // HAVE_KGEOMAP

    unsetCursor();
}

void ImagePropertiesSideBar::setImagePropertiesInformation(const KUrl& url)
{
    if (!url.isValid())
    {
        return;
    }

    QString str;
    QString unavailable(i18n("<i>unavailable</i>"));

    KFileItem fi(KFileItem::Unknown, KFileItem::Unknown, url);
    QFileInfo fileInfo(url.toLocalFile());
    DMetadata metaData(url.toLocalFile());

    // -- File system information -----------------------------------------

    QDateTime modifiedDate = fileInfo.lastModified();
    str = KGlobal::locale()->formatDateTime(modifiedDate, KLocale::ShortDate, true);
    m_propertiesTab->setFileModifiedDate(str);

    str = QString("%1 (%2)").arg(KIO::convertSize(fi.size()))
          .arg(KGlobal::locale()->formatNumber(fi.size(), 0));
    m_propertiesTab->setFileSize(str);
    m_propertiesTab->setFileOwner(QString("%1 - %2").arg(fi.user()).arg(fi.group()));
    m_propertiesTab->setFilePermissions(fi.permissionsString());

    // -- Image Properties --------------------------------------------------

    QSize   dims;
    QString bitDepth, colorMode;
    QString rawFilesExt(KDcrawIface::KDcraw::rawFiles());
    QString ext = fileInfo.suffix().toUpper();

    if (!ext.isEmpty() && rawFilesExt.toUpper().contains(ext))
    {
        m_propertiesTab->setImageMime(i18n("RAW Image"));
        bitDepth    = "48";
        dims        = metaData.getImageDimensions();
        colorMode   = i18n("Uncalibrated");
    }
    else
    {
        m_propertiesTab->setImageMime(fi.mimeComment());

        dims = metaData.getPixelSize();

        DImg img;
        img.loadImageInfo(url.toLocalFile(), false, false, false, false);
        bitDepth.number(img.originalBitDepth());
        colorMode = DImg::colorModelToString(img.originalColorModel());
    }

    QString mpixels;
    mpixels.setNum(dims.width()*dims.height()/1000000.0, 'f', 2);
    str = (!dims.isValid()) ? i18n("Unknown") : i18n("%1x%2 (%3Mpx)",
            dims.width(), dims.height(), mpixels);
    m_propertiesTab->setImageDimensions(str);

    if (!dims.isValid()) str = i18n("Unknown");
    else m_propertiesTab->aspectRatioToString(dims.width(), dims.height(), str);

    m_propertiesTab->setImageRatio(str);
    m_propertiesTab->setImageBitDepth(bitDepth.isEmpty()   ? unavailable : i18n("%1 bpp", bitDepth));
    m_propertiesTab->setImageColorMode(colorMode.isEmpty() ? unavailable : colorMode);

    // -- Photograph information ------------------------------------------

    PhotoInfoContainer photoInfo = metaData.getPhotographInformation();

    m_propertiesTab->setPhotoInfoDisable(photoInfo.isEmpty());
    ImagePropertiesTab::shortenedMakeInfo(photoInfo.make);
    ImagePropertiesTab::shortenedModelInfo(photoInfo.model);
    m_propertiesTab->setPhotoMake(photoInfo.make.isEmpty()   ? unavailable : photoInfo.make);
    m_propertiesTab->setPhotoModel(photoInfo.model.isEmpty() ? unavailable : photoInfo.model);

    if (photoInfo.dateTime.isValid())
    {
        str = KGlobal::locale()->formatDateTime(photoInfo.dateTime, KLocale::ShortDate, true);
        m_propertiesTab->setPhotoDateTime(str);
    }
    else
    {
        m_propertiesTab->setPhotoDateTime(unavailable);
    }

    m_propertiesTab->setPhotoLens(photoInfo.lens.isEmpty()         ? unavailable : photoInfo.lens);
    m_propertiesTab->setPhotoAperture(photoInfo.aperture.isEmpty() ? unavailable : photoInfo.aperture);

    if (photoInfo.focalLength35mm.isEmpty())
    {
        m_propertiesTab->setPhotoFocalLength(photoInfo.focalLength.isEmpty() ? unavailable : photoInfo.focalLength);
    }
    else
    {
        str = i18n("%1 (%2)", photoInfo.focalLength, photoInfo.focalLength35mm);
        m_propertiesTab->setPhotoFocalLength(str);
    }

    m_propertiesTab->setPhotoExposureTime(photoInfo.exposureTime.isEmpty() ? unavailable : photoInfo.exposureTime);
    m_propertiesTab->setPhotoSensitivity(photoInfo.sensitivity.isEmpty()   ? unavailable : i18n("%1 ISO", photoInfo.sensitivity));

    if (photoInfo.exposureMode.isEmpty() && photoInfo.exposureProgram.isEmpty())
    {
        m_propertiesTab->setPhotoExposureMode(unavailable);
    }
    else if (!photoInfo.exposureMode.isEmpty() && photoInfo.exposureProgram.isEmpty())
    {
        m_propertiesTab->setPhotoExposureMode(photoInfo.exposureMode);
    }
    else if (photoInfo.exposureMode.isEmpty() && !photoInfo.exposureProgram.isEmpty())
    {
        m_propertiesTab->setPhotoExposureMode(photoInfo.exposureProgram);
    }
    else
    {
        str = QString("%1 / %2").arg(photoInfo.exposureMode).arg(photoInfo.exposureProgram);
        m_propertiesTab->setPhotoExposureMode(str);
    }

    m_propertiesTab->setPhotoFlash(photoInfo.flash.isEmpty()               ? unavailable : photoInfo.flash);
    m_propertiesTab->setPhotoWhiteBalance(photoInfo.whiteBalance.isEmpty() ? unavailable : photoInfo.whiteBalance);

    // -- Video information ------------------------------------------

    VideoInfoContainer videoInfo = metaData.getVideoInformation();

    m_propertiesTab->setVideoInfoDisable(videoInfo.isEmpty());

    m_propertiesTab->setVideoAspectRatio(videoInfo.aspectRatio.isEmpty()           ? unavailable : videoInfo.aspectRatio);
    m_propertiesTab->setVideoDuration(videoInfo.duration.isEmpty()                 ? unavailable : videoInfo.duration);
    m_propertiesTab->setVideoFrameRate(videoInfo.frameRate.isEmpty()               ? unavailable : videoInfo.frameRate);
    m_propertiesTab->setVideoVideoCodec(videoInfo.videoCodec.isEmpty()             ? unavailable : videoInfo.videoCodec);
    m_propertiesTab->setVideoAudioBitRate(videoInfo.audioBitRate.isEmpty()         ? unavailable : videoInfo.audioBitRate);
    m_propertiesTab->setVideoAudioChannelType(videoInfo.audioChannelType.isEmpty() ? unavailable : videoInfo.audioChannelType);
    m_propertiesTab->setVideoAudioCompressor(videoInfo.audioCompressor.isEmpty()   ? unavailable : videoInfo.audioCompressor);

    // -- Caption, ratings, tag information ---------------------

    CaptionsMap captions = metaData.getImageComments();
    QString caption;

    if (captions.contains("x-default"))
        caption = captions.value("x-default").caption;
    else if (!captions.isEmpty())
        caption = captions.begin().value().caption;

    m_propertiesTab->setCaption(caption);

    m_propertiesTab->setRating(metaData.getImageRating());

    QStringList tagPaths;
    metaData.getImageTagsPath(tagPaths);
    m_propertiesTab->setTags(tagPaths);
    m_propertiesTab->showOrHideCaptionAndTags();
}

void ImagePropertiesSideBar::doLoadState()
{
    Sidebar::doLoadState();

    /// @todo m_propertiesTab should load its settings from our group
    m_propertiesTab->setObjectName("Image Properties SideBar Expander");

    KConfigGroup group = getConfigGroup();

#if KDCRAW_VERSION >= 0x020000
    m_propertiesTab->readSettings(group);
#else
    m_propertiesTab->readSettings();
#endif

#ifdef HAVE_KGEOMAP
    const KConfigGroup groupGPSTab      = KConfigGroup(&group, entryName("GPS Properties Tab"));
    m_gpsTab->readSettings(groupGPSTab);
#endif // HAVE_KGEOMAP

    const KConfigGroup groupColorTab    = KConfigGroup(&group, entryName("Color Properties Tab"));
    m_colorTab->readSettings(groupColorTab);

    const KConfigGroup groupMetadataTab = KConfigGroup(&group, entryName("Metadata Properties Tab"));
    m_metadataTab->readSettings(groupMetadataTab);
}

void ImagePropertiesSideBar::doSaveState()
{
    Sidebar::doSaveState();

    KConfigGroup group = getConfigGroup();

#if KDCRAW_VERSION >= 0x020000
    m_propertiesTab->writeSettings(group);
#else
    m_propertiesTab->writeSettings();
#endif

#ifdef HAVE_KGEOMAP
    KConfigGroup groupGPSTab      = KConfigGroup(&group, entryName("GPS Properties Tab"));
    m_gpsTab->writeSettings(groupGPSTab);
#endif // HAVE_KGEOMAP

    KConfigGroup groupColorTab    = KConfigGroup(&group, entryName("Color Properties Tab"));
    m_colorTab->writeSettings(groupColorTab);

    KConfigGroup groupMetadataTab = KConfigGroup(&group, entryName("Metadata Properties Tab"));
    m_metadataTab->writeSettings(groupMetadataTab);
}

void ImagePropertiesSideBar::slotLoadMetadataFilters()
{
    m_metadataTab->loadFilters();
}

}  // namespace Digikam
