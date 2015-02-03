/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2007-07-19
 * Description : A widget to display XMP metadata
 *
 * Copyright (C) 2007-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "xmpwidget.moc"

// Qt includes

#include <QMap>
#include <QFile>

// KDE includes


#include <klocale.h>

// Local includes

#include "dmetadata.h"

namespace Digikam
{

static const char* StandardXmpEntryList[] =
{
    "aux",             // Schema for Additional Exif Properties.
    "crs",             // Camera Raw schema.
    "dc",              // Dublin Core schema.
    "digiKam",         // Our Xmp schema used to store private information (see DMetadata class for details).
    "kipi",            // Our 2nd Xmp schema used to store private information from kipi-plugins.
    "exif",            // Schema for Exif-specific Properties.
    "iptc",            // IPTC Core schema.
    "iptcExt",         // IPTC Extension schema.
    "MicrosoftPhoto",  // Microsoft schema.
    "pdf",             // Adobe PDF schema.
    "photoshop",       // Adobe Photoshop schema.
    "plus",            // PLUS License Data Format Schema.
    "tiff",            // Schema for TIFF Properties
    "xmp",             // Basic schema.
    "xmpBJ",           // Basic Job Ticket schema.
    "xmpDM",           // Dynamic Media schema.
    "xmpMM",           // Media Management schema.
    "xmpRights",       // Rights Management schema.
    "xmpTPg",          // Paged-Text schema.
    "lr",              // Adobe LightRoom schema.
    "video",           // Exiv2 Video Metadata Schema
    "audio",           // Exiv2 Audio Metadata Schema

    "-1"
};

XmpWidget::XmpWidget(QWidget* const parent, const char* name)
    : MetadataWidget(parent, name)
{
    for (int i=0 ; QString(StandardXmpEntryList[i]) != QString("-1") ; ++i)
    {
        m_keysFilter << StandardXmpEntryList[i];
    }
}

XmpWidget::~XmpWidget()
{
}

QString XmpWidget::getMetadataTitle()
{
    return i18n("XMP Schema");
}

bool XmpWidget::loadFromURL(const KUrl& url)
{
    setFileName(url.fileName());

    if (url.isEmpty())
    {
        setMetadata();
        return false;
    }
    else
    {
        DMetadata metadata(url.toLocalFile());

        if (!metadata.hasXmp())
        {
            setMetadata();
            return false;
        }
        else
        {
            setMetadata(metadata);
        }
    }

    return true;
}

bool XmpWidget::decodeMetadata()
{
    DMetadata data = getMetadata();

    if (!data.hasXmp())
    {
        return false;
    }

    // Update all metadata contents.
    setMetadataMap(data.getXmpTagsDataList(QStringList()));
    return true;
}

void XmpWidget::buildView()
{
    switch (getMode())
    {
        case CUSTOM:
            setIfdList(getMetadataMap(), m_keysFilter, getTagsFilter());
            break;

        case PHOTO:
            setIfdList(getMetadataMap(), m_keysFilter, QStringList() << QString("FULL"));
            break;

        default: // NONE
            setIfdList(getMetadataMap(), QStringList());
            break;
    }

    MetadataWidget::buildView();
}

QString XmpWidget::getTagTitle(const QString& key)
{
    DMetadata metadataIface;
    QString title = metadataIface.getXmpTagTitle(key.toAscii());

    if (title.isEmpty())
    {
        return key.section('.', -1);
    }

    return title;
}

QString XmpWidget::getTagDescription(const QString& key)
{
    DMetadata metadataIface;
    QString desc = metadataIface.getXmpTagDescription(key.toAscii());

    if (desc.isEmpty())
    {
        return i18n("No description available");
    }

    return desc;
}

void XmpWidget::slotSaveMetadataToFile()
{
    KUrl url = saveMetadataToFile(i18n("XMP File to Save"),
                                  QString("*.xmp|"+i18n("XMP text Files (*.xmp)")));
    storeMetadataToFile(url, getMetadata().getXmp());
}

}  // namespace Digikam
