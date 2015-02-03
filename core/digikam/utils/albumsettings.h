/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2003-16-10
 * Description : albums settings interface
 *
 * Copyright (C) 2003-2004 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2003-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2007      by Arnd Baecker <arnd dot baecker at web dot de>
 * Copyright (C) 2014      by Mohamed Anwer <mohammed dot ahmed dot anwer at gmail dot com>
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

#ifndef ALBUMSETTINGS_H
#define ALBUMSETTINGS_H

// Qt includes

#include <QStringList>
#include <QString>
#include <QFont>
#include <QObject>

// KDE includes

#include <kmultitabbar.h>
#include <kconfiggroup.h>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DatabaseParameters;
class VersionManagerSettings;

class AlbumSettings : public QObject
{
    Q_OBJECT

public:

    enum AlbumSortOrder
    {
        ByFolder = 0,
        ByCategory,
        ByDate
    };

    enum ItemLeftClickAction
    {
        ShowPreview = 0,
        StartEditor
    };

    /**
     * Possible ways of comparing strings.
     */
    enum StringComparisonType
    {

        /**
         * Natural compare using KStringHandler::naturalCompare.
         */
        Natural = 0,

        /**
         * Normal comparison using Qt's compare function.
         */
        Normal
    };

Q_SIGNALS:

    void setupChanged();

    void recurseSettingsChanged();
    void nepomukSettingsChanged();

public:

    static AlbumSettings* instance();

    void readSettings();
    void saveSettings();

    void emitSetupChanged();

    bool showToolTipsIsValid()      const;
    bool showAlbumToolTipsIsValid() const;

    void setDatabaseFilePath(const QString& path);
    QString getDatabaseFilePath() const;

    void setShowSplashScreen(bool val);
    bool getShowSplashScreen() const;

    void setAlbumCategoryNames(const QStringList& list);
    QStringList getAlbumCategoryNames() const;

    bool addAlbumCategoryName(const QString& name);
    bool delAlbumCategoryName(const QString& name);

    void setAlbumSortOrder(const AlbumSortOrder order);
    AlbumSortOrder getAlbumSortOrder() const;

    void setAlbumSortChanged(bool val);
    bool getAlbumSortChanged() const;

    void setImageSortOrder(int order);
    int  getImageSortOrder() const;

    // means ascending or descending
    void setImageSorting(int sorting);
    int  getImageSorting() const;

    void setImageGroupMode(int mode);
    int  getImageGroupMode() const;

    void setItemLeftClickAction(const ItemLeftClickAction action);
    ItemLeftClickAction getItemLeftClickAction() const;

    QString getImageFileFilter() const;
    void addToImageFileFilter(const QString& extensions);

    QString getMovieFileFilter() const;

    QString getAudioFileFilter() const;

    QString getRawFileFilter() const;

    bool    addImageFileExtension(const QString& ext);
    QString getAllFileFilter() const;

    void setDefaultIconSize(int val);
    int  getDefaultIconSize() const;

    void setTreeViewIconSize(int val);
    int  getTreeViewIconSize() const;

    void setTreeViewFont(const QFont& font);
    QFont getTreeViewFont() const;

    void setIconViewFont(const QFont& font);
    QFont getIconViewFont() const;

    void setRatingFilterCond(int val);
    int  getRatingFilterCond() const;

    void setIconShowName(bool val);
    bool getIconShowName() const;

    void setIconShowSize(bool val);
    bool getIconShowSize() const;

    void setIconShowTitle(bool val);
    bool getIconShowTitle() const;

    void setIconShowComments(bool val);
    bool getIconShowComments() const;

    void setIconShowResolution(bool val);
    bool getIconShowResolution() const;

    void setIconShowAspectRatio(bool val);
    bool getIconShowAspectRatio() const;

    void setIconShowTags(bool val);
    bool getIconShowTags() const;

    void setIconShowDate(bool val);
    bool getIconShowDate() const;

    void setIconShowModDate(bool val);
    bool getIconShowModDate() const;

    void setIconShowRating(bool val);
    bool getIconShowRating() const;

    void setIconShowImageFormat(bool val);
    bool getIconShowImageFormat() const;

    void setIconShowCoordinates(bool val);
    bool getIconShowCoordinates() const;

    /**
     * Sets the visibility of the overlay buttons on the image icons.
     */
    void setIconShowOverlays(bool val);
    /**
     * Determines whether the overlay buttons should be displayed on the icons.
     */
    bool getIconShowOverlays() const;

    void setToolTipsFont(const QFont& font);
    QFont getToolTipsFont() const;

    void setShowToolTips(bool val);
    bool getShowToolTips() const;

    void setToolTipsShowFileName(bool val);
    bool getToolTipsShowFileName() const;

    void setToolTipsShowFileDate(bool val);
    bool getToolTipsShowFileDate() const;

    void setToolTipsShowFileSize(bool val);
    bool getToolTipsShowFileSize() const;

    void setToolTipsShowImageType(bool val);
    bool getToolTipsShowImageType() const;

    void setToolTipsShowImageDim(bool val);
    bool getToolTipsShowImageDim() const;

    void setToolTipsShowImageAR(bool val);
    bool getToolTipsShowImageAR() const;

    void setToolTipsShowPhotoMake(bool val);
    bool getToolTipsShowPhotoMake() const;

    void setToolTipsShowPhotoDate(bool val);
    bool getToolTipsShowPhotoDate() const;

    void setToolTipsShowPhotoFocal(bool val);
    bool getToolTipsShowPhotoFocal() const;

    void setToolTipsShowPhotoExpo(bool val);
    bool getToolTipsShowPhotoExpo() const;

