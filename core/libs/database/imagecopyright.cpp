/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2008-05-12
 * Description : Access to copyright info of an image in the database
 *
 * Copyright (C) 2008-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2009-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "imagecopyright.h"

// KDE includes


#include <klocale.h>
#include <kglobal.h>

// Local includes

#include "albumdb.h"
#include "databaseaccess.h"
#include "imagescanner.h"
#include "template.h"

namespace Digikam
{

class ImageCopyrightCache
{
public:

    explicit ImageCopyrightCache(ImageCopyright* const object)
        : object(object)
    {
        // set this as cache
        object->m_cache = this;
        // read all properties
        infos = DatabaseAccess().db()->getImageCopyright(object->m_id, QString());
    }

    ~ImageCopyrightCache()
    {
        object->m_cache = 0;
    }

    QList<CopyrightInfo> infos;

private:

    ImageCopyright* object;
};

// -------------------------------------------------------------------------------------------

ImageCopyright::ImageCopyright(qlonglong imageid)
    : m_id(imageid), m_cache(0)
{
}

ImageCopyright::ImageCopyright()
    : m_id(0), m_cache(0)
{
}

ImageCopyright::ImageCopyright(const ImageCopyright& other)
    : m_id(other.m_id), m_cache(0)
{
    // the cache is only short-lived, to keep complexity low
}

ImageCopyright::~ImageCopyright()
{
    delete m_cache;
    m_cache = 0;
}

ImageCopyright& ImageCopyright::operator=(const ImageCopyright& other)
{
    delete m_cache;
    m_cache = 0;

    m_id = other.m_id;
    return *this;
}

void ImageCopyright::replaceFrom(const ImageCopyright& source)
{
    if (!m_id)
    {
        return;
    }

    DatabaseAccess access;
    access.db()->removeImageCopyrightProperties(m_id);

    if (!source.m_id)
    {
        return;
    }

    QList<CopyrightInfo> infos = access.db()->getImageCopyright(source.m_id, QString());

    foreach(const CopyrightInfo& info, infos)
    {
        access.db()->setImageCopyrightProperty(m_id, info.property, info.value,
                                               info.extraValue, AlbumDB::PropertyNoConstraint);
    }
}

QStringList ImageCopyright::creator() const
{
    QList<CopyrightInfo> infos = copyrightInfos(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreCreator));
    QStringList list;

    foreach(const CopyrightInfo& info, infos)
    {
        list << info.value;
    }

    return list;
}

void ImageCopyright::setCreator(const QString& creator, ReplaceMode mode)
{
    AlbumDB::CopyrightPropertyUnique uniqueness;

    if (mode == ReplaceAllEntries)
    {
        uniqueness = AlbumDB::PropertyUnique;
    }
    else
    {
        uniqueness = AlbumDB::PropertyNoConstraint;
    }

    DatabaseAccess().db()->setImageCopyrightProperty(m_id, ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreCreator),
                                                     creator, QString(), uniqueness);
}

void ImageCopyright::removeCreators()
{
    removeProperties(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreCreator));
}

QString ImageCopyright::provider() const
{
    return readSimpleProperty(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreProvider));
}

void ImageCopyright::setProvider(const QString& provider)
{
    setSimpleProperty(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreProvider), provider);
}

void ImageCopyright::removeProvider()
{
    removeProperties(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreProvider));
}

QString ImageCopyright::copyrightNotice(const QString& languageCode)
{
    return readLanguageProperty(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreCopyrightNotice), languageCode);
}

KExiv2Iface::KExiv2::AltLangMap ImageCopyright::allCopyrightNotices()
{
    return readLanguageProperties(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreCopyrightNotice));
}

void ImageCopyright::setCopyrightNotice(const QString& notice, const QString& languageCode, ReplaceMode mode)
{
    setLanguageProperty(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreCopyrightNotice), notice, languageCode, mode);
}

void ImageCopyright::removeCopyrightNotices()
{
    removeProperties(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreCopyrightNotice));
}

QString ImageCopyright::rightsUsageTerms(const QString& languageCode)
{
    return readLanguageProperty(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreRightsUsageTerms), languageCode);
}

KExiv2Iface::KExiv2::AltLangMap ImageCopyright::allRightsUsageTerms()
{
    return readLanguageProperties(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreRightsUsageTerms));
}

void ImageCopyright::setRightsUsageTerms(const QString& term, const QString& languageCode, ReplaceMode mode)
{
    setLanguageProperty(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreRightsUsageTerms), term, languageCode, mode);
}

void ImageCopyright::removeRightsUsageTerms()
{
    removeProperties(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreRightsUsageTerms));
}

QString ImageCopyright::source()
{
    return readSimpleProperty(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreSource));
}

void ImageCopyright::setSource(const QString& source)
{
    setSimpleProperty(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreSource), source);
}

