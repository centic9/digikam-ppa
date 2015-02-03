/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2009-06-29
 * Description : metadata template viewer.
 *
 * Copyright (C) 2009-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "templateviewer.moc"

// Qt includes

#include <QGridLayout>

// KDE includes

#include <klocale.h>
#include <kiconloader.h>
#include <kdialog.h>
#include <kvbox.h>
#include <kurllabel.h>
#include <ktoolinvocation.h>

// LibKExiv2 includes

#include <libkexiv2/countryselector.h>

// Local includes

#include "imagepropertiestxtlabel.h"
#include "template.h"
#include "templatemanager.h"

using namespace KExiv2Iface;

namespace Digikam
{

class TemplateViewer::Private
{
public:

    Private()
    {
        names                      = 0;
        position                   = 0;
        credit                     = 0;
        copyright                  = 0;
        usages                     = 0;
        source                     = 0;
        instructions               = 0;
        namesList                  = 0;
        labelPosition              = 0;
        labelCredit                = 0;
        labelCopyright             = 0;
        labelUsages                = 0;
        labelSource                = 0;
        labelInstructions          = 0;
        locationCountry            = 0;
        locationProvinceState      = 0;
        locationCity               = 0;
        locationSublocation        = 0;
        labelLocationCountry       = 0;
        labelLocationProvinceState = 0;
        labelLocationCity          = 0;
        labelLocationSublocation   = 0;
        contactCity                = 0;
        contactCountry             = 0;
        contactAddress             = 0;
        contactPostalCode          = 0;
        contactProvinceState       = 0;
        contactEmail               = 0;
        contactPhone               = 0;
        contactWebUrl              = 0;
        labelContactCity           = 0;
        labelContactCountry        = 0;
        labelContactAddress        = 0;
        labelContactPostalCode     = 0;
        labelContactProvinceState  = 0;
        labelContactPhone          = 0;
        labelContactEmail          = 0;
        labelContactWebUrl         = 0;
        subjectsList               = 0;
    }

    // IPTC Rights info.

    DTextLabelName* names;
    DTextLabelName* position;
    DTextLabelName* credit;
    DTextLabelName* copyright;
    DTextLabelName* usages;
    DTextLabelName* source;
    DTextLabelName* instructions;

    DTextList*      namesList;
    DTextBrowser*   labelPosition;
    DTextBrowser*   labelCredit;
    DTextBrowser*   labelCopyright;
    DTextBrowser*   labelUsages;
    DTextBrowser*   labelSource;
    DTextBrowser*   labelInstructions;

    // IPTC Location info.

    DTextLabelName* locationCountry;
    DTextLabelName* locationProvinceState;
    DTextLabelName* locationCity;
    DTextLabelName* locationSublocation;

    DTextBrowser*   labelLocationCountry;
    DTextBrowser*   labelLocationProvinceState;
    DTextBrowser*   labelLocationCity;
    DTextBrowser*   labelLocationSublocation;

    // IPTC Contact info.

    DTextLabelName* contactCity;
    DTextLabelName* contactCountry;
    DTextLabelName* contactAddress;
    DTextLabelName* contactPostalCode;
    DTextLabelName* contactProvinceState;
    DTextLabelName* contactPhone;
    DTextLabelName* contactEmail;
    DTextLabelName* contactWebUrl;

    DTextBrowser*   labelContactCity;
    DTextBrowser*   labelContactCountry;
    DTextBrowser*   labelContactAddress;
    DTextBrowser*   labelContactPostalCode;
    DTextBrowser*   labelContactProvinceState;
    DTextBrowser*   labelContactPhone;
    KUrlLabel*      labelContactEmail;
    KUrlLabel*      labelContactWebUrl;

