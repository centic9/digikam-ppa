/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2012-01-30
 * Description : maintenance dialog
 *
 * Copyright (C) 2012-2017 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "maintenancedlg.h"

// Qt includes

#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QGridLayout>
#include <QComboBox>
#include <QScrollArea>
#include <QIcon>
#include <QStandardPaths>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QPushButton>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "dwidgetutils.h"
#include "dexpanderbox.h"
#include "dnuminput.h"
#include "digikam_config.h"
#include "setup.h"
#include "albumselectors.h"
#include "facescansettings.h"
#include "imagequalitysettings.h"
#include "metadatasynchronizer.h"
#include "dxmlguiwindow.h"

namespace Digikam
{

class MaintenanceDlg::Private
{
public:

    enum Operation
    {
        Options = 0,
        NewItems,
        Thumbnails,
        FingerPrints,
        Duplicates,
        FaceManagement,
        ImageQualitySorter,
        MetadataSync,
        Stretch
    };

public:

    Private() :
        buttons(0),
        logo(0),
        title(0),
        scanThumbs(0),
        scanFingerPrints(0),
        useMutiCoreCPU(0),
        qualityScanMode(0),
        metadataSetup(0),
        qualitySetup(0),
        syncDirection(0),
        hbox(0),
        vbox(0),
        vbox2(0),
        hbox3(0),
        faceScannedHandling(0),
        minSimilarity(0),
        maxSimilarity(0),
        expanderBox(0),
        albumSelectors(0)
    {
    }

    static const QString configGroupName;
    static const QString configUseMutiCoreCPU;
    static const QString configNewItems;
    static const QString configThumbnails;
    static const QString configScanThumbs;
    static const QString configFingerPrints;
    static const QString configScanFingerPrints;
    static const QString configDuplicates;
    static const QString configMinSimilarity;
    static const QString configMaxSimilarity;
    static const QString configFaceManagement;
    static const QString configFaceScannedHandling;
    static const QString configImageQualitySorter;
    static const QString configQualityScanMode;
    static const QString configMetadataSync;
    static const QString configSyncDirection;

