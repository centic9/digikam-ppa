/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2003-03-09
 * Description : Captions, Tags, and Rating properties editor
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2003-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2009-2011 by Andi Clemens <andi dot clemens at gmail dot com>
 * Copyright (C) 2009-2011 by Johannes Wienke <languitar at semipol dot de>
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

#include "imagedescedittab.moc"

// Qt includes

#include <QTextEdit>
#include <QStyle>
#include <QGridLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QSignalMapper>
#include <QTimer>
#include <QToolButton>

// KDE includes

#include <kmenu.h>
#include <kapplication.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kdialog.h>
#include <ktabwidget.h>
#include <kdebug.h>

// Libkexiv2 includes

#include <libkexiv2/version.h>
#include <libkexiv2/altlangstredit.h>

// Libkdcraw includes

#include <libkdcraw/version.h>

// Local includes

#include "captionedit.h"
#include "ddatetimeedit.h"
#include "addtagslineedit.h"
#include "albumsettings.h"
#include "albumthumbnailloader.h"
#include "collectionscanner.h"
#include "databasetransaction.h"
#include "fileactionmngr.h"
#include "ratingwidget.h"
#include "scancontroller.h"
#include "tagcheckview.h"
#include "templateselector.h"
#include "templateviewer.h"
#include "imageattributeswatch.h"
#include "statusprogressbar.h"
#include "tagmodificationhelper.h"
#include "template.h"
#include "imageinfolist.h"
#include "imageinfo.h"
#include "colorlabelwidget.h"
#include "picklabelwidget.h"
#include "fileactionprogress.h"
#include "searchtextbar.h"
#include "addtagslineedit.h"

namespace Digikam
{

class ImageDescEditTab::Private
{

public:

    enum DescEditTab
    {
        DESCRIPTIONS=0,
        TAGS,
        INFOS
    };

    Private()
    {
        modified                   = false;
        ignoreImageAttributesWatch = false;
        ignoreTagChanges           = false;
        togglingSearchSettings     = false;
        recentTagsBtn              = 0;
        titleEdit                  = 0;
        captionsEdit               = 0;
        tagsSearchBar              = 0;
        dateTimeEdit               = 0;
        tagCheckView               = 0;
        ratingWidget               = 0;
        assignedTagsBtn            = 0;
        applyBtn                   = 0;
        moreButton                 = 0;
        revertBtn                  = 0;
        moreMenu                   = 0;
        applyToAllVersionsButton   = 0;
        recentTagsMapper           = 0;
        newTagEdit                 = 0;
        lastSelectedWidget         = 0;
        templateSelector           = 0;
        templateViewer             = 0;
        tabWidget                  = 0;
        tagModel                   = 0;
        tagCheckView               = 0;
        colorLabelSelector         = 0;
        pickLabelSelector          = 0;
        metadataChangeTimer        = 0;
    }

    bool                 modified;
    bool                 ignoreImageAttributesWatch;
    bool                 ignoreTagChanges;
    bool                 togglingSearchSettings;

    QToolButton*         recentTagsBtn;
    QToolButton*         assignedTagsBtn;
    QToolButton*         revertBtn;

    KMenu*               moreMenu;

    QSignalMapper*       recentTagsMapper;

    QPushButton*         applyBtn;
    QPushButton*         moreButton;
    QPushButton*         applyToAllVersionsButton;

    QWidget*             lastSelectedWidget;

    AltLangStrEdit*      titleEdit;

    CaptionEdit*         captionsEdit;

    DDateTimeEdit*       dateTimeEdit;

    KTabWidget*          tabWidget;

    SearchTextBar*       tagsSearchBar;
    AddTagsLineEdit*     newTagEdit;

    ImageInfoList        currInfos;

    TagCheckView*        tagCheckView;

    TemplateSelector*    templateSelector;
    TemplateViewer*      templateViewer;

    RatingWidget*        ratingWidget;
    ColorLabelSelector*  colorLabelSelector;
    PickLabelSelector*   pickLabelSelector;

    MetadataHubOnTheRoad hub;

    TagModel*            tagModel;

