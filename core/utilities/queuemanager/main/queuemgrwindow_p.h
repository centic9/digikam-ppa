/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2008-11-21
 * Description : Batch Queue Manager GUI
 *
 * Copyright (C) 2008-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef QUEUEMGRWINDOWPRIVATE_H
#define QUEUEMGRWINDOWPRIVATE_H

// Qt includes

#include <QLabel>
#include <QString>

// KDE includes

#include <kaction.h>
#include <kiconloader.h>
#include <kselectaction.h>
#include <ktoggleaction.h>

// Local includes

#include "actionthread.h"
#include "assignedlist.h"
#include "queuelist.h"
#include "queuepool.h"
#include "queuesettingsview.h"
#include "statusprogressbar.h"
#include "sidebar.h"
#include "toolsettingsview.h"
#include "toolsview.h"

namespace Digikam
{

class BatchToolsManager;

class QueueMgrWindow::Private
{

public:

    Private() :
        TOP_SPLITTER_CONFIG_KEY("BqmTopSplitter"),
        BOTTOM_SPLITTER_CONFIG_KEY("BqmBottomSplitter"),
        VERTICAL_SPLITTER_CONFIG_KEY("BqmVerticalSplitter")
    {
        busy                   = false;
        clearQueueAction       = 0;
        moveUpToolAction       = 0;
        moveDownToolAction     = 0;
        removeToolAction       = 0;
        removeItemsSelAction   = 0;
        removeItemsDoneAction  = 0;
        clearToolsAction       = 0;
        donateMoneyAction      = 0;
        statusProgressBar      = 0;
        statusLabel            = 0;
        runAction              = 0;
        stopAction             = 0;
        removeQueueAction      = 0;
        newQueueAction         = 0;
        saveQueueAction        = 0;
        rawCameraListAction    = 0;
        topSplitter            = 0;
        bottomSplitter         = 0;
        verticalSplitter       = 0;
        contributeAction       = 0;
        assignedList           = 0;
        queuePool              = 0;
        queueSettingsView      = 0;
        toolsView              = 0;
        batchToolsMgr          = 0;
        toolSettings           = 0;
        showMenuBarAction      = 0;
        thread                 = 0;
        currentQueueToProcess  = 0;
    }

    bool                     busy;

    int                      currentQueueToProcess;

    QLabel*                  statusLabel;

    KAction*                 clearQueueAction;
    KAction*                 removeItemsSelAction;
    QAction*                 removeItemsDoneAction;
    QAction*                 moveUpToolAction;
    QAction*                 moveDownToolAction;
    QAction*                 removeToolAction;
    QAction*                 clearToolsAction;

    KAction*                 runAction;
    KAction*                 stopAction;
    KAction*                 removeQueueAction;
    KAction*                 newQueueAction;
    KAction*                 saveQueueAction;
    KAction*                 donateMoneyAction;
    KAction*                 contributeAction;
    KAction*                 rawCameraListAction;

    SidebarSplitter*         topSplitter;
    SidebarSplitter*         bottomSplitter;
    SidebarSplitter*         verticalSplitter;

    KToggleAction*           showMenuBarAction;

    BatchToolsManager*       batchToolsMgr;

    StatusProgressBar*       statusProgressBar;

    ActionThread*            thread;

    ToolsView*               toolsView;
    ToolSettingsView*        toolSettings;
    AssignedListView*        assignedList;
    QueuePool*               queuePool;
    QueueSettingsView*       queueSettingsView;

    const QString            TOP_SPLITTER_CONFIG_KEY;
    const QString            BOTTOM_SPLITTER_CONFIG_KEY;
    const QString            VERTICAL_SPLITTER_CONFIG_KEY;
};

}  // namespace Digikam

#endif /* QUEUEMGRWINDOWPRIVATE_H */
