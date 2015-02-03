/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2009-04-24
 * Description : Qt item view for images
 *
 * Copyright (C) 2009-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2009-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAMIMAGEVIEW_H
#define DIGIKAMIMAGEVIEW_H

// Local includes

#include "imagecategorizedview.h"
#include "imageviewutilities.h"

namespace Digikam
{

class ImageViewUtilities;

class DigikamImageView : public ImageCategorizedView
{
    Q_OBJECT

public:

    explicit DigikamImageView(QWidget* const parent = 0);
    ~DigikamImageView();

    ImageViewUtilities* utilities() const;

    int  fitToWidthIcons();

    virtual void setThumbnailSize(const ThumbnailSize& size);

public Q_SLOTS:

    void openFile(const ImageInfo& info);

    void insertSelectedToCurrentQueue();
    void insertSelectedToNewQueue();
    void insertSelectedToExistingQueue(int queueid);

    void deleteSelected(const ImageViewUtilities::DeleteMode deleteMode = ImageViewUtilities::DeleteUseTrash);
    void deleteSelectedDirectly(const ImageViewUtilities::DeleteMode deleteMode = ImageViewUtilities::DeleteUseTrash);
    void assignTagToSelected(int tagID);
    void removeTagFromSelected(int tagID);
    void assignPickLabelToSelected(int pickId);
    void assignColorLabelToSelected(int colorId);
    void assignRatingToSelected(int rating);

    void setAsAlbumThumbnail(const ImageInfo& setAsThumbnail);
    void createNewAlbumForSelected();
    void rename();

    void assignPickLabel(const QModelIndex& index, int pickId);
    void assignColorLabel(const QModelIndex& index, int colorId);
    void assignRating(const QList<QModelIndex>& index, int rating);

    void createGroupFromSelection();
    void createGroupByTimeFromSelection();
    void ungroupSelected();
    void removeSelectedFromGroup();

    void setFaceMode(bool on);
    void confirmFaces(const QList<QModelIndex>& indexes, int tagId);
    void removeFaces(const QList<QModelIndex>& indexes);

Q_SIGNALS:

    void previewRequested(const ImageInfo& info);
    void gotoAlbumAndImageRequested(const ImageInfo& info);
    void gotoTagAndImageRequested(int tagId);
    void gotoDateAndImageRequested(const ImageInfo& info);
    void signalPopupTagsView();

protected Q_SLOTS:

    void groupIndicatorClicked(const QModelIndex& index);
    void showGroupContextMenu(const QModelIndex& index, QContextMenuEvent* event);

protected:

    void addRejectionOverlay(ImageDelegate* delegate = 0);
    void addAssignNameOverlay(ImageDelegate* delegate = 0);

    virtual void activated(const ImageInfo& info, Qt::KeyboardModifiers modifiers);
    virtual void showContextMenuOnInfo(QContextMenuEvent* event, const ImageInfo& info);
    virtual void showContextMenu(QContextMenuEvent* event);
    virtual void slotSetupChanged();

private Q_SLOTS:

    void slotRotateLeft(const QList<QModelIndex>&);
    void slotRotateRight(const QList<QModelIndex>&);
    void slotInitProgressIndicator();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif /* DIGIKAMIMAGEVIEW_H */