    QTimer*              metadataChangeTimer;
    QList<int>           metadataChangeIds;
};

ImageDescEditTab::ImageDescEditTab(QWidget* const parent)
    : KVBox(parent), d(new Private)
{
    setMargin(0);
    setSpacing(KDialog::spacingHint());
    d->tabWidget           = new KTabWidget(this);

    d->metadataChangeTimer = new QTimer(this);
    d->metadataChangeTimer->setSingleShot(true);
    d->metadataChangeTimer->setInterval(250);

    // Captions/Date/Rating view -----------------------------------

    QScrollArea* sv        = new QScrollArea(d->tabWidget);
    sv->setFrameStyle(QFrame::NoFrame);
    sv->setWidgetResizable(true);

    QWidget* captionTagsArea = new QWidget(sv->viewport());
    QGridLayout* grid1       = new QGridLayout(captionTagsArea);
    sv->setWidget(captionTagsArea);
    sv->viewport()->setAutoFillBackground(false);
    captionTagsArea->setAutoFillBackground(false);

    d->titleEdit    = new AltLangStrEdit(captionTagsArea);
    d->titleEdit->setTitle(i18n("Title:"));
    d->titleEdit->setClickMessage(i18n("Enter title here."));

    d->captionsEdit = new CaptionEdit(captionTagsArea);

    KHBox* dateBox  = new KHBox(captionTagsArea);
    new QLabel(i18n("Date:"), dateBox);
    d->dateTimeEdit = new DDateTimeEdit(dateBox, "datepicker");

    KHBox* pickBox       = new KHBox(captionTagsArea);
    new QLabel(i18n("Pick Label:"), pickBox);
    d->pickLabelSelector = new PickLabelSelector(pickBox);
    pickBox->layout()->setAlignment(d->pickLabelSelector, Qt::AlignVCenter|Qt::AlignRight);

    KHBox* colorBox       = new KHBox(captionTagsArea);
    new QLabel(i18n("Color Label:"), colorBox);
    d->colorLabelSelector = new ColorLabelSelector(colorBox);
    colorBox->layout()->setAlignment(d->colorLabelSelector, Qt::AlignVCenter|Qt::AlignRight);

    KHBox* rateBox  = new KHBox(captionTagsArea);
    new QLabel(i18n("Rating:"), rateBox);
    d->ratingWidget = new RatingWidget(rateBox);
    rateBox->layout()->setAlignment(d->ratingWidget, Qt::AlignVCenter|Qt::AlignRight);

    // Buttons -----------------------------------------

    KHBox* applyButtonBox = new KHBox(this);
    applyButtonBox->setSpacing(KDialog::spacingHint());

    d->applyBtn           = new QPushButton(i18n("Apply"), applyButtonBox);
    d->applyBtn->setIcon(SmallIcon("dialog-ok-apply"));
    d->applyBtn->setEnabled(false);
    d->applyBtn->setToolTip( i18n("Apply all changes to images"));
    //buttonsBox->setStretchFactor(d->applyBtn, 10);

    KHBox* buttonsBox     = new KHBox(this);
    buttonsBox->setSpacing(KDialog::spacingHint());

    d->revertBtn          = new QToolButton(buttonsBox);
    d->revertBtn->setIcon(SmallIcon("document-revert"));
    d->revertBtn->setToolTip( i18n("Revert all changes"));
    d->revertBtn->setEnabled(false);

    d->applyToAllVersionsButton = new QPushButton(i18n("Apply to all versions"), buttonsBox);
    d->applyToAllVersionsButton->setIcon(SmallIcon("dialog-ok-apply"));
    d->applyToAllVersionsButton->setEnabled(false);
    d->applyToAllVersionsButton->setToolTip(i18n("Apply all changes to all versions of this image"));

    d->moreButton = new QPushButton(i18n("More"), buttonsBox);
    d->moreMenu   = new KMenu(captionTagsArea);
    d->moreButton->setMenu(d->moreMenu);

    // --------------------------------------------------

    grid1->addWidget(d->titleEdit,    0, 0, 1, 2);
    grid1->addWidget(d->captionsEdit, 1, 0, 1, 2);
    grid1->addWidget(dateBox,         2, 0, 1, 2);
    grid1->addWidget(pickBox,         3, 0, 1, 2);
    grid1->addWidget(colorBox,        4, 0, 1, 2);
    grid1->addWidget(rateBox,         5, 0, 1, 2);
    grid1->setRowStretch(1, 10);
    grid1->setMargin(KDialog::spacingHint());
    grid1->setSpacing(KDialog::spacingHint());

    d->tabWidget->insertTab(Private::DESCRIPTIONS, sv, i18n("Description"));

    // Tags view ---------------------------------------------------

    QScrollArea* sv3    = new QScrollArea(d->tabWidget);
    sv3->setFrameStyle(QFrame::NoFrame);
    sv3->setWidgetResizable(true);

    QWidget* tagsArea   = new QWidget(sv3->viewport());
    QGridLayout* grid3  = new QGridLayout(tagsArea);
    sv3->setWidget(tagsArea);
    sv3->viewport()->setAutoFillBackground(false);
    tagsArea->setAutoFillBackground(false);

    d->tagModel     = new TagModel(AbstractAlbumModel::IncludeRootAlbum, this);
    d->tagModel->setCheckable(true);
    d->tagModel->setRootCheckable(false);
    d->tagCheckView = new TagCheckView(tagsArea, d->tagModel);
    d->tagCheckView->setCheckNewTags(true);

    d->newTagEdit   = new AddTagsLineEdit(tagsArea);
    d->newTagEdit->setModel(d->tagModel);
    d->newTagEdit->setTagTreeView(d->tagCheckView);
    //, "ImageDescEditTabNewTagEdit",
    //d->newTagEdit->setCaseSensitive(false);
    d->newTagEdit->setClickMessage(i18n("Enter new tag here..."));
    //d->newTagEdit->setWhatsThis(i18n("Enter the text used to create new tags here. "
    //                                 "'/' can be used to create a hierarchy of tags. "
    //                                 "',' can be used to create more than one hierarchy at the same time."));

    KHBox* tagsSearch  = new KHBox(tagsArea);
    tagsSearch->setSpacing(KDialog::spacingHint());

    d->tagsSearchBar   = new SearchTextBar(tagsSearch, "ImageDescEditTabTagsSearchBar");
    d->tagsSearchBar->setModel(d->tagCheckView->filteredModel(),
                               AbstractAlbumModel::AlbumIdRole, AbstractAlbumModel::AlbumTitleRole);
    d->tagsSearchBar->setFilterModel(d->tagCheckView->albumFilterModel());

    d->assignedTagsBtn = new QToolButton(tagsSearch);
    d->assignedTagsBtn->setToolTip( i18n("Tags already assigned"));
    d->assignedTagsBtn->setIcon(KIconLoader::global()->loadIcon("tag-assigned",
                                KIconLoader::NoGroup, KIconLoader::SizeSmall));
    d->assignedTagsBtn->setCheckable(true);

    d->recentTagsBtn      = new QToolButton(tagsSearch);
    KMenu* recentTagsMenu = new KMenu(d->recentTagsBtn);
    d->recentTagsBtn->setToolTip( i18n("Recent Tags"));
    d->recentTagsBtn->setIcon(KIconLoader::global()->loadIcon("tag-recents",
                              KIconLoader::NoGroup, KIconLoader::SizeSmall));
    d->recentTagsBtn->setIconSize(QSize(KIconLoader::SizeSmall, KIconLoader::SizeSmall));
    d->recentTagsBtn->setMenu(recentTagsMenu);
    d->recentTagsBtn->setPopupMode(QToolButton::InstantPopup);
    d->recentTagsMapper   = new QSignalMapper(this);

    grid3->addWidget(d->newTagEdit,   0, 0, 1, 2);
    grid3->addWidget(d->tagCheckView, 1, 0, 1, 2);
    grid3->addWidget(tagsSearch,      2, 0, 1, 2);
    grid3->setRowStretch(1, 10);

    d->tabWidget->insertTab(Private::TAGS, sv3, i18n("Tags"));

    // Information Managament View --------------------------------------

    QScrollArea* sv2    = new QScrollArea(d->tabWidget);
    sv2->setFrameStyle(QFrame::NoFrame);
    sv2->setWidgetResizable(true);

    QWidget* infoArea   = new QWidget(sv2->viewport());
    QGridLayout* grid2  = new QGridLayout(infoArea);
    sv2->setWidget(infoArea);
    sv2->viewport()->setAutoFillBackground(false);
    infoArea->setAutoFillBackground(false);

    d->templateSelector = new TemplateSelector(infoArea);
    d->templateViewer   = new TemplateViewer(infoArea);
    d->templateViewer->setObjectName("ImageDescEditTab Expander");

    grid2->addWidget(d->templateSelector, 0, 0, 1, 2);
    grid2->addWidget(d->templateViewer,   1, 0, 1, 2);
    grid2->setRowStretch(1, 10);
    grid2->setMargin(KDialog::spacingHint());
    grid2->setSpacing(KDialog::spacingHint());

    d->tabWidget->insertTab(Private::INFOS, sv2, i18n("Information"));

    // --------------------------------------------------

    connect(d->tagCheckView->checkableModel(), SIGNAL(checkStateChanged(Album*,Qt::CheckState)),
            this, SLOT(slotTagStateChanged(Album*,Qt::CheckState)));

    connect(d->titleEdit, SIGNAL(signalModified(QString,QString)),
            this, SLOT(slotTitleChanged()));

    connect(d->titleEdit, SIGNAL(signalValueAdded(QString,QString)),
            this, SLOT(slotTitleChanged()));

    connect(d->titleEdit, SIGNAL(signalValueDeleted(QString)),
            this, SLOT(slotTitleChanged()));

    connect(d->captionsEdit, SIGNAL(signalModified()),
            this, SLOT(slotCommentChanged()));

    connect(d->dateTimeEdit, SIGNAL(dateTimeChanged(QDateTime)),
            this, SLOT(slotDateTimeChanged(QDateTime)));

    connect(d->pickLabelSelector, SIGNAL(signalPickLabelChanged(int)),
            this, SLOT(slotPickLabelChanged(int)));

    connect(d->colorLabelSelector, SIGNAL(signalColorLabelChanged(int)),
            this, SLOT(slotColorLabelChanged(int)));

    connect(d->ratingWidget, SIGNAL(signalRatingChanged(int)),
            this, SLOT(slotRatingChanged(int)));

    connect(d->templateSelector, SIGNAL(signalTemplateSelected()),
            this, SLOT(slotTemplateSelected()));

    connect(d->tagsSearchBar, SIGNAL(signalSearchTextSettings(SearchTextSettings)),
            this, SLOT(slotTagsSearchChanged(SearchTextSettings)));

    connect(d->assignedTagsBtn, SIGNAL(toggled(bool)),
            this, SLOT(slotAssignedTagsToggled(bool)));

    connect(d->newTagEdit, SIGNAL(taggingActionActivated(TaggingAction)),
            this, SLOT(slotTaggingActionActivated(TaggingAction)));

    connect(d->applyBtn, SIGNAL(clicked()),
            this, SLOT(slotApplyAllChanges()));

    connect(d->applyToAllVersionsButton, SIGNAL(clicked()),
            this, SLOT(slotApplyChangesToAllVersions()));

    connect(d->revertBtn, SIGNAL(clicked()),
            this, SLOT(slotRevertAllChanges()));

    connect(d->moreMenu, SIGNAL(aboutToShow()),
            this, SLOT(slotMoreMenu()));

    connect(d->recentTagsMapper, SIGNAL(mapped(int)),
            this, SLOT(slotRecentTagsMenuActivated(int)));

    connect(d->metadataChangeTimer, SIGNAL(timeout()),
            this, SLOT(slotReloadForMetadataChange()));

    connect(this, SIGNAL(askToApplyChanges(QList<ImageInfo>,MetadataHubOnTheRoad*)),
            this, SLOT(slotAskToApplyChanges(QList<ImageInfo>,MetadataHubOnTheRoad*)),
            Qt::QueuedConnection
           );

    // Initialize ---------------------------------------------

    d->titleEdit->installEventFilter(this);
    d->captionsEdit->installEventFilter(this);
    d->dateTimeEdit->installEventFilter(this);
    d->pickLabelSelector->installEventFilter(this);
    d->colorLabelSelector->installEventFilter(this);
    d->ratingWidget->installEventFilter(this);
    // TODO update, what does this filter?
    d->tagCheckView->installEventFilter(this);
    updateRecentTags();

    // Connect to attribute watch ------------------------------

    ImageAttributesWatch* const watch = ImageAttributesWatch::instance();

    connect(watch, SIGNAL(signalImageTagsChanged(qlonglong)),
            this, SLOT(slotImageTagsChanged(qlonglong)));

    connect(watch, SIGNAL(signalImagesChanged(int)),
            this, SLOT(slotImagesChanged(int)));

    connect(watch, SIGNAL(signalImageRatingChanged(qlonglong)),
            this, SLOT(slotImageRatingChanged(qlonglong)));

    connect(watch, SIGNAL(signalImageDateChanged(qlonglong)),
            this, SLOT(slotImageDateChanged(qlonglong)));

    connect(watch, SIGNAL(signalImageCaptionChanged(qlonglong)),
            this, SLOT(slotImageCaptionChanged(qlonglong)));
}

ImageDescEditTab::~ImageDescEditTab()
{
    delete d;
}

void ImageDescEditTab::readSettings(KConfigGroup& group)
{
    d->tabWidget->setCurrentIndex(group.readEntry("ImageDescEdit Tab", (int)Private::DESCRIPTIONS));
#if KEXIV2_VERSION >= 0x020101
    d->titleEdit->setCurrentLanguageCode(group.readEntry("ImageDescEditTab TitleLang", QString()));
#endif
    d->captionsEdit->setCurrentLanguageCode(group.readEntry("ImageDescEditTab CaptionsLang", QString()));

#if KDCRAW_VERSION >= 0x020000
    d->templateViewer->readSettings(group);
#else
    d->templateViewer->readSettings();
#endif

    d->tagCheckView->setConfigGroup(group);
    d->tagCheckView->setEntryPrefix("ImageDescEditTab TagCheckView");
    d->tagCheckView->loadState();
    d->tagsSearchBar->setConfigGroup(group);
    d->tagsSearchBar->setEntryPrefix("ImageDescEditTab SearchBar");
    d->tagsSearchBar->loadState();
}

void ImageDescEditTab::writeSettings(KConfigGroup& group)
{
    group.writeEntry("ImageDescEdit Tab",             d->tabWidget->currentIndex());
    group.writeEntry("ImageDescEditTab TitleLang",    d->titleEdit->currentLanguageCode());
    group.writeEntry("ImageDescEditTab CaptionsLang", d->captionsEdit->currentLanguageCode());

#if KDCRAW_VERSION >= 0x020000
    d->templateViewer->writeSettings(group);
#else
    d->templateViewer->writeSettings();
#endif

    d->tagCheckView->saveState();
    d->tagsSearchBar->saveState();
}

void ImageDescEditTab::setFocusToNewTagEdit()
{
    //select "Tags" tab and focus the NewTagLineEdit widget
    d->tabWidget->setCurrentIndex(Private::TAGS);
    d->newTagEdit->setFocus();
}

bool ImageDescEditTab::singleSelection() const
{
    return (d->currInfos.count() == 1);
}

void ImageDescEditTab::slotChangingItems()
{
    if (!d->modified)
    {
        return;
    }

    if (d->currInfos.isEmpty())
    {
        return;
    }

    if (!AlbumSettings::instance()->getApplySidebarChangesDirectly())
    {
        // Open dialog via queued connection out-of-scope, see bug 302311
        emit askToApplyChanges(d->currInfos, new MetadataHubOnTheRoad(d->hub));
        reset();
    }
    else
    {
        slotApplyAllChanges();
    }
}

void ImageDescEditTab::slotAskToApplyChanges(const QList<ImageInfo>& infos, MetadataHubOnTheRoad* hub)
{
    KDialog* const dialog = new KDialog(this);

    dialog->setCaption(i18n("Apply changes?"));
    dialog->setButtons(KDialog::Yes | KDialog::No);
    dialog->setDefaultButton(KDialog::Yes);
    dialog->setEscapeButton(KDialog::No);
    dialog->setButtonGuiItem(KDialog::Yes, KStandardGuiItem::yes());
    dialog->setButtonGuiItem(KDialog::No,  KStandardGuiItem::discard());
    dialog->setModal(true);

    int changedFields = 0;

    if (hub->titlesChanged())
    {
        ++changedFields;
    }

    if (hub->commentsChanged())
    {
        ++changedFields;
    }

    if (hub->dateTimeChanged())
    {
        ++changedFields;
    }

    if (hub->ratingChanged())
    {
        ++changedFields;
    }

    if (hub->pickLabelChanged())
    {
        ++changedFields;
    }

    if (hub->colorLabelChanged())
    {
        ++changedFields;
    }

    if (hub->tagsChanged())
    {
        ++changedFields;
    }

    QString text;

    if (changedFields == 1)
    {
        if (hub->commentsChanged())
            text = i18np("You have edited the image caption. ",
                        "You have edited the captions of %1 images. ",
                        infos.count());
        else if (hub->titlesChanged())
            text = i18np("You have edited the image title. ",
                        "You have edited the titles of %1 images. ",
                        infos.count());
        else if (hub->dateTimeChanged())
            text = i18np("You have edited the date of the image. ",
                        "You have edited the date of %1 images. ",
                        infos.count());
        else if (hub->pickLabelChanged())
            text = i18np("You have edited the pick label of the image. ",
                        "You have edited the pick label of %1 images. ",
                        infos.count());
        else if (hub->colorLabelChanged())
            text = i18np("You have edited the color label of the image. ",
                        "You have edited the color label of %1 images. ",
                        infos.count());
        else if (hub->ratingChanged())
            text = i18np("You have edited the rating of the image. ",
                        "You have edited the rating of %1 images. ",
                        infos.count());
        else if (hub->tagsChanged())
            text = i18np("You have edited the tags of the image. ",
                        "You have edited the tags of %1 images. ",
                        infos.count());

        text += i18n("Do you want to apply your changes?");
    }
    else
    {
        text = i18np("<p>You have edited the metadata of the image: </p><p><ul>",
                    "<p>You have edited the metadata of %1 images: </p><p><ul>",
                    infos.count());

        if (hub->titlesChanged())
            text += i18n("<li>title</li>");
        if (hub->commentsChanged())
        {
            text += i18n("<li>caption</li>");
        }

        if (hub->dateTimeChanged())
        {
            text += i18n("<li>date</li>");
        }

        if (hub->pickLabelChanged())
        {
            text += i18n("<li>pick label</li>");
        }

        if (hub->colorLabelChanged())
        {
            text += i18n("<li>color label</li>");
        }

        if (hub->ratingChanged())
        {
            text += i18n("<li>rating</li>");
        }

        if (hub->tagsChanged())
        {
            text += i18n("<li>tags</li>");
        }

        text += "</ul></p>";

        text += i18n("<p>Do you want to apply your changes?</p>");
    }

    bool alwaysApply = false;
    int returnCode   = KMessageBox::createKMessageBox(dialog, QMessageBox::Information, text, QStringList(),
                                                      i18n("Always apply changes without confirmation"),
                                                      &alwaysApply, KMessageBox::Notify);

    if (alwaysApply)
    {
        AlbumSettings::instance()->setApplySidebarChangesDirectly(true);
    }

    if (returnCode == KDialog::No)
    {
        delete hub;
        return;
    }
    // otherwise apply:
    FileActionMngr::instance()->applyMetadata(infos, hub);
}

void ImageDescEditTab::reset()
{
    d->modified = false;
    d->hub.resetChanged();
    d->applyBtn->setEnabled(false);
    d->revertBtn->setEnabled(false);
    d->applyToAllVersionsButton->setEnabled(false);
}

void ImageDescEditTab::slotApplyAllChanges()
{
    if (!d->modified)
    {
        return;
    }

    if (d->currInfos.isEmpty())
    {
        return;
    }

    FileActionMngr::instance()->applyMetadata(d->currInfos, d->hub);
    reset();
}

void ImageDescEditTab::slotRevertAllChanges()
{
    if (!d->modified)
    {
        return;
    }

    if (d->currInfos.isEmpty())
    {
        return;
    }

    setInfos(d->currInfos);
}

void ImageDescEditTab::setItem(const ImageInfo& info)
{
    slotChangingItems();
    ImageInfoList list;

    if (!info.isNull())
    {
        list << info;
    }

    setInfos(list);
}

void ImageDescEditTab::setItems(const ImageInfoList& infos)
{
    slotChangingItems();
    setInfos(infos);
}

void ImageDescEditTab::setInfos(const ImageInfoList& infos)
{
    if (infos.isEmpty())
    {
        d->hub = MetadataHub();
        d->captionsEdit->blockSignals(true);
        d->captionsEdit->reset();
        d->captionsEdit->blockSignals(false);
        d->titleEdit->blockSignals(true);
        d->titleEdit->reset();
        d->titleEdit->blockSignals(false);
        d->currInfos.clear();
        resetMetadataChangeInfo();
        setEnabled(false);
        return;
    }

    setEnabled(true);
    d->currInfos = infos;
    d->modified  = false;
    resetMetadataChangeInfo();
    d->hub       = MetadataHub();
    d->applyBtn->setEnabled(false);
    d->revertBtn->setEnabled(false);

    foreach(const ImageInfo& info, d->currInfos)
    {
        d->hub.load(info);
    }

    updateComments();
    updatePickLabel();
    updateColorLabel();
    updateRating();
    updateDate();
    updateTemplate();
    updateTagsView();
    updateRecentTags();
    setFocusToLastSelectedWidget();
}

void ImageDescEditTab::slotReadFromFileMetadataToDatabase()
{
    initProgressIndicator();

    emit signalProgressMessageChanged(i18n("Reading metadata from files. Please wait..."));

    d->ignoreImageAttributesWatch = true;
    int i                         = 0;

    DatabaseTransaction transaction;
    ScanController::instance()->suspendCollectionScan();

    CollectionScanner scanner;

    foreach(const ImageInfo& info, d->currInfos)
    {
        scanner.scanFile(info, CollectionScanner::Rescan);

        /*
        // A batch operation: a hub for each single file, not the common hub
        MetadataHub fileHub(MetadataHub::NewTagsImport);
        // read in from DMetadata
        fileHub.load(info.filePath());
        // write out to database
        fileHub.write(info);
        */

        emit signalProgressValueChanged(i++/(float)d->currInfos.count());

        kapp->processEvents();
    }

    ScanController::instance()->resumeCollectionScan();
    d->ignoreImageAttributesWatch = false;

    emit signalProgressFinished();

    // reload everything
    setInfos(d->currInfos);
}

void ImageDescEditTab::slotWriteToFileMetadataFromDatabase()
{
    initProgressIndicator();

    emit signalProgressMessageChanged(i18n("Writing metadata to files. Please wait..."));

    int i = 0;

    foreach(const ImageInfo& info, d->currInfos)
    {
        MetadataHub fileHub;
        // read in from database
        fileHub.load(info);
        // write out to file DMetadata
        fileHub.write(info.filePath());

        emit signalProgressValueChanged(i++/(float)d->currInfos.count());
        kapp->processEvents();
    }

    emit signalProgressFinished();
}

bool ImageDescEditTab::eventFilter(QObject* o, QEvent* e)
{
    if ( e->type() == QEvent::KeyPress )
    {
        QKeyEvent* k = static_cast<QKeyEvent*>(e);

        if (k->key() == Qt::Key_Enter || k->key() == Qt::Key_Return)
        {
            if (k->modifiers() == Qt::ControlModifier)
            {
                d->lastSelectedWidget = qobject_cast<QWidget*>(o);
                emit signalNextItem();
                return true;
            }
            else if (k->modifiers() == Qt::ShiftModifier)
            {
                d->lastSelectedWidget = qobject_cast<QWidget*>(o);
                emit signalPrevItem();
                return true;
            }
        }
    }

    return KVBox::eventFilter(o, e);
}

void ImageDescEditTab::populateTags()
{
    // TODO update, this wont work... crashes
    //KConfigGroup group;
    //d->tagCheckView->loadViewState(group);
}

void ImageDescEditTab::slotTagStateChanged(Album* album, Qt::CheckState checkState)
{
    TAlbum* tag = dynamic_cast<TAlbum*> (album);

    if (!tag || d->ignoreTagChanges)
    {
        return;
    }

    bool isChecked = false;

    switch (checkState)
    {
        case Qt::Checked:
            isChecked = true;
            break;
        default:
            isChecked = false;
            break;
    }

    d->hub.setTag(tag->id(), isChecked);
    slotModified();
}

void ImageDescEditTab::slotCommentChanged()
{
    d->hub.setComments(d->captionsEdit->values());
    setMetadataWidgetStatus(d->hub.commentsStatus(), d->captionsEdit);
    slotModified();
}

void ImageDescEditTab::slotTitleChanged()
{
    CaptionsMap titles;

    titles.fromAltLangMap(d->titleEdit->values());
    d->hub.setTitles(titles);
    setMetadataWidgetStatus(d->hub.titlesStatus(), d->titleEdit);
    slotModified();
}

void ImageDescEditTab::slotDateTimeChanged(const QDateTime& dateTime)
{
    d->hub.setDateTime(dateTime);
    setMetadataWidgetStatus(d->hub.dateTimeStatus(), d->dateTimeEdit);
    slotModified();
}

void ImageDescEditTab::slotTemplateSelected()
{
    d->hub.setMetadataTemplate(d->templateSelector->getTemplate());
    d->templateViewer->setTemplate(d->templateSelector->getTemplate());
    setMetadataWidgetStatus(d->hub.templateStatus(), d->templateSelector);
    slotModified();
}

void ImageDescEditTab::slotPickLabelChanged(int pickId)
{
    d->hub.setPickLabel(pickId);
    // no handling for MetadataDisjoint needed for pick label,
    // we set it to 0 when disjoint, see below
    slotModified();
}

void ImageDescEditTab::slotColorLabelChanged(int colorId)
{
    d->hub.setColorLabel(colorId);
    // no handling for MetadataDisjoint needed for color label,
    // we set it to 0 when disjoint, see below
    slotModified();
}

void ImageDescEditTab::slotRatingChanged(int rating)
{
    d->hub.setRating(rating);
    // no handling for MetadataDisjoint needed for rating,
    // we set it to 0 when disjoint, see below
    slotModified();
}

void ImageDescEditTab::slotModified()
{
    d->modified = true;
    d->applyBtn->setEnabled(true);
    d->revertBtn->setEnabled(true);

    if(d->currInfos.size() == 1)
    {
        d->applyToAllVersionsButton->setEnabled(true);
    }
}

void ImageDescEditTab::slotCreateNewTag()
{
    if (d->newTagEdit->text().isEmpty())
    {
        return;
    }

    TAlbum* created = d->tagCheckView->tagModificationHelper()->
                      slotTagNew(d->tagCheckView->currentAlbum(), d->newTagEdit->text());

    if (created)
    {
        //d->tagCheckView->slotSelectAlbum(created);
        d->newTagEdit->clear();
    }
}

void ImageDescEditTab::slotTaggingActionActivated(const TaggingAction& action)
{
    TAlbum* assigned = 0;

    if (action.shallAssignTag())
    {
        assigned = AlbumManager::instance()->findTAlbum(action.tagId());

        if (assigned)
        {
            d->tagModel->setChecked(assigned, true);
        }
    }
    else if (action.shallCreateNewTag())
    {
        TAlbum* const parent = AlbumManager::instance()->findTAlbum(action.parentTagId());
        // tag is assigned automatically
        assigned = d->tagCheckView->tagModificationHelper()->slotTagNew(parent, action.newTagName());
    }

    if (assigned)
    {
        d->newTagEdit->clear();
        d->tagCheckView->scrollTo(d->tagCheckView->albumFilterModel()->indexForAlbum(assigned));
    }
}

void ImageDescEditTab::assignPickLabel(int pickId)
{
    d->pickLabelSelector->setPickLabel((PickLabel)pickId);
}

void ImageDescEditTab::assignColorLabel(int colorId)
{
    d->colorLabelSelector->setColorLabel((ColorLabel)colorId);
}

void ImageDescEditTab::assignRating(int rating)
{
    d->ratingWidget->setRating(rating);
}

void ImageDescEditTab::setTagState(TAlbum* const tag, MetadataHub::TagStatus status)
{
    if (!tag)
    {
        return;
    }

    switch (status.status)
    {
        case MetadataHub::MetadataDisjoint:
            d->tagModel->setCheckState(tag, Qt::PartiallyChecked);
            break;
        case MetadataHub::MetadataAvailable:
        case MetadataHub::MetadataInvalid:
            d->tagModel->setChecked(tag, status.hasTag);
            break;
        default:
            kWarning() << "Untreated tag status enum value " << status.status;
            d->tagModel->setCheckState(tag, Qt::PartiallyChecked);
            break;
    }
}

void ImageDescEditTab::initializeTags(QModelIndex& parent)
{
    TAlbum* tag = d->tagModel->albumForIndex(parent);

    if (!tag)
    {
        return;
    }

    setTagState(tag, d->hub.tagStatus(tag->id()));

    for (int row = 0; row < d->tagModel->rowCount(parent); ++row)
    {
        QModelIndex index = d->tagModel->index(row, 0, parent);
        initializeTags(index);
    }
}

void ImageDescEditTab::updateTagsView()
{
    // avoid that the automatic tag toggling handles these calls and
    // modification is indicated to this widget
    TagCheckView::ToggleAutoTags toggle = d->tagCheckView->getToggleAutoTags();
    d->tagCheckView->setToggleAutoTags(TagCheckView::NoToggleAuto);
    d->ignoreTagChanges                 = true;

    // first reset the tags completely
    d->tagModel->resetAllCheckedAlbums();

    // then update checked state for all tags of the currently selected images
    for (int row = 0; row < d->tagModel->rowCount(); ++row)
    {
        QModelIndex index = d->tagModel->index(row, 0);
        initializeTags(index);
    }

    d->ignoreTagChanges = false;
    d->tagCheckView->setToggleAutoTags(toggle);

    // The condition is a temporary fix not to destroy name filtering on image change.
    // See comments in these methods.
    if (d->assignedTagsBtn->isChecked())
    {
        slotAssignedTagsToggled(d->assignedTagsBtn->isChecked());
    }
}

void ImageDescEditTab::updateComments()
{
    d->captionsEdit->blockSignals(true);
    d->captionsEdit->setValues(d->hub.comments());
    setMetadataWidgetStatus(d->hub.commentsStatus(), d->captionsEdit);
    d->captionsEdit->blockSignals(false);

    d->titleEdit->blockSignals(true);
    d->titleEdit->setValues(d->hub.titles().toAltLangMap());
    setMetadataWidgetStatus(d->hub.titlesStatus(), d->titleEdit);
    d->titleEdit->blockSignals(false);
}

void ImageDescEditTab::updatePickLabel()
{
    d->pickLabelSelector->blockSignals(true);

    if (d->hub.pickLabelStatus() == MetadataHub::MetadataDisjoint)
    {
        d->pickLabelSelector->setPickLabel(NoPickLabel);
    }
    else
    {
        d->pickLabelSelector->setPickLabel((PickLabel)d->hub.pickLabel());
    }

    d->pickLabelSelector->blockSignals(false);
}

void ImageDescEditTab::updateColorLabel()
{
    d->colorLabelSelector->blockSignals(true);

    if (d->hub.colorLabelStatus() == MetadataHub::MetadataDisjoint)
    {
        d->colorLabelSelector->setColorLabel(NoColorLabel);
    }
    else
    {
        d->colorLabelSelector->setColorLabel((ColorLabel)d->hub.colorLabel());
    }

    d->colorLabelSelector->blockSignals(false);
}

void ImageDescEditTab::updateRating()
{
    d->ratingWidget->blockSignals(true);

    if (d->hub.ratingStatus() == MetadataHub::MetadataDisjoint)
    {
        d->ratingWidget->setRating(0);
    }
    else
    {
        d->ratingWidget->setRating(d->hub.rating());
    }

    d->ratingWidget->blockSignals(false);
}

void ImageDescEditTab::updateDate()
{
    d->dateTimeEdit->blockSignals(true);
    d->dateTimeEdit->setDateTime(d->hub.dateTime());
    setMetadataWidgetStatus(d->hub.dateTimeStatus(), d->dateTimeEdit);
    d->dateTimeEdit->blockSignals(false);
}

void ImageDescEditTab::updateTemplate()
{
    d->templateSelector->blockSignals(true);
    d->templateSelector->setTemplate(d->hub.metadataTemplate());
    d->templateViewer->setTemplate(d->hub.metadataTemplate());
    setMetadataWidgetStatus(d->hub.templateStatus(), d->templateSelector);
    d->templateSelector->blockSignals(false);
}

void ImageDescEditTab::setMetadataWidgetStatus(int status, QWidget* const widget)
{
    if (status == MetadataHub::MetadataDisjoint)
    {
        // For text widgets: Set text color to color of disabled text
        QPalette palette = widget->palette();
        palette.setColor(QPalette::Text, palette.color(QPalette::Disabled, QPalette::Text));
        widget->setPalette(palette);
    }
    else
    {
        widget->setPalette(QPalette());
    }
}

void ImageDescEditTab::slotMoreMenu()
{
    d->moreMenu->clear();

    if (singleSelection())
    {
        d->moreMenu->addAction(i18n("Read metadata from file to database"), this, SLOT(slotReadFromFileMetadataToDatabase()));
        QAction* const writeAction = d->moreMenu->addAction(i18n("Write metadata to each file"), this,
                                                            SLOT(slotWriteToFileMetadataFromDatabase()));
        // we do not need a "Write to file" action here because the apply button will do just that
        // if selection is a single file.
        // Adding the option will confuse users: Does the apply button not write to file?
        // Removing the option will confuse users: There is not option to write to file! (not visible in single selection)
        // Disabling will confuse users: Why is it disabled?
        writeAction->setEnabled(false);
    }
    else
    {
        // We need to make clear that this action is different from the Apply button,
        // which saves the same changes to all files. These batch operations operate on each single file.
        d->moreMenu->addAction(i18n("Read metadata from each file to database"), this, SLOT(slotReadFromFileMetadataToDatabase()));
        d->moreMenu->addAction(i18n("Write metadata to each file"), this, SLOT(slotWriteToFileMetadataFromDatabase()));
    }
}

void ImageDescEditTab::slotImageTagsChanged(qlonglong imageId)
{
    // don't lose modifications
    if (d->ignoreImageAttributesWatch || d->modified)
    {
        return;
    }

    metadataChange(imageId);
}

void ImageDescEditTab::slotImagesChanged(int albumId)
{
    if (d->ignoreImageAttributesWatch || d->modified)
    {
        return;
    }

    Album* const a = AlbumManager::instance()->findAlbum(albumId);

    if (d->currInfos.isEmpty() || !a || a->isRoot() || a->type() != Album::TAG)
    {
        return;
    }

    setInfos(d->currInfos);
}

void ImageDescEditTab::slotImageRatingChanged(qlonglong imageId)
{
    if (d->ignoreImageAttributesWatch || d->modified)
    {
        return;
    }
    metadataChange(imageId);
}

void ImageDescEditTab::slotImageCaptionChanged(qlonglong imageId)
{
    if (d->ignoreImageAttributesWatch || d->modified)
    {
        return;
    }

    metadataChange(imageId);
}

void ImageDescEditTab::slotImageDateChanged(qlonglong imageId)
{
    if (d->ignoreImageAttributesWatch || d->modified)
    {
        return;
    }

    metadataChange(imageId);
}

// private common code for above methods
void ImageDescEditTab::metadataChange(qlonglong imageId)
{
    d->metadataChangeIds << imageId;
    d->metadataChangeTimer->start();
}

void ImageDescEditTab::resetMetadataChangeInfo()
{
    d->metadataChangeTimer->stop();
    d->metadataChangeIds.clear();
}

void ImageDescEditTab::slotReloadForMetadataChange()
{
    // NOTE: What to do if d->modified? Reloading is no option.
    // It may be a little change the user wants to ignore, or a large conflict.
    if (d->currInfos.isEmpty() || d->modified)
    {
        resetMetadataChangeInfo();
        return;
    }

    if (singleSelection())
    {
        if (d->metadataChangeIds.contains(d->currInfos.first().id()))
        {
            setInfos(d->currInfos);
        }
    }
    else
    {
        // if image id is in our list, update
        foreach(const ImageInfo& info, d->currInfos)
        {
            if (d->metadataChangeIds.contains(info.id()))
            {
                setInfos(d->currInfos);
                break;
            }
        }
    }
}

void ImageDescEditTab::updateRecentTags()
{
    KMenu* const menu = dynamic_cast<KMenu*>(d->recentTagsBtn->menu());

    if (!menu)
    {
        return;
    }

    menu->clear();

    AlbumList recentTags = AlbumManager::instance()->getRecentlyAssignedTags();

    if (recentTags.isEmpty())
    {
        QAction* const noTagsAction = menu->addAction(i18n("No Recently Assigned Tags"));
        noTagsAction->setEnabled(false);
    }
    else
    {
        for (AlbumList::const_iterator it = recentTags.constBegin();
             it != recentTags.constEnd(); ++it)
        {
            TAlbum* const album = static_cast<TAlbum*>(*it);

            if (album)
            {
                AlbumThumbnailLoader* const loader = AlbumThumbnailLoader::instance();
                QPixmap                     icon;

                if (!loader->getTagThumbnail(album, icon))
                {
                    if (icon.isNull())
                    {
                        icon = loader->getStandardTagIcon(album, AlbumThumbnailLoader::SmallerSize);
                    }
                }

                TAlbum* const parent = dynamic_cast<TAlbum*> (album->parent());

                if (parent)
                {
                    QString text          = album->title() + " (" + parent->prettyUrl() + ')';
                    QAction* const action = menu->addAction(icon, text, d->recentTagsMapper, SLOT(map()));
                    d->recentTagsMapper->setMapping(action, album->id());
                }
                else
                {
                    kError() << "Tag" << album << "doesn't have a valid parent";
                }
            }
        }
    }
}

void ImageDescEditTab::slotRecentTagsMenuActivated(int id)
{
    AlbumManager* const albumMan = AlbumManager::instance();

    if (id > 0)
    {
        TAlbum* const album = albumMan->findTAlbum(id);

        if (album)
        {
            d->tagModel->setChecked(album, true);
        }
    }
}

void ImageDescEditTab::slotTagsSearchChanged(const SearchTextSettings& settings)
{
    Q_UNUSED(settings);

    // if we filter, we should reset the assignedTagsBtn again
    if (d->assignedTagsBtn->isChecked() && !d->togglingSearchSettings)
    {
        d->togglingSearchSettings = true;
        d->assignedTagsBtn->setChecked(false);
        d->togglingSearchSettings = false;
    }
}

void ImageDescEditTab::slotAssignedTagsToggled(bool t)
{
    d->tagCheckView->checkableAlbumFilterModel()->setFilterChecked(t);
    d->tagCheckView->checkableAlbumFilterModel()->setFilterPartiallyChecked(t);
    d->tagCheckView->checkableAlbumFilterModel()->setFilterBehavior(t ? AlbumFilterModel::StrictFiltering : AlbumFilterModel::FullFiltering);

    if (t)
    {
        // if we filter by assigned, we should initially clear the normal search
        if (!d->togglingSearchSettings)
        {
            d->togglingSearchSettings = true;
            d->tagsSearchBar->clear();
            d->togglingSearchSettings = false;
        }

        // Only after above change, do this
        d->tagCheckView->expandMatches(d->tagCheckView->rootIndex());
   }
}

void ImageDescEditTab::setFocusToLastSelectedWidget()
{
    if (d->lastSelectedWidget)
    {
        d->lastSelectedWidget->setFocus();
    }

    d->lastSelectedWidget = 0;
}

void ImageDescEditTab::setFocusToTagsView()
{
    d->lastSelectedWidget = qobject_cast<QWidget*>(d->tagCheckView);
    d->tagCheckView->setFocus();
}

void ImageDescEditTab::slotApplyChangesToAllVersions()
{
    if (!d->modified)
    {
        return;
    }

    if (d->currInfos.isEmpty())
    {
        return;
    }

    QSet<qlonglong>                     tmpSet;
    QList<QPair<qlonglong, qlonglong> > relations;

    foreach(const ImageInfo& info, d->currInfos)
    {
        // Collect all ids in all image's relations
        relations.append(info.relationCloud());
    }

    for(int i = 0; i < relations.size(); ++i)
    {
        // Use QSet to prevent duplicates
        tmpSet.insert(relations.at(i).first);
        tmpSet.insert(relations.at(i).second);
    }

    FileActionMngr::instance()->applyMetadata(ImageInfoList(tmpSet.toList()), d->hub);

    d->modified = false;
    d->hub.resetChanged();
    d->applyBtn->setEnabled(false);
    d->revertBtn->setEnabled(false);
    d->applyToAllVersionsButton->setEnabled(false);
}

void ImageDescEditTab::initProgressIndicator()
{
    if (!ProgressManager::instance()->findItembyId("ImageDescEditTabProgress"))
    {
        FileActionProgress* const item = new FileActionProgress("ImageDescEditTabProgress");

        connect(this, SIGNAL(signalProgressMessageChanged(QString)),
                item, SLOT(slotProgressStatus(QString)));

        connect(this, SIGNAL(signalProgressValueChanged(float)),
                item, SLOT(slotProgressValue(float)));

        connect(this, SIGNAL(signalProgressFinished()),
                item, SLOT(slotCompleted()));
    }
}

AddTagsLineEdit* ImageDescEditTab::getNewTagEdit()
{
    return d->newTagEdit;
}
}  // namespace Digikam