void ImageCopyright::removeSource()
{
    removeProperties(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreSource));
}

QString ImageCopyright::creatorJobTitle() const
{
    return readSimpleProperty(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreCreatorJobTitle));
}

void ImageCopyright::setCreatorJobTitle(const QString& title)
{
    setSimpleProperty(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreCreatorJobTitle), title);
}

void ImageCopyright::removeCreatorJobTitle()
{
    removeProperties(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreCreatorJobTitle));
}

QString ImageCopyright::instructions()
{
    return readSimpleProperty(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreInstructions));
}

void ImageCopyright::setInstructions(const QString& instructions)
{
    setSimpleProperty(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreInstructions), instructions);
}

void ImageCopyright::removeInstructions()
{
    removeProperties(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreInstructions));
}

IptcCoreContactInfo ImageCopyright::contactInfo()
{
    IptcCoreContactInfo info;
    info.city          = readSimpleProperty(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreContactInfoCity));
    info.country       = readSimpleProperty(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreContactInfoCountry));
    info.address       = readSimpleProperty(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreContactInfoAddress));
    info.postalCode    = readSimpleProperty(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreContactInfoPostalCode));
    info.provinceState = readSimpleProperty(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreContactInfoProvinceState));
    info.email         = readSimpleProperty(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreContactInfoEmail));
    info.phone         = readSimpleProperty(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreContactInfoPhone));
    info.webUrl        = readSimpleProperty(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreContactInfoWebUrl));
    return info;
}

void ImageCopyright::setContactInfo(const IptcCoreContactInfo& info)
{
    setSimpleProperty(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreContactInfoCity), info.city);
    setSimpleProperty(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreContactInfoCountry), info.country);
    setSimpleProperty(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreContactInfoAddress), info.address);
    setSimpleProperty(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreContactInfoPostalCode), info.postalCode);
    setSimpleProperty(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreContactInfoProvinceState), info.provinceState);
    setSimpleProperty(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreContactInfoEmail), info.email);
    setSimpleProperty(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreContactInfoPhone), info.phone);
    setSimpleProperty(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreContactInfoWebUrl), info.webUrl);
}

void ImageCopyright::removeContactInfo()
{
    removeProperties(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreContactInfoCity));
    removeProperties(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreContactInfoCountry));
    removeProperties(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreContactInfoAddress));
    removeProperties(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreContactInfoPostalCode));
    removeProperties(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreContactInfoProvinceState));
    removeProperties(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreContactInfoEmail));
    removeProperties(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreContactInfoPhone));
    removeProperties(ImageScanner::iptcCorePropertyName(MetadataInfo::IptcCoreContactInfoWebUrl));
}

void ImageCopyright::fillTemplate(Template& t)
{
    ImageCopyrightCache cache(this);

    t.setAuthors(author());
    t.setAuthorsPosition(authorsPosition());
    t.setCredit(credit());
    t.setCopyright(allCopyrightNotices());
    t.setRightUsageTerms(allRightsUsageTerms());
    t.setSource(source());
    t.setInstructions(instructions());
    t.setContactInfo(contactInfo());
}

void ImageCopyright::setFromTemplate(const Template& t)
{
    foreach(QString author, t.authors()) // krazy:exclude=foreach
    {
        setAuthor(author, ImageCopyright::AddEntryToExisting);
    }

    setCredit(t.credit());

    KExiv2::AltLangMap copyrights = t.copyright();
    KExiv2::AltLangMap::const_iterator it;

    for (it = copyrights.constBegin() ; it != copyrights.constEnd() ; ++it)
    {
        setRights(it.value(), it.key(), ImageCopyright::AddEntryToExisting);
    }

    KExiv2::AltLangMap usages = t.rightUsageTerms();
    KExiv2::AltLangMap::const_iterator it2;

    for (it2 = usages.constBegin() ; it2 != usages.constEnd() ; ++it2)
    {
        setRightsUsageTerms(it2.value(), it2.key(), ImageCopyright::AddEntryToExisting);
    }

    setSource(t.source());
    setAuthorsPosition(t.authorsPosition());
    setInstructions(t.instructions());
    setContactInfo(t.contactInfo());
}

void ImageCopyright::removeAll()
{
    ImageCopyrightCache cache(this);

    removeCreators();
    removeProvider();
    removeCopyrightNotices();
    removeRightsUsageTerms();
    removeSource();
    removeCreatorJobTitle();
    removeInstructions();
    removeContactInfo();
}

CopyrightInfo ImageCopyright::copyrightInfo(const QString& property) const
{
    if (m_cache)
    {
        foreach(const CopyrightInfo& info, m_cache->infos)
        {
            if (info.property == property)
            {
                return info;
            }
        }
    }
    else
    {
        QList<CopyrightInfo> infos = DatabaseAccess().db()->getImageCopyright(m_id, property);

        if (!infos.isEmpty())
        {
            return infos.first();
        }
    }

    return CopyrightInfo();
}

