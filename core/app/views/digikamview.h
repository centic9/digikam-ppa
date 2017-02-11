/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2002-16-10
 * Description : implementation of album view interface.
 *
 * Copyright (C) 2002-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2002-2017 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009-2011 by Johannes Wienke <languitar at semipol dot de>
 * Copyright (C) 2010-2011 by Andi Clemens <andi dot clemens at gmail dot com>
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

#ifndef DIGIKAMVIEW_H
#define DIGIKAMVIEW_H

// Qt includes

#include <QStringList>
#include <QUrl>

// Local includes

#include "metaengine_rotation.h"
#include "digikam_config.h"
#include "searchtextbar.h"
#include "imageinfo.h"
#include "digikammodelcollection.h"
#include "sidebarwidget.h"
#include "stackedview.h"
#include "dwidgetutils.h"

namespace Digikam
{

class AlbumIconItem;
class ApplicationSettings;
class Album;
class PAlbum;
class TAlbum;
class BatchSyncMetadata;
class FilterStatusBar;
class SlideShowSettings;
class DCategorizedView;

class DigikamView : public DHBox
{
    Q_OBJECT

public:

    DigikamView(QWidget* const parent, DigikamModelCollection* const modelCollection);
    ~DigikamView();

    void applySettings();
    void refreshView();
    void clearHistory();
    void getForwardHistory(QStringList& titles);
    void getBackwardHistory(QStringList& titles);

    void showSideBars();
    void hideSideBars();
    void toggleLeftSidebar();
    void toggleRightSidebar();
    void previousLeftSideBarTab();
    void nextLeftSideBarTab();
    void previousRightSideBarTab();
    void nextRightSideBarTab();

    void setToolsIconView(DCategorizedView* const view);
    void setThumbSize(int size);
    void toggleShowBar(bool);
    void setRecurseAlbums(bool recursive);
    void setRecurseTags(bool recursive);
    void imageTransform(MetaEngineRotation::TransformationAction transform);

    void connectIconViewFilter(FilterStatusBar* const filter);

    QList<QUrl> allUrls()      const;
    QList<QUrl> selectedUrls() const;
    QUrl currentUrl()          const;
    bool hasCurrentItem()      const;
    ImageInfo currentInfo()    const;

    QList<ImageInfo> selectedInfoList(const bool currentFirst = false) const;
    ImageInfoList allInfo()    const;

    double zoomMin()           const;
    double zoomMax()           const;

    void toggleTag(int tagID);
    void toggleFullScreen(bool set);
    QList<SidebarWidget*> leftSidebarWidgets() const;
    StackedView::StackedViewMode viewMode()    const;

Q_SIGNALS:

    void signalAlbumSelected(Album*);
    void signalImageSelected(const ImageInfoList& selectedImage, const ImageInfoList& allImages);
    void signalNoCurrentItem();
    void signalSelectionChanged(int numberOfSelectedItems);
    void signalThumbSizeChanged(int);
    void signalZoomChanged(double);
    void signalSwitchedToPreview();
    void signalSwitchedToIconView();
    void signalSwitchedToMapView();
    void signalSwitchedToTableView();
    void signalSwitchedToTrashView();

    void signalGotoAlbumAndItem(const ImageInfo&);
    void signalGotoDateAndItem(AlbumIconItem*);
    void signalGotoTagAndItem(int tagID);
    void signalChangedTab(QWidget*);
    void signalFuzzySidebarActive(bool active);

public Q_SLOTS:

    void setZoomFactor(double zoom);

    // View Action slots
    void slotZoomIn();
    void slotZoomOut();
    void slotZoomTo100Percents();
    void slotFitToWindow();
    void slotSlideShowAll();
    void slotSlideShowSelection();
    void slotSlideShowRecursive();
    void slotSlideShowManualFromCurrent();
    void slotSlideShowManualFrom(const ImageInfo& info);
    void slotPresentation();

    // Album action slots
    void slotRefresh();
    void slotNewAlbum();
    void slotSortAlbums(int role);
    void slotDeleteAlbum();
    void slotRenameAlbum();
    void slotAlbumPropsEdit();
    void slotAlbumOpenInFileManager();
    void slotAlbumHistoryBack(int steps=1);
    void slotAlbumHistoryForward(int steps=1);
    void slotAlbumWriteMetadata();
    void slotAlbumReadMetadata();
    void slotAlbumSelected(QList<Album*> albums);

