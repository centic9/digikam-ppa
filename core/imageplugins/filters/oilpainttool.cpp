/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2004-08-25
 * Description : a plugin to simulate Oil Painting
 *
 * Copyright (C) 2004-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#include "oilpainttool.moc"

// Qt includes

#include <QGridLayout>
#include <QImage>
#include <QLabel>

// KDE includes

#include <kapplication.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <kcursor.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kstandarddirs.h>

// LibKDcraw includes

#include <libkdcraw/rnuminput.h>

// Local includes

#include "dimg.h"
#include "editortoolsettings.h"
#include "imageiface.h"
#include "imageregionwidget.h"
#include "oilpaintfilter.h"

using namespace KDcrawIface;

namespace DigikamFxFiltersImagePlugin
{

class OilPaintTool::Private
{
public:

    Private() :
        brushSizeInput(0),
        smoothInput(0),
        previewWidget(0),
        gboxSettings(0)
    {}

    static const QString configGroupName;
    static const QString configBrushSizeEntry;
    static const QString configSmoothAdjustmentEntry;

    RIntNumInput*        brushSizeInput;
    RIntNumInput*        smoothInput;

    ImageRegionWidget*   previewWidget;
    EditorToolSettings*  gboxSettings;
};

const QString OilPaintTool::Private::configGroupName("oilpaint Tool");
const QString OilPaintTool::Private::configBrushSizeEntry("BrushSize");
const QString OilPaintTool::Private::configSmoothAdjustmentEntry("SmoothAdjustment");

// --------------------------------------------------------

OilPaintTool::OilPaintTool(QObject* const parent)
    : EditorToolThreaded(parent),
      d(new Private)
{
    setObjectName("oilpaint");
    setToolName(i18n("Oil Paint"));
    setToolIcon(SmallIcon("oilpaint")); // krazy:exclude=iconnames

    // -------------------------------------------------------------

    d->gboxSettings = new EditorToolSettings;
    d->gboxSettings->setButtons(EditorToolSettings::Default|
                                EditorToolSettings::Ok|
                                EditorToolSettings::Cancel|
                                EditorToolSettings::Try);

    // -------------------------------------------------------------

    QLabel* label1    = new QLabel(i18n("Brush size:"));
    d->brushSizeInput = new RIntNumInput();
    d->brushSizeInput->setRange(1, 30, 1);
    d->brushSizeInput->setSliderEnabled(true);
    d->brushSizeInput->setDefaultValue(1);
    d->brushSizeInput->setWhatsThis(i18n("Set here the brush size to use for simulating the oil painting."));

    // -------------------------------------------------------------

    QLabel* label2  = new QLabel(i18nc("value of smoothing effect", "Smooth:"));
    d->smoothInput  = new RIntNumInput();
    d->smoothInput->setRange(10, 255, 1);
    d->smoothInput->setSliderEnabled(true);
    d->smoothInput->setDefaultValue(10);
    d->smoothInput->setWhatsThis(i18n("This value controls the smoothing effect of the brush under the canvas."));

    QGridLayout* mainLayout = new QGridLayout();
    mainLayout->addWidget(label1,            0, 0, 1, 2);
    mainLayout->addWidget(d->brushSizeInput, 1, 0, 1, 2);
    mainLayout->addWidget(label2,            2, 0, 1, 2);
    mainLayout->addWidget(d->smoothInput,    3, 0, 1, 2);
    mainLayout->setRowStretch(4, 10);
    mainLayout->setMargin(d->gboxSettings->spacingHint());
    mainLayout->setSpacing(d->gboxSettings->spacingHint());
    d->gboxSettings->plainPage()->setLayout(mainLayout);

    // -------------------------------------------------------------

    d->previewWidget = new ImageRegionWidget;

    setToolSettings(d->gboxSettings);
    setToolView(d->previewWidget);
    setPreviewModeMask(PreviewToolBar::AllPreviewModes);
}

OilPaintTool::~OilPaintTool()
{
    delete d;
}

void OilPaintTool::readSettings()
{
    KSharedConfig::Ptr config = KGlobal::config();
    KConfigGroup group        = config->group(d->configGroupName);
    d->brushSizeInput->blockSignals(true);
    d->smoothInput->blockSignals(true);
    d->brushSizeInput->setValue(group.readEntry(d->configBrushSizeEntry,     d->brushSizeInput->defaultValue()));
    d->smoothInput->setValue(group.readEntry(d->configSmoothAdjustmentEntry, d->smoothInput->defaultValue()));
    d->brushSizeInput->blockSignals(false);
    d->smoothInput->blockSignals(false);
}

void OilPaintTool::writeSettings()
{
    KSharedConfig::Ptr config = KGlobal::config();
    KConfigGroup group        = config->group(d->configGroupName);

    group.writeEntry(d->configBrushSizeEntry,        d->brushSizeInput->value());
    group.writeEntry(d->configSmoothAdjustmentEntry, d->smoothInput->value());
    group.sync();
}

void OilPaintTool::slotResetSettings()
{
    d->brushSizeInput->blockSignals(true);
    d->smoothInput->blockSignals(true);

    d->brushSizeInput->slotReset();
    d->smoothInput->slotReset();

    d->brushSizeInput->blockSignals(false);
    d->smoothInput->blockSignals(false);

    slotPreview();
}

void OilPaintTool::preparePreview()
{
    DImg image = d->previewWidget->getOriginalRegionImage();
    int b      = d->brushSizeInput->value();
    int s      = d->smoothInput->value();

    setFilter(new OilPaintFilter(&image, this, b, s));
}

void OilPaintTool::prepareFinal()
{
    int b = d->brushSizeInput->value();
    int s = d->smoothInput->value();

    ImageIface iface;
    setFilter(new OilPaintFilter(iface.original(), this, b, s));
}

void OilPaintTool::setPreviewImage()
{
    d->previewWidget->setPreviewImage(filter()->getTargetImage());
}

void OilPaintTool::setFinalImage()
{
    ImageIface iface;
    iface.setOriginal(i18n("Oil Paint"), filter()->filterAction(), filter()->getTargetImage());
}

}  // namespace DigikamFxFiltersImagePlugin