QList<CopyrightInfo> ImageCopyright::copyrightInfos(const QString& property) const
{
    if (m_cache)
    {
        QList<CopyrightInfo> infos;

        foreach(const CopyrightInfo& info, m_cache->infos)
        {
            if (info.property == property)
            {
                infos << info;
            }
        }

        return infos;
    }
    else
    {
        return DatabaseAccess().db()->getImageCopyright(m_id, property);
    }
}

QString ImageCopyright::readSimpleProperty(const QString& property) const
{
    return copyrightInfo(property).value;
}

void ImageCopyright::setSimpleProperty(const QString& property, const QString& value)
{
    DatabaseAccess().db()->setImageCopyrightProperty(m_id, property, value, QString(), AlbumDB::PropertyUnique);
}

QString ImageCopyright::readLanguageProperty(const QString& property, const QString& languageCode)
{
    QList<CopyrightInfo> infos = copyrightInfos(property);
    int index                  = languageMatch(infos, languageCode);

    if (index == -1)
    {
        return QString();
    }
    else
    {
        return infos.at(index).value;
    }
}

KExiv2Iface::KExiv2::AltLangMap ImageCopyright::readLanguageProperties(const QString& property)
{
    KExiv2Iface::KExiv2::AltLangMap map;
    QList<CopyrightInfo> infos = copyrightInfos(property);

    foreach(const CopyrightInfo& info, infos)
    {
        map[info.extraValue] = info.value;
    }

    return map;
}

void ImageCopyright::setLanguageProperty(const QString& property, const QString& value,
                                         const QString& languageCode, ReplaceMode mode)
{
    AlbumDB::CopyrightPropertyUnique uniqueness;

    if (mode == ReplaceAllEntries)
    {
        uniqueness = AlbumDB::PropertyUnique;
    }
    else if (mode == ReplaceLanguageEntry)
    {
        uniqueness = AlbumDB::PropertyExtraValueUnique;
    }
    else
    {
        uniqueness = AlbumDB::PropertyNoConstraint;
    }

    QString language = languageCode;

    if (language.isNull())
    {
        language = "x-default";
    }

    DatabaseAccess().db()->setImageCopyrightProperty(m_id, property, value, language, uniqueness);
}

void ImageCopyright::removeProperties(const QString& property)
{
    // if we have a cache, find out if anything need to be done at all
    if (m_cache && copyrightInfo(property).isNull())
    {
        return;
    }

    DatabaseAccess().db()->removeImageCopyrightProperties(m_id, property);
}

void ImageCopyright::removeLanguageProperty(const QString& property, const QString& languageCode)
{
    if (m_cache && copyrightInfo(property).isNull())
    {
        return;
    }

    DatabaseAccess().db()->removeImageCopyrightProperties(m_id, property, languageCode);
}

int ImageCopyright::languageMatch(const QList<CopyrightInfo> infos, const QString& languageCode) const
{
    QString langCode;
    QString fullCode = languageCode;

    if (languageCode.isNull())
    {
        // find local language
        KLocale* const locale = KGlobal::locale();
        langCode              = locale->language().toLower() + '-';
        fullCode              = langCode + locale->country().toLower();
    }
    else if (languageCode == "x-default")
    {
        langCode = languageCode;
    }
    else
    {
        // en-us => en-
        langCode = languageCode.section('-', 0, 0, QString::SectionIncludeTrailingSep);
    }

    int fullCodeMatch, langCodeMatch, defaultCodeMatch, firstMatch;
    fullCodeMatch    = -1;
    langCodeMatch    = -1;
    defaultCodeMatch = -1;
    firstMatch       = -1;

    if (infos.isEmpty())
    {
        return -1;
    }
    else
    {
        firstMatch = 0; // index of first entry - at least we have one
    }

    // First we search for a full match
    // Second for a match of the language code
    // Third for the default code
    // Fourth we return the first comment

    QLatin1String defaultCode("x-default");

    for (int i=0; i<infos.size(); ++i)
    {
        const CopyrightInfo& info = infos.at(i);

        if (info.extraValue == fullCode)
        {
            fullCodeMatch = i;
            break;
        }
        else if (info.extraValue.startsWith(langCode) && langCodeMatch == -1)
        {
            langCodeMatch = i;
        }
        else if (info.extraValue == defaultCode)
        {
            defaultCodeMatch = i;
        }
    }

    int chosen = fullCodeMatch;

    if (chosen == -1)
    {
        chosen = langCodeMatch;
    }

    if (chosen == -1)
    {
        chosen = defaultCodeMatch;
    }

    if (chosen == -1)
    {
        chosen = firstMatch;
    }

    return chosen;
}

} // namespace Digikam
