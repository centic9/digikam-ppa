/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-03-22
 * Description : tags folder view.
 *
 * Copyright (C) 2005-2006 by Joern Ahrens <joern dot ahrens at kdemail dot net>
 * Copyright (C) 2006-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef TAGFOLDERVIEW_H
#define TAGFOLDERVIEW_H

// Qt includes

#include <QTreeView>

// Local includes

#include "albumtreeview.h"

namespace Digikam
{

class ContextMenuHelper;

class TagFolderView: public TagTreeView
{
    Q_OBJECT

public:

    /**
     * Constructor.
     *
     * @param parent parent for Qt's parent child mechanism
     * @param model tag model to display
     */
    TagFolderView(QWidget* parent, Digikam::TagModel* model);

    /**
     * Destructor.
     */
    virtual ~TagFolderView();

    /**
     * Define whether to show the "find duplicate" action in context menus
     * or not.
     *
     * @param show if <code>true</code> the action to find duplicate images in
     *             the tag album is displayed
     */
    void setShowFindDuplicateAction(bool show);

Q_SIGNALS:

    void signalFindDuplicatesInAlbum(Album*);

protected:

    QString contextMenuTitle() const;

    /**
     * Hook method to add custom actions to the generated context menu.
     *
     * The default implementation adds actions to reset the tag icon and to
     * find duplicates in a tag album. If you want to use these actions,
     * remember to call this class' implementation of this method and
     * the handleCustomContextMenuAction in your derived class.
     *
     * @param cmh helper object to create the context menu
     * @param album tag on which the context menu will be created. May be null if
     *              it is requested on no tag entry
     */
    virtual void addCustomContextMenuActions(ContextMenuHelper& cmh, Album* album);

    /**
     * Hook method to handle the custom context menu actions that were added
     * with addCustomContextMenuActions.
     *
     * @param action the action that was chosen by the user, may be null if none
     *               of the custom actions were selected
     * @param album the tag on which the context menu was requested. May be null
     *              if there was no
     */
    virtual void handleCustomContextMenuAction(QAction* action, AlbumPointer<Album> album);

private Q_SLOTS:

    void slotTagNewFromABCMenu(const QString& personName);

private:

    class TagFolderViewPriv;
    TagFolderViewPriv* const d;
};

} // namespace Digikam

#endif // TAGFOLDERVIEW_H