    // IPTC Subjects info.
    DTextList*      subjectsList;
};

TemplateViewer::TemplateViewer(QWidget* const parent)
    : RExpanderBox(parent), d(new Private)
{
    setFrameStyle(QFrame::NoFrame);

    KVBox* w1            = new KVBox(this);
    d->names             = new DTextLabelName(i18n("Names:"), w1);
    d->namesList         = new DTextList(QStringList(), w1);
    d->position          = new DTextLabelName(i18n("Position:"), w1);
    d->labelPosition     = new DTextBrowser(QString(), w1);
    d->credit            = new DTextLabelName(i18n("Credit:"), w1);
    d->labelCredit       = new DTextBrowser(QString(), w1);
    d->copyright         = new DTextLabelName(i18n("Copyright:"), w1);
    d->labelCopyright    = new DTextBrowser(QString(), w1);
    d->usages            = new DTextLabelName(i18n("Usages:"), w1);
    d->labelUsages       = new DTextBrowser(QString(), w1);
    d->source            = new DTextLabelName(i18n("Source:"), w1);
    d->labelSource       = new DTextBrowser(QString(), w1);
    d->instructions      = new DTextLabelName(i18n("Instructions:"), w1);
    d->labelInstructions = new DTextBrowser(QString(), w1);

    d->names->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    d->position->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    d->credit->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    d->copyright->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    d->credit->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    d->usages->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    d->source->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    d->instructions->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    addItem(w1, SmallIcon("flag-red"),
            i18n("Rights"), QString("Rights"), true);

    // ------------------------------------------------------------------

    KVBox* w2                     = new KVBox(this);
    d->locationCity               = new DTextLabelName(i18n("City:"), w2);
    d->labelLocationCity          = new DTextBrowser(QString(), w2);
    d->locationSublocation        = new DTextLabelName(i18n("Sublocation:"), w2);
    d->labelLocationSublocation   = new DTextBrowser(QString(), w2);
    d->locationProvinceState      = new DTextLabelName(i18n("Province/State:"), w2);
    d->labelLocationProvinceState = new DTextBrowser(QString(), w2);
    d->locationCountry            = new DTextLabelName(i18n("Country:"), w2);
    d->labelLocationCountry       = new DTextBrowser(QString(), w2);

    d->locationCity->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    d->locationSublocation->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    d->locationProvinceState->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    d->locationCountry->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    addItem(w2, SmallIcon("applications-internet"),
            i18n("Location"), QString("Location"), true);

    // ------------------------------------------------------------------

    KVBox* w3                    = new KVBox(this);
    d->contactAddress            = new DTextLabelName(i18n("Address:"), w3);
    d->labelContactAddress       = new DTextBrowser(QString(), w3);
    d->contactPostalCode         = new DTextLabelName(i18n("Postal Code:"), w3);
    d->labelContactPostalCode    = new DTextBrowser(QString(), w3);
    d->contactCity               = new DTextLabelName(i18n("City:"), w3);
    d->labelContactCity          = new DTextBrowser(QString(), w3);
    d->contactProvinceState      = new DTextLabelName(i18n("Province/State:"), w3);
    d->labelContactProvinceState = new DTextBrowser(QString(), w3);
    d->contactCountry            = new DTextLabelName(i18n("Country:"), w3);
    d->labelContactCountry       = new DTextBrowser(QString(), w3);
    d->contactPhone              = new DTextLabelName(i18n("Phone:"), w3);
    d->labelContactPhone         = new DTextBrowser(QString(), w3);
    d->contactEmail              = new DTextLabelName(i18n("Email:"), w3);
    d->labelContactEmail         = new KUrlLabel(w3);
    d->contactWebUrl             = new DTextLabelName(i18n("URL:"), w3);
    d->labelContactWebUrl        = new KUrlLabel(w3);

    d->contactAddress->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    d->contactPostalCode->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    d->contactCity->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    d->contactProvinceState->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    d->contactCountry->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    d->contactPhone->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    d->contactEmail->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    d->contactWebUrl->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    addItem(w3, SmallIcon("view-pim-contacts"),
            i18n("Contact"), QString("Contact"), true);

    // ------------------------------------------------------------------

    KVBox* w4       = new KVBox(this);
    d->subjectsList = new DTextList(QStringList(), w4);

    addItem(w4, SmallIcon("feed-subscribe"),
            i18n("Subjects"), QString("Subjects"), true);

    addStretch();

    connect(d->labelContactWebUrl, SIGNAL(leftClickedUrl(QString)),
            this, SLOT(slotProcessUrl(QString)));

    connect(d->labelContactEmail, SIGNAL(leftClickedUrl(QString)),
            this, SLOT(slotProcessEmail(QString)));
}

TemplateViewer::~TemplateViewer()
{
    delete d;
}

void TemplateViewer::setTemplate(const Template& t)
{
    d->namesList->clear();
    d->namesList->addItems(t.authors());
    d->labelPosition->setText(t.authorsPosition());
    d->labelCredit->setText(t.credit());
    d->labelCopyright->setText(t.copyright()["x-default"]);
    d->labelUsages->setText(t.rightUsageTerms()["x-default"]);
    d->labelSource->setText(t.source());
    d->labelInstructions->setText(t.instructions());

    d->labelLocationCity->setText(t.locationInfo().city);
    d->labelLocationSublocation->setText(t.locationInfo().location);
    d->labelLocationProvinceState->setText(t.locationInfo().provinceState);
    d->labelLocationCountry->setText(CountrySelector::countryForCode(t.locationInfo().countryCode));

    d->labelContactAddress->setText(t.contactInfo().address);
    d->labelContactPostalCode->setText(t.contactInfo().postalCode);
    d->labelContactProvinceState->setText(t.contactInfo().provinceState);
    d->labelContactCity->setText(t.contactInfo().city);
    d->labelContactCountry->setText(t.contactInfo().country);
    d->labelContactPhone->setText(t.contactInfo().phone);
    d->labelContactEmail->setUrl(t.contactInfo().email);
    d->labelContactEmail->setText(t.contactInfo().email);
    KUrl url(t.contactInfo().webUrl);
    d->labelContactWebUrl->setText(url.host());
    d->labelContactWebUrl->setUrl(url.url());

    d->subjectsList->clear();
    d->subjectsList->addItems(t.IptcSubjects());
}

void TemplateViewer::slotProcessUrl(const QString& url)
{
    KToolInvocation::self()->invokeBrowser(url);
}

void TemplateViewer::slotProcessEmail(const QString& email)
{
    KToolInvocation::self()->invokeMailer(email);
}

}  // namespace Digikam
