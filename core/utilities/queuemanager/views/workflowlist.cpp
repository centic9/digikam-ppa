/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2012-12-18
 * Description : Customized Workflow Settings list.
 *
 * Copyright (C) 2012-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "workflowlist.moc"

// Qt includes

#include <QDrag>
#include <QHeaderView>
#include <QMap>
#include <QMimeData>
#include <QPainter>
#include <QPixmap>

// KDE includes

#include <kaction.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <kstandardguiitem.h>

// Local includes

#include "workflowmanager.h"
#include "workflowdlg.h"

namespace Digikam
{

WorkflowItem::WorkflowItem(WorkflowList* const parent, const QString& title)
    : QTreeWidgetItem(parent)
{
    setDisabled(false);
    setSelected(false);

    Workflow q = WorkflowManager::instance()->findByTitle(title);

    setIcon(0, SmallIcon("step"));
    setText(0, title);
    setText(1, QString::number(q.aTools.count()));
    setText(2, q.desc);
}

WorkflowItem::~WorkflowItem()
{
}

QString WorkflowItem::title() const
{
    return text(0);
}

int WorkflowItem::count() const
{
    return text(1).toInt();
}

// ---------------------------------------------------------------------------

WorkflowList::WorkflowList(QWidget* const parent)
    : QTreeWidget(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    setIconSize(QSize(22, 22));
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSortingEnabled(false);
    setAllColumnsShowFocus(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setColumnCount(3);
    setHeaderHidden(false);
    setDragEnabled(true);
    setRootIsDecorated(false);

    QStringList titles;
    titles.append(i18n("Title"));
    titles.append(i18n("Tools"));
    titles.append(i18n("Description"));

    setHeaderLabels(titles);
    header()->setResizeMode(0, QHeaderView::ResizeToContents);
    header()->setResizeMode(1, QHeaderView::ResizeToContents);
    header()->setResizeMode(2, QHeaderView::Stretch);

    connect(this, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(slotContextMenu()));

    WorkflowManager* const mngr = WorkflowManager::instance();
    QStringList failed;
    mngr->load(failed);

    foreach (const Workflow& q, mngr->queueSettingsList())
    {
        slotsAddQueueSettings(q.title);
    }

    if (!failed.isEmpty())
    {
        KMessageBox::informationList(0, i18n("Some Workflows cannot be loaded from your config file due to an incompatible "
                                             "version of a tool."),
                                     failed, i18n("Batch Queue Manager"));
    }
}

WorkflowList::~WorkflowList()
{
}

void WorkflowList::slotsAddQueueSettings(const QString& title)
{
    WorkflowItem* const item = findByTitle(title);

    if (!item)
    {
        new WorkflowItem(this, title);
    }
}

void WorkflowList::slotRemoveQueueSettings(const QString& title)
{
    WorkflowItem* const item = findByTitle(title);

    if (item)
    {
        delete item;
    }
}

WorkflowItem* WorkflowList::findByTitle(const QString& title)
{
    QTreeWidgetItemIterator it(this);

    while (*it)
    {
        WorkflowItem* const item = dynamic_cast<WorkflowItem*>(*it);

        if (item && item->title() == title)
        {
            return item;
        }

        ++it;
    }

    return 0;
}

void WorkflowList::startDrag(Qt::DropActions /*supportedActions*/)
{
    QList<QTreeWidgetItem*> list = selectedItems();
    if (!list.isEmpty())
    {
        WorkflowItem* const item = dynamic_cast<WorkflowItem*>(list.first());
        if (!item)
        {
            return;
        }

        QPixmap icon(DesktopIcon("step", 48));
        int w = icon.width();
        int h = icon.height();

        QPixmap pix(w + 4, h + 4);
        QString text(QString::number(item->count()));

        QPainter p(&pix);
        p.fillRect(0, 0, pix.width() - 1, pix.height() - 1, QColor(Qt::white));
        p.setPen(QPen(Qt::black, 1));
        p.drawRect(0, 0, pix.width() - 1, pix.height() - 1);
        p.drawPixmap(2, 2, icon);
        QRect r = p.boundingRect(2, 2, w, h, Qt::AlignLeft | Qt::AlignTop, text);
        r.setWidth(qMax(r.width(), r.height()));
        r.setHeight(qMax(r.width(), r.height()));
        p.fillRect(r, QColor(0, 80, 0));
        p.setPen(Qt::white);
        QFont f(font());
        f.setBold(true);
        p.setFont(f);
        p.drawText(r, Qt::AlignCenter, text);
        p.end();

        QDrag* const drag = new QDrag(this);
        drag->setMimeData(mimeData(list));
        drag->setPixmap(pix);
        drag->exec();
    }
}

QStringList WorkflowList::mimeTypes() const
{
    return QStringList() << "digikam/workflow";
}

void WorkflowList::mouseDoubleClickEvent(QMouseEvent*)
{
    if (viewport()->isEnabled())
    {
        slotAssignQueueSettings();
    }
}

QMimeData* WorkflowList::mimeData(const QList<QTreeWidgetItem*> items) const
{
    QMimeData* const mimeData = new QMimeData();
    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    if (!items.isEmpty())
    {
        WorkflowItem* const item  = dynamic_cast<WorkflowItem*>(items.first());
        if (item)
        {
            stream << item->title();
        }
    }

    mimeData->setData("digikam/workflow", encodedData);
    return mimeData;
}

void WorkflowList::slotContextMenu()
{
    KMenu popmenu(this);
    KAction* const assignAction = new KAction(KIcon("bqm-add"),             i18n("Assign Workflow to current queue"), this);
    KAction* const propAction   = new KAction(KIcon("document-properties"), i18n("Edit Workflow"),                    this);
    KAction* const delAction    = new KAction(KIcon("edit-delete"),         i18n("Delete Workflow"),                  this);

    popmenu.addAction(assignAction);
    popmenu.addAction(propAction);
    popmenu.addSeparator();
    popmenu.addAction(delAction);

    QAction* const choice = popmenu.exec(QCursor::pos());

    if (choice == assignAction)
    {
        slotAssignQueueSettings();
    }
    else if (choice == propAction)
    {
        QList<QTreeWidgetItem*> list = selectedItems();
        if (!list.isEmpty())
        {
            WorkflowItem* const item = dynamic_cast<WorkflowItem*>(list.first());
            if (item)
            {
                WorkflowManager* const mngr = WorkflowManager::instance();
                Workflow wfOld              = mngr->findByTitle(item->title());
                Workflow wfNew              = wfOld;

                if (WorkflowDlg::editProps(wfNew))
                {
                    mngr->remove(wfOld);
                    mngr->insert(wfNew);
                    mngr->save();
                }
            }
        }
    }
    else if (choice == delAction)
    {
        QList<QTreeWidgetItem*> list = selectedItems();
        if (!list.isEmpty())
        {
            WorkflowItem* const item = dynamic_cast<WorkflowItem*>(list.first());
            if (item)
            {
                int result = KMessageBox::warningYesNo(0,
                                                    i18n("Are you sure you want to "
                                                            "delete the selected workflow "
                                                            "\"%1\"?", item->title()),
                                                    i18n("Delete Workflow?"),
                                                    KGuiItem(i18n("Delete")),
                                                    KStandardGuiItem::cancel());
                if (result == KMessageBox::Yes)
                {
                    WorkflowManager* const mngr = WorkflowManager::instance();
                    Workflow wf                 = mngr->findByTitle(item->title());
                    mngr->remove(wf);
                    removeItemWidget(item, 0);
                    delete item;
                }
            }
        }
    }
}

void WorkflowList::slotAssignQueueSettings()
{
    QList<QTreeWidgetItem*> list = selectedItems();
    if (!list.isEmpty())
    {
        WorkflowItem* const item = dynamic_cast<WorkflowItem*>(list.first());
        if (item)
        {
            emit signalAssignQueueSettings(item->title());
        }
    }
}

}  // namespace Digikam
