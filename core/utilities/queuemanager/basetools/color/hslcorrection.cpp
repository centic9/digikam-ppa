/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2010-02-11
 * Description : Hue/Saturation/Lightness batch tool.
 *
 * Copyright (C) 2010-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "hslcorrection.moc"

// Qt includes

#include <QWidget>

// KDE includes

#include <klocale.h>
#include <kstandarddirs.h>
#include <kglobal.h>

// Local includes

#include "dimg.h"
#include "hslfilter.h"
#include "hslsettings.h"

namespace Digikam
{

HSLCorrection::HSLCorrection(QObject* const parent)
    : BatchTool("HSLCorrection", ColorTool, parent)
{
    m_settingsView = 0;
    setToolTitle(i18n("HSL Correction"));
    setToolDescription(i18n("Fix Hue/Saturation/Lightness."));
    setToolIconName("adjusthsl");
}

HSLCorrection::~HSLCorrection()
{
}

void HSLCorrection::registerSettingsWidget()
{
    m_settingsWidget = new QWidget;
    m_settingsView   = new HSLSettings(m_settingsWidget);

    connect(m_settingsView, SIGNAL(signalSettingsChanged()),
            this, SLOT(slotSettingsChanged()));

    BatchTool::registerSettingsWidget();
}

BatchToolSettings HSLCorrection::defaultSettings()
{
    BatchToolSettings prm;
    HSLContainer defaultPrm = m_settingsView->defaultSettings();

    prm.insert("Hue", (double)defaultPrm.hue);
    prm.insert("Saturation", (double)defaultPrm.saturation);
    prm.insert("Lightness", (double)defaultPrm.lightness);
    prm.insert("Vibrance", (double)defaultPrm.vibrance);

    return prm;
}

void HSLCorrection::slotAssignSettings2Widget()
{
    HSLContainer prm;
    prm.hue        = settings()["Hue"].toDouble();
    prm.saturation = settings()["Saturation"].toDouble();
    prm.lightness  = settings()["Lightness"].toDouble();
    prm.vibrance   = settings()["Vibrance"].toDouble();
    m_settingsView->setSettings(prm);
}

void HSLCorrection::slotSettingsChanged()
{
    BatchToolSettings prm;
    HSLContainer currentPrm = m_settingsView->settings();

    prm.insert("Hue", (double)currentPrm.hue);
    prm.insert("Saturation", (double)currentPrm.saturation);
    prm.insert("Lightness", (double)currentPrm.lightness);
    prm.insert("Vibrance", (double)currentPrm.vibrance);

    BatchTool::slotSettingsChanged(prm);
}

bool HSLCorrection::toolOperations()
{
    if (!loadToDImg())
    {
        return false;
    }

    HSLContainer prm;
    prm.hue        = settings()["Hue"].toDouble();
    prm.saturation = settings()["Saturation"].toDouble();
    prm.lightness  = settings()["Lightness"].toDouble();
    prm.vibrance   = settings()["Vibrance"].toDouble();

    HSLFilter hsl(&image(), 0L, prm);
    applyFilter(&hsl);

    return (savefromDImg());
}

}  // namespace Digikam