    void setToolTipsShowPhotoMode(bool val);
    bool getToolTipsShowPhotoMode() const;

    void setToolTipsShowPhotoFlash(bool val);
    bool getToolTipsShowPhotoFlash() const;

    void setToolTipsShowPhotoWB(bool val);
    bool getToolTipsShowPhotoWB() const;

    void setToolTipsShowAlbumName(bool val);
    bool getToolTipsShowAlbumName() const;

    void setToolTipsShowComments(bool val);
    bool getToolTipsShowComments() const;

    void setToolTipsShowTags(bool val);
    bool getToolTipsShowTags() const;

    void setToolTipsShowLabelRating(bool val);
    bool getToolTipsShowLabelRating() const;

    void setShowAlbumToolTips(bool val);
    bool getShowAlbumToolTips() const;

    void setToolTipsShowAlbumTitle(bool val);
    bool getToolTipsShowAlbumTitle() const;

    void setToolTipsShowAlbumDate(bool val);
    bool getToolTipsShowAlbumDate() const;

    void setToolTipsShowAlbumCollection(bool val);
    bool getToolTipsShowAlbumCollection() const;

    void setToolTipsShowAlbumCategory(bool val);
    bool getToolTipsShowAlbumCategory() const;

    void setToolTipsShowAlbumCaption(bool val);
    bool getToolTipsShowAlbumCaption() const;

    void setToolTipsShowVideoAspectRatio(bool val);
    bool getToolTipsShowVideoAspectRatio() const;

    void setToolTipsShowVideoAudioBitRate(bool val);
    bool getToolTipsShowVideoAudioBitRate() const;

    void setToolTipsShowVideoAudioChannelType(bool val);
    bool getToolTipsShowVideoAudioChannelType() const;

    void setToolTipsShowVideoAudioCompressor(bool val);
    bool getToolTipsShowVideoAudioCompressor() const;

    void setToolTipsShowVideoDuration(bool val);
    bool getToolTipsShowVideoDuration() const;

    void setToolTipsShowVideoFrameRate(bool val);
    bool getToolTipsShowVideoFrameRate() const;

    void setToolTipsShowVideoVideoCodec(bool val);
    bool getToolTipsShowVideoVideoCodec() const;

    void    setCurrentTheme(const QString& theme);
    QString getCurrentTheme() const;

    void setSidebarTitleStyle(KMultiTabBar::KMultiTabBarStyle style);
    KMultiTabBar::KMultiTabBarStyle getSidebarTitleStyle() const;

    void    setUseTrash(bool val);
    bool    getUseTrash() const;

    void    setShowTrashDeleteDialog(bool val);
    bool    getShowTrashDeleteDialog() const;

    void    setShowPermanentDeleteDialog(bool val);
    bool    getShowPermanentDeleteDialog() const;

    void    setApplySidebarChangesDirectly(bool val);
    bool    getApplySidebarChangesDirectly() const;

    void setPreviewLoadFullImageSize(bool val);
    bool getPreviewLoadFullImageSize() const;

    void setPreviewShowIcons(bool val);
    bool getPreviewShowIcons() const;

    void setShowFolderTreeViewItemsCount(bool val);
    bool getShowFolderTreeViewItemsCount() const;

    void setRecurseAlbums(bool val);
    bool getRecurseAlbums() const;

    void setRecurseTags(bool val);
    bool getRecurseTags() const;

    void setShowThumbbar(bool val);
    bool getShowThumbbar() const;

    void setSyncNepomukToDigikam(bool val);
    bool getSyncNepomukToDigikam() const;

    void setSyncDigikamToNepomuk(bool val);
    bool getSyncDigikamToNepomuk() const;

    /**
     * Defines the way in which string comparisons are performed.
     *
     * @param val new way to compare strings
     */
    void setStringComparisonType(AlbumSettings::StringComparisonType val);
    /**
     * Tells in which way strings are compared at the moment.
     *
     * @return string comparison type to use.
     */
    StringComparisonType getStringComparisonType() const;

    DatabaseParameters getDatabaseParameters() const;
    void setDatabaseParameters(const DatabaseParameters& params);

    QString getDatabaseType() const;
    void setDatabaseType(const QString& databaseType);

    QString getDatabaseConnectoptions() const;
    void setDatabaseConnectoptions(const QString& connectoptions);

    QString getDatabaseName() const;
    void setDatabaseName(const QString& databaseName);

    QString getDatabaseNameThumbnails() const;
    void setDatabaseNameThumbnails(const QString& databaseNameThumbnails);

    QString getDatabaseHostName() const;
    void setDatabaseHostName(const QString& hostName);

    QString getDatabasePassword() const;
    void setDatabasePassword(const QString& password);

    int getDatabasePort() const;
    void setDatabasePort(int port);

    QString getDatabaseUserName() const;
    void setDatabaseUserName(const QString& userName);

    bool getInternalDatabaseServer() const;
    void setInternalDatabaseServer(const bool useInternalDBServer);

    void setVersionManagerSettings(const VersionManagerSettings& settings);
    VersionManagerSettings getVersionManagerSettings() const;

    double getFaceDetectionAccuracy() const;
    void setFaceDetectionAccuracy(double value);

    void setApplicationStyle(const QString& style);
    QString getApplicationStyle() const;

    KConfigGroup generalConfigGroup() const;

public Q_SLOTS:

    void applyNepomukSettings() const;
    void triggerResyncWithNepomuk() const;

private:

    AlbumSettings();
    ~AlbumSettings();

    void init();

private:

    friend class AlbumSettingsCreator;

    class Private;
    Private* const d;
};

}  // namespace Digikam

#endif  // ALBUMSETTINGS_H