    QDialogButtonBox*    buttons;
    QLabel*              logo;
    QLabel*              title;
    QCheckBox*           scanThumbs;
    QCheckBox*           scanFingerPrints;
    QCheckBox*           useMutiCoreCPU;
    QComboBox*           qualityScanMode;
    QPushButton*         metadataSetup;
    QPushButton*         qualitySetup;
    QComboBox*           syncDirection;
    DHBox*               hbox;
    DVBox*               vbox;
    DVBox*               vbox2;
    DHBox*               hbox3;
    QComboBox*           faceScannedHandling;
    DIntNumInput*        minSimilarity;
    DIntNumInput*        maxSimilarity;
    DExpanderBox*        expanderBox;
    AlbumSelectors*      albumSelectors;
};

const QString MaintenanceDlg::Private::configGroupName(QLatin1String("MaintenanceDlg Settings"));
const QString MaintenanceDlg::Private::configUseMutiCoreCPU(QLatin1String("UseMutiCoreCPU"));
const QString MaintenanceDlg::Private::configNewItems(QLatin1String("NewItems"));
const QString MaintenanceDlg::Private::configThumbnails(QLatin1String("Thumbnails"));
const QString MaintenanceDlg::Private::configScanThumbs(QLatin1String("ScanThumbs"));
const QString MaintenanceDlg::Private::configFingerPrints(QLatin1String("FingerPrints"));
const QString MaintenanceDlg::Private::configScanFingerPrints(QLatin1String("ScanFingerPrints"));
const QString MaintenanceDlg::Private::configDuplicates(QLatin1String("Duplicates"));
const QString MaintenanceDlg::Private::configMinSimilarity(QLatin1String("minSimilarity"));
const QString MaintenanceDlg::Private::configMaxSimilarity(QLatin1String("maxSimilarity"));
const QString MaintenanceDlg::Private::configFaceManagement(QLatin1String("FaceManagement"));
const QString MaintenanceDlg::Private::configFaceScannedHandling(QLatin1String("FaceScannedHandling"));
const QString MaintenanceDlg::Private::configImageQualitySorter(QLatin1String("ImageQualitySorter"));
const QString MaintenanceDlg::Private::configQualityScanMode(QLatin1String("QualityScanMode"));
const QString MaintenanceDlg::Private::configMetadataSync(QLatin1String("MetadataSync"));
const QString MaintenanceDlg::Private::configSyncDirection(QLatin1String("SyncDirection"));

MaintenanceDlg::MaintenanceDlg(QWidget* const parent)
    : QDialog(parent),
      d(new Private)
{
    setWindowTitle(i18n("Maintenance"));

    d->buttons = new QDialogButtonBox(QDialogButtonBox::Help | QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    d->buttons->button(QDialogButtonBox::Cancel)->setDefault(true);

    QScrollArea* const main = new QScrollArea(this);
    QWidget* const page     = new QWidget(main->viewport());
    main->setWidget(page);
    main->setWidgetResizable(true);

    QGridLayout* const grid = new QGridLayout(page);

    d->logo                 = new QLabel(page);
    d->logo->setPixmap(QIcon::fromTheme(QLatin1String("digikam")).pixmap(QSize(48,48)));
    d->title                = new QLabel(i18n("<qt><b>Select Maintenance Operations to Process</b></qt>"), page);
    d->expanderBox          = new DExpanderBox(page);

    // --------------------------------------------------------------------------------------

    DVBox* const options    = new DVBox;
    d->albumSelectors       = new AlbumSelectors(i18nc("@label", "Process items from:"), d->configGroupName, options);
    d->useMutiCoreCPU       = new QCheckBox(i18nc("@option:check", "Work on all processor cores (when it possible)"), options);
    d->expanderBox->insertItem(Private::Options, options, QIcon::fromTheme(QLatin1String("configure")), i18n("Common Options"), QLatin1String("Options"), true);

    // --------------------------------------------------------------------------------------

    d->expanderBox->insertItem(Private::NewItems, new QLabel(i18n("<qt>No option<br>"
                               "<i>Note: only Albums Collection are processed by this tool.</i></qt>")),
                               QIcon::fromTheme(QLatin1String("view-refresh")), i18n("Scan for new items"), QLatin1String("NewItems"), false);
    d->expanderBox->setCheckBoxVisible(Private::NewItems, true);

    // --------------------------------------------------------------------------------------

    d->scanThumbs        = new QCheckBox(i18n("Scan for changed or non-cataloged items (faster)"));
    d->expanderBox->insertItem(Private::Thumbnails, d->scanThumbs, QIcon::fromTheme(QLatin1String("view-process-all")),
                               i18n("Rebuild Thumbnails"), QLatin1String("Thumbnails"), false);
    d->expanderBox->setCheckBoxVisible(Private::Thumbnails, true);

    // --------------------------------------------------------------------------------------

    d->scanFingerPrints  = new QCheckBox(i18n("Scan for changed or non-cataloged items (faster)"));
    d->expanderBox->insertItem(Private::FingerPrints, d->scanFingerPrints, QIcon::fromTheme(QLatin1String("run-build")),
                               i18n("Rebuild Finger-prints"), QLatin1String("Fingerprints"), false);
    d->expanderBox->setCheckBoxVisible(Private::FingerPrints, true);

    // --------------------------------------------------------------------------------------

    d->hbox              = new DHBox;
    new QLabel(i18n("Similarity range (in percents): "), d->hbox);
    QWidget* const space = new QWidget(d->hbox);
    d->hbox->setStretchFactor(space, 10);
    d->minSimilarity        = new DIntNumInput(d->hbox);
    d->minSimilarity->setDefaultValue(90);
    d->minSimilarity->setRange(0, 100, 1);
    new QLabel(QLatin1String("-"), d->hbox);
    // Create the maximum similarity and set the minimum value to the current value of minSimilarity
    d->maxSimilarity        = new DIntNumInput(d->hbox);
    d->maxSimilarity->setDefaultValue(100);
    d->maxSimilarity->setRange(d->minSimilarity->value(), 100, 1);

    d->expanderBox->insertItem(Private::Duplicates, d->hbox, QIcon::fromTheme(QLatin1String("tools-wizard")),
                               i18n("Find Duplicates Items"), QLatin1String("Duplicates"), false);
    d->expanderBox->setCheckBoxVisible(Private::Duplicates, true);

    // --------------------------------------------------------------------------------------

    d->hbox3               = new DHBox;
    new QLabel(i18n("Faces data management: "), d->hbox3);
    QWidget* const space3  = new QWidget(d->hbox3);
    d->hbox3->setStretchFactor(space3, 10);
    d->faceScannedHandling = new QComboBox(d->hbox3);
    d->faceScannedHandling->addItem(i18n("Skip images already scanned"),          FaceScanSettings::Skip);
    d->faceScannedHandling->addItem(i18n("Scan again and merge results"),         FaceScanSettings::Merge);
    d->faceScannedHandling->addItem(i18n("Clear unconfirmed results and rescan"), FaceScanSettings::Rescan);
    d->expanderBox->insertItem(Private::FaceManagement, d->hbox3, QIcon::fromTheme(QLatin1String("edit-image-face-detect")),
                               i18n("Detect and recognize Faces (experimental)"), QLatin1String("FaceManagement"), false);
    d->expanderBox->setCheckBoxVisible(Private::FaceManagement, true);

    // --------------------------------------------------------------------------------------

    d->vbox               = new DVBox;
    DHBox* const hbox11   = new DHBox(d->vbox);
    new QLabel(i18n("Scan Mode: "), hbox11);
    QWidget* const space7 = new QWidget(hbox11);
    hbox11->setStretchFactor(space7, 10);

    d->qualityScanMode    = new QComboBox(hbox11);
    d->qualityScanMode->addItem(i18n("Clean all and re-scan"),  ImageQualitySorter::AllItems);
    d->qualityScanMode->addItem(i18n("Scan non-assigned only"), ImageQualitySorter::NonAssignedItems);

    DHBox* const hbox12   = new DHBox(d->vbox);
    new QLabel(i18n("Check quality sorter setup panel for details: "), hbox12);
    QWidget* const space2 = new QWidget(hbox12);
    hbox12->setStretchFactor(space2, 10);
    d->qualitySetup       = new QPushButton(i18n("Settings..."), hbox12);
    d->expanderBox->insertItem(Private::ImageQualitySorter, d->vbox, QIcon::fromTheme(QLatin1String("flag-green")),
                               i18n("Image Quality Sorter"), QLatin1String("ImageQualitySorter"), false);
    d->expanderBox->setCheckBoxVisible(Private::ImageQualitySorter, true);

    // --------------------------------------------------------------------------------------

    d->vbox2              = new DVBox;
    DHBox* const hbox21   = new DHBox(d->vbox2);
    new QLabel(i18n("Sync Direction: "), hbox21);
    QWidget* const space5 = new QWidget(hbox21);
    hbox21->setStretchFactor(space5, 10);
    d->syncDirection      = new QComboBox(hbox21);
    d->syncDirection->addItem(i18n("From database to image metadata"), MetadataSynchronizer::WriteFromDatabaseToFile);
    d->syncDirection->addItem(i18n("From image metadata to database"), MetadataSynchronizer::ReadFromFileToDatabase);

    DHBox* const hbox22   = new DHBox(d->vbox2);
    new QLabel(i18n("Check metadata setup panel for details: "), hbox22);
    QWidget* const space6 = new QWidget(hbox22);
    hbox22->setStretchFactor(space6, 10);
    d->metadataSetup      = new QPushButton(i18n("Settings..."), hbox22);
    d->expanderBox->insertItem(Private::MetadataSync, d->vbox2, QIcon::fromTheme(QLatin1String("run-build-file")),
                               i18n("Sync Metadata and Database"), QLatin1String("MetadataSync"), false);
    d->expanderBox->setCheckBoxVisible(Private::MetadataSync, true);
    d->expanderBox->insertStretch(Private::Stretch);

    // --------------------------------------------------------------------------------------

    grid->addWidget(d->logo,                        0, 0, 1, 1);
    grid->addWidget(d->title,                       0, 1, 1, 1);
    grid->addWidget(d->expanderBox,                 5, 0, 3, 2);
    grid->setSpacing(style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
    grid->setContentsMargins(QMargins());
    grid->setColumnStretch(1, 10);
    grid->setRowStretch(5, 10);

    QVBoxLayout* const vbx = new QVBoxLayout(this);
    vbx->addWidget(main);
    vbx->addWidget(d->buttons);
    setLayout(vbx);

    // --------------------------------------------------------------------------------------

    connect(d->buttons->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
            this, SLOT(slotOk()));

    connect(d->buttons->button(QDialogButtonBox::Cancel), SIGNAL(clicked()),
            this, SLOT(reject()));

    connect(d->buttons->button(QDialogButtonBox::Help), SIGNAL(clicked()),
            this, SLOT(slotHelp()));

    connect(d->expanderBox, SIGNAL(signalItemToggled(int,bool)),
            this, SLOT(slotItemToggled(int,bool)));

    connect(d->metadataSetup, SIGNAL(clicked()),
            this, SLOT(slotMetadataSetup()));

    connect(d->qualitySetup, SIGNAL(clicked()),
            this, SLOT(slotQualitySetup()));

    connect(d->minSimilarity, SIGNAL(valueChanged(int)),
            this,SLOT(slotMinSimilarityChanged(int)));

    // --------------------------------------------------------------------------------------

    readSettings();
}

MaintenanceDlg::~MaintenanceDlg()
{
    delete d;
}

void MaintenanceDlg::slotMinSimilarityChanged(int newValue)
{
    // Set the new minimum value of the maximum similarity
    d->maxSimilarity->setRange(newValue, 100, 1);
    // If the new value of the mimimum is now higher than the maximum similarity,
    // set the maximum similarity to the new value.
    if (newValue > d->maxSimilarity->value())
    {
        d->maxSimilarity->setValue(d->minSimilarity->value());
    }
}

void MaintenanceDlg::slotOk()
{
    writeSettings();
    accept();
}

MaintenanceSettings MaintenanceDlg::settings() const
{
    MaintenanceSettings prm;
    prm.wholeAlbums                         = d->albumSelectors->wholeAlbumsCollection();
    prm.wholeTags                           = d->albumSelectors->wholeTagsCollection();
    prm.albums                              = d->albumSelectors->selectedPAlbums();
    prm.tags                                = d->albumSelectors->selectedTAlbums();
    prm.useMutiCoreCPU                      = d->useMutiCoreCPU->isChecked();
    prm.newItems                            = d->expanderBox->isChecked(Private::NewItems);
    prm.thumbnails                          = d->expanderBox->isChecked(Private::Thumbnails);
    prm.scanThumbs                          = d->scanThumbs->isChecked();
    prm.fingerPrints                        = d->expanderBox->isChecked(Private::FingerPrints);
    prm.scanFingerPrints                    = d->scanFingerPrints->isChecked();
    prm.duplicates                          = d->expanderBox->isChecked(Private::Duplicates);
    prm.minSimilarity                       = d->minSimilarity->value();
    prm.maxSimilarity                       = d->maxSimilarity->value();
    prm.faceManagement                      = d->expanderBox->isChecked(Private::FaceManagement);
    prm.faceSettings.alreadyScannedHandling = (FaceScanSettings::AlreadyScannedHandling)d->faceScannedHandling->itemData(d->faceScannedHandling->currentIndex()).toInt();
    prm.faceSettings.albums                 = d->albumSelectors->selectedAlbums();
    prm.qualitySort                         = d->expanderBox->isChecked(Private::ImageQualitySorter);
    prm.qualityScanMode                     = d->qualityScanMode->itemData(d->qualityScanMode->currentIndex()).toInt();
    ImageQualitySettings imgq;
    imgq.readFromConfig();
    prm.quality                             = imgq;
    prm.metadataSync                        = d->expanderBox->isChecked(Private::MetadataSync);
    prm.syncDirection                       = d->syncDirection->itemData(d->syncDirection->currentIndex()).toInt();
    return prm;
}

void MaintenanceDlg::readSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(d->configGroupName);
    d->expanderBox->readSettings(group);
    d->albumSelectors->loadState();

    MaintenanceSettings prm;

    d->useMutiCoreCPU->setChecked(group.readEntry(d->configUseMutiCoreCPU,                               prm.useMutiCoreCPU));
    d->expanderBox->setChecked(Private::NewItems,           group.readEntry(d->configNewItems,           prm.newItems));
    d->expanderBox->setChecked(Private::Thumbnails,         group.readEntry(d->configThumbnails,         prm.thumbnails));
    d->scanThumbs->setChecked(group.readEntry(d->configScanThumbs,                                       prm.scanThumbs));
    d->expanderBox->setChecked(Private::FingerPrints,       group.readEntry(d->configFingerPrints,       prm.fingerPrints));
    d->scanFingerPrints->setChecked(group.readEntry(d->configScanFingerPrints,                           prm.scanFingerPrints));
    d->expanderBox->setChecked(Private::Duplicates,         group.readEntry(d->configDuplicates,         prm.duplicates));
    d->minSimilarity->setValue(group.readEntry(d->configMinSimilarity,                                   prm.minSimilarity));
    d->maxSimilarity->setValue(group.readEntry(d->configMaxSimilarity,                                   prm.maxSimilarity));
    d->expanderBox->setChecked(Private::FaceManagement,     group.readEntry(d->configFaceManagement,     prm.faceManagement));
    d->faceScannedHandling->setCurrentIndex(group.readEntry(d->configFaceScannedHandling,                (int)prm.faceSettings.alreadyScannedHandling));
    d->expanderBox->setChecked(Private::ImageQualitySorter, group.readEntry(d->configImageQualitySorter, prm.qualitySort));
    d->qualityScanMode->setCurrentIndex(group.readEntry(d->configQualityScanMode,                        prm.qualityScanMode));
    d->expanderBox->setChecked(Private::MetadataSync,       group.readEntry(d->configMetadataSync,       prm.metadataSync));
    d->syncDirection->setCurrentIndex(group.readEntry(d->configSyncDirection,                            prm.syncDirection));

    for (int i = Private::NewItems ; i < Private::Stretch ; ++i)
    {
        slotItemToggled(i, d->expanderBox->isChecked(i));
    }

    winId();
    DXmlGuiWindow::restoreWindowSize(windowHandle(), group);
    resize(windowHandle()->size());
}

void MaintenanceDlg::writeSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(d->configGroupName);
    d->expanderBox->writeSettings(group);
    d->albumSelectors->saveState();

    MaintenanceSettings prm   = settings();

    group.writeEntry(d->configUseMutiCoreCPU,      prm.useMutiCoreCPU);
    group.writeEntry(d->configNewItems,            prm.newItems);
    group.writeEntry(d->configThumbnails,          prm.thumbnails);
    group.writeEntry(d->configScanThumbs,          prm.scanThumbs);
    group.writeEntry(d->configFingerPrints,        prm.fingerPrints);
    group.writeEntry(d->configScanFingerPrints,    prm.scanFingerPrints);
    group.writeEntry(d->configDuplicates,          prm.duplicates);
    group.writeEntry(d->configMinSimilarity,       prm.minSimilarity);
    group.writeEntry(d->configMaxSimilarity,       prm.maxSimilarity);
    group.writeEntry(d->configFaceManagement,      prm.faceManagement);
    group.writeEntry(d->configFaceScannedHandling, (int)prm.faceSettings.alreadyScannedHandling);
    group.writeEntry(d->configImageQualitySorter,  prm.qualitySort);
    group.writeEntry(d->configQualityScanMode,     prm.qualityScanMode);
    group.writeEntry(d->configMetadataSync,        prm.metadataSync);
    group.writeEntry(d->configSyncDirection,       prm.syncDirection);

    DXmlGuiWindow::saveWindowSize(windowHandle(), group);
}

void MaintenanceDlg::slotItemToggled(int index, bool b)
{
    switch (index)
    {
        case Private::Thumbnails:
            d->scanThumbs->setEnabled(b);
            break;

        case Private::FingerPrints:
            d->scanFingerPrints->setEnabled(b);
            break;

        case Private::Duplicates:
            d->hbox->setEnabled(b);
            break;

        case Private::FaceManagement:
            d->hbox3->setEnabled(b);
            break;

        case Private::ImageQualitySorter:
            d->vbox->setEnabled(b);
            break;

        case Private::MetadataSync:
            d->vbox2->setEnabled(b);
            break;

        default :  // NewItems
            break;
    }
}

void MaintenanceDlg::slotMetadataSetup()
{
    Setup::execSinglePage(this, Setup::MetadataPage);
}

void MaintenanceDlg::slotQualitySetup()
{
    Setup::execSinglePage(this, Setup::ImageQualityPage);
}

void MaintenanceDlg::slotHelp()
{
    DXmlGuiWindow::openHandbook(QLatin1String("digikam"));
}

}  // namespace Digikam
