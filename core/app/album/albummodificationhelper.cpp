/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2000-12-05
 * Description : helper class used to modify physical albums in views
 *
 * Copyright (C) 2009-2011 by Johannes Wienke <languitar at semipol dot de>
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

#include "albummodificationhelper.moc"

// Qt includes

#include <QAction>

// KDE includes

#include <kdebug.h>
#include <kinputdialog.h>
#include <kio/jobuidelegate.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kurl.h>

// Local includes

#include "albummanager.h"
#include "albumpropsedit.h"
#include "applicationsettings.h"
#include "collectionmanager.h"
#include "deletedialog.h"
#include "dio.h"
#include "digikamview.h"
#include "digikamapp.h"

namespace Digikam
{

class AlbumModificationHelper::Private
{
public:
    Private() :
        dialogParent(0)
    {
    }

    QWidget*     dialogParent;
};

AlbumModificationHelper::AlbumModificationHelper(QObject* const parent, QWidget* const dialogParent)
    : QObject(parent), d(new Private)
{
    d->dialogParent = dialogParent;
}

AlbumModificationHelper::~AlbumModificationHelper()
{
    delete d;
}

void AlbumModificationHelper::bindAlbum(QAction* const action, PAlbum* const album) const
{
    action->setData(QVariant::fromValue(AlbumPointer<PAlbum>(album)));
}

PAlbum* AlbumModificationHelper::boundAlbum(QObject* const sender) const
{
    QAction* action = 0;

    if ( (action = qobject_cast<QAction*>(sender)) )
    {
        return action->data().value<AlbumPointer<PAlbum> >();
    }

    return 0;
}

PAlbum* AlbumModificationHelper::slotAlbumNew()
{
    return slotAlbumNew(boundAlbum(sender()));
}

PAlbum* AlbumModificationHelper::slotAlbumNew(PAlbum* parent)
{
    if (!parent)
    {
        kWarning() << "No parent album given";
        return 0;
    }

    ApplicationSettings* settings = ApplicationSettings::instance();

    if (!settings)
    {
        kWarning() << "could not get Album Settings";
        return 0;
    }

/*
    QDir libraryDir(settings->getAlbumLibraryPath());

    if(!libraryDir.exists())
    {
        KMessageBox::error(0,
                           i18n("The album library has not been set correctly.\n"
                                "Select \"Configure Digikam\" from the Settings "
                                "menu and choose a folder to use for the album "
                                "library."));
        return;
    }
*/

    // if we create an album under root, need to supply the album root path.
    QString albumRootPath;

    albumRootPath = CollectionManager::instance()->oneAlbumRootPath();

    QString     title;
    QString     comments;
    QString     category;
    QDate       date;
    QStringList albumCategories;
    int         parentSelector;

    if (!AlbumPropsEdit::createNew(parent, title, comments, date, category,
                                   albumCategories, parentSelector))
    {
        return 0;
    }

    QStringList oldAlbumCategories(ApplicationSettings::instance()->getAlbumCategoryNames());

    if (albumCategories != oldAlbumCategories)
    {
        ApplicationSettings::instance()->setAlbumCategoryNames(albumCategories);
    }

    QString errMsg;
    PAlbum* album = 0;

    if (parent->isRoot() || parentSelector == 1)
    {
        album = AlbumManager::instance()->createPAlbum(albumRootPath, title, comments,
                                                       date, category, errMsg);
    }
    else
    {
        album = AlbumManager::instance()->createPAlbum(parent, title, comments,
                                                       date, category, errMsg);
    }

    if (!album)
    {
        KMessageBox::error(0, errMsg);
        return 0;
    }

    return album;
}

void AlbumModificationHelper::slotAlbumDelete()
{
    slotAlbumDelete(boundAlbum(sender()));
}

void AlbumModificationHelper::slotAlbumDelete(PAlbum* album)
{
    if (!album || album->isRoot() || album->isAlbumRoot())
    {
        return;
    }

    // find subalbums
    KUrl::List childrenList;
    addAlbumChildrenToList(childrenList, album);

    DeleteDialog dialog(d->dialogParent);

    // All subalbums will be presented in the list as well
    if (!dialog.confirmDeleteList(childrenList,
                                  childrenList.size() == 1 ?
                                  DeleteDialogMode::Albums : DeleteDialogMode::Subalbums,
                                  DeleteDialogMode::UserPreference))
    {
        return;
    }

    bool useTrash = !dialog.shouldDelete();

    // Currently trash kioslave can handle only full paths.
    // pass full folder path to the trashing job
    DIO::del(album, useTrash);
}

void AlbumModificationHelper::slotAlbumRename()
{
    slotAlbumRename(boundAlbum(sender()));
}

void AlbumModificationHelper::slotAlbumRename(PAlbum* album)
{
    if (!album)
    {
        return;
    }

    QString oldTitle(album->title());
    bool    ok;

    QString title = KInputDialog::getText(i18n("Rename Album (%1)", oldTitle),
                                          i18n("Enter new album name:"),
                                          oldTitle, &ok, d->dialogParent);

    if (!ok)
    {
        return;
    }

    if (title != oldTitle)
    {
        QString errMsg;

        if (!AlbumManager::instance()->renamePAlbum(album, title, errMsg))
        {
            KMessageBox::error(0, errMsg);
        }
    }
}

void AlbumModificationHelper::addAlbumChildrenToList(KUrl::List& list, Album* const album)
{
    // simple recursive helper function
    if (album)
    {
        list.append(album->databaseUrl());
        AlbumIterator it(album);

        while (it.current())
        {
            addAlbumChildrenToList(list, *it);
            ++it;
        }
    }
}

void AlbumModificationHelper::slotAlbumEdit()
{
    slotAlbumEdit(boundAlbum(sender()));
}

void AlbumModificationHelper::slotAlbumEdit(PAlbum* album)
{
    if (!album || album->isRoot() || album->isAlbumRoot())
    {
        return;
    }

    QString     oldTitle(album->title());
    QString     oldComments(album->caption());
    QString     oldCategory(album->category());
    QDate       oldDate(album->date());
    QStringList oldAlbumCategories(ApplicationSettings::instance()->getAlbumCategoryNames());

    QString     title, comments, category;
    QDate       date;
    QStringList albumCategories;

    if (AlbumPropsEdit::editProps(album, title, comments, date,
                                  category, albumCategories))
    {
        if (comments != oldComments)
        {
            album->setCaption(comments);
        }

        if (date != oldDate && date.isValid())
        {
            album->setDate(date);
        }

        if (category != oldCategory)
        {
            album->setCategory(category);
        }

        ApplicationSettings::instance()->setAlbumCategoryNames(albumCategories);

        // Do this last : so that if anything else changed we can
        // successfuly save to the db with the old name

        if (title != oldTitle)
        {
            QString errMsg;

            if (!AlbumManager::instance()->renamePAlbum(album, title, errMsg))
            {
                KMessageBox::error(d->dialogParent, errMsg);
            }
        }
        // Resorting the tree View after changing metadata
        DigikamApp::instance()->view()->slotSortAlbums(ApplicationSettings::instance()->getAlbumSortOrder());
    }
}

void AlbumModificationHelper::slotAlbumResetIcon(PAlbum* album)
{
    if (!album)
    {
        return;
    }

    QString err;
    AlbumManager::instance()->updatePAlbumIcon(album, 0, err);
}

void AlbumModificationHelper::slotAlbumResetIcon()
{
    slotAlbumResetIcon(boundAlbum(sender()));
}

} // namespace Digikam
