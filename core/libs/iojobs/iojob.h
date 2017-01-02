/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2015-06-15
 * Description : IO Jobs for file systems jobs
 *
 * Copyright (C) 2015 by Mohamed Anwer <m dot anwer at gmx dot com>
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

#ifndef IOJOB_H
#define IOJOB_H

// Qt includes

#include <QUrl>

// Local includes

#include "dtrashiteminfo.h"
#include "dtrash.h"
#include "actionthreadbase.h"
#include "digikam_export.h"

namespace Digikam
{

class ImageInfo;

class DIGIKAM_EXPORT IOJob : public ActionJob
{
    Q_OBJECT

protected:

    IOJob();

Q_SIGNALS:

    void error(const QString& errMsg);
};

// ---------------------------------------

class DIGIKAM_EXPORT CopyJob : public IOJob
{
    Q_OBJECT

public:

    CopyJob(const QUrl& src, const QUrl& dest, bool isMove);

protected:

    void run();
    bool copyFolderRecursively(const QString& srcPath, const QString& dstPath);

private:

    QUrl m_src;
    QUrl m_dest;
    bool m_isMove;
};

// ---------------------------------------

class DIGIKAM_EXPORT DeleteJob : public IOJob
{
    Q_OBJECT

public:

    DeleteJob(const QUrl& srcToDelete, bool useTrash);

protected:

    void run();

private:

    QUrl m_srcToDelete;
    bool m_useTrash;
};

// ---------------------------------------

class DIGIKAM_EXPORT RenameFileJob : public IOJob
{
    Q_OBJECT

public:

    RenameFileJob(const QUrl& srcToRename, const QUrl& newName);

Q_SIGNALS:

    void signalRenamed(const QUrl& oldUrl, const QUrl& newUrl);

protected:

    void run();

private:

    QUrl m_srcToRename;
    QUrl m_newUrl;
};

// ----------------------------------------------

class DIGIKAM_EXPORT DTrashItemsListingJob : public IOJob
{
    Q_OBJECT

public:

    DTrashItemsListingJob(const QString& collectionPath);

Q_SIGNALS:

    void trashItemInfo(const DTrashItemInfo& info);

private:

    void run();

private:

    QString m_collectionPath;
};

} // namespace Digikam

#endif // IOJOB_H