    void slotGotoAlbumAndItem(const ImageInfo& imageInfo);
    void slotGotoDateAndItem(const ImageInfo& imageInfo);
    void slotGotoTagAndItem(int tagID);

    void slotSelectAlbum(const QUrl& url);
    void slotSetCurrentWhenAvailable(const qlonglong id);

    // Tag action slots
    void slotNewTag();
    void slotDeleteTag();
    void slotEditTag();
    void slotOpenTagsManager();
    void slotAssignTag();

    // Search action slots
    void slotNewKeywordSearch();
    void slotNewAdvancedSearch();
    void slotNewDuplicatesSearch(PAlbum* album=0);
    void slotNewDuplicatesSearch(QList<PAlbum*> albums);
    void slotNewDuplicatesSearch(QList<TAlbum*> albums);

    // Image action slots
    void slotImageLightTable();
    void slotImageAddToLightTable();
    void slotImageAddToCurrentQueue();
    void slotImageAddToNewQueue();
    void slotImageAddToExistingQueue(int);
    void slotImagePreview();
    void slotMapWidgetView();
    void slotTableView();
    void slotIconView();
    void slotImageEdit();
    void slotImageFindSimilar();
    void slotImageExifOrientation(int orientation);
    void slotImageRename();
    void slotImageDelete();
    void slotImageDeletePermanently();
    void slotImageDeletePermanentlyDirectly();
    void slotImageTrashDirectly();
    void slotImageWriteMetadata();
    void slotImageReadMetadata();
    void slotSelectAll();
    void slotSelectNone();
    void slotSelectInvert();
    void slotSortImages(int order);
    void slotSortImagesOrder(int order);
    void slotGroupImages(int mode);
    void slotSortImageGroupOrder(int order);
    void slotMoveSelectionToAlbum();

    void slotAssignPickLabel(int pickId);
    void slotAssignColorLabel(int colorId);
    void slotAssignRating(int rating);

    // Tools action slots.
    void slotEditor();
    void slotLightTable();
    void slotQueueMgr();
    void slotFileWithDefaultApplication();

    void slotLeftSideBarActivate(QWidget* widget);
    void slotLeftSideBarActivate(SidebarWidget* widget);
    void slotLeftSideBarActivateAlbums();
    void slotLeftSideBarActivateTags();

    void slotRightSideBarActivateTitles();
    void slotRightSideBarActivateComments();
    void slotRightSideBarActivateAssignedTags();

    void slotFocusAndNextImage();

private:

    void toggleZoomActions();
    void setupConnections();
    void loadViewState();
    void saveViewState();
    void changeAlbumFromHistory(QList<Album*> album, QWidget* const widget);
    void slideShow(const ImageInfoList& infoList);

private Q_SLOTS:

    void slotAllAlbumsLoaded();

    void slotAlbumsCleared();

    void slotImageSelected();
    void slotTogglePreviewMode(const ImageInfo& info);
    void slotDispatchImageSelected();

    void slotLeftSidebarChangedTab(QWidget* w);

    void slotFirstItem();
    void slotPrevItem();
    void slotNextItem();
    void slotLastItem();
    void slotSelectItemByUrl(const QUrl&);
    void slotAwayFromSelection();

    void slotViewModeChanged();
    void slotEscapePreview();

    void slotSlideShowBuilderComplete(const SlideShowSettings& settings);

    void slotThumbSizeEffect();
    void slotZoomFactorChanged(double);

    void slotSidebarTabTitleStyleChanged();

    void slotImageChangeFailed(const QString& message, const QStringList& fileNames);

    void slotRatingChanged(const QUrl&, int);
    void slotColorLabelChanged(const QUrl&, int);
    void slotPickLabelChanged(const QUrl&, int);
    void slotToggleTag(const QUrl&, int);

    void slotPopupFiltersView();
    void slotSetupMetadataFilters(int);

    void slotAlbumRefreshComplete();

private:

    class Private;
    Private* const d;
};

}  // namespace Digikam

#endif // DIGIKAMVIEW_H
