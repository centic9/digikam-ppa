/* ============================================================
*
* This file is a part of digiKam project
* http://www.digikam.org
*
* Date        : 2009-04-30
* Description : rating icon view item at mouse hover
*
* Copyright (C) 2008      by Peter Penz <peter.penz@gmx.at>
* Copyright (C) 2010      by Aditya Bhatt <caulier dot gilles at gmail dot com>
* Copyright (C) 2009-2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#include "tagslineeditoverlay.moc"

// Qt includes

#include <QMouseEvent>

// KDE includes

#include <klocale.h>
#include <kglobalsettings.h>
#include <kdebug.h>

// Local includes

#include "imagedelegate.h"
#include "imagemodel.h"
#include "imagecategorizedview.h"
#include "addtagslineedit.h"
#include "tagscache.h"
#include "abstractalbummodel.h"
#include "albummodel.h"

namespace Digikam
{

TagsLineEditOverlay::TagsLineEditOverlay(QObject* const parent)
    : AbstractWidgetDelegateOverlay(parent)
{
}

AddTagsLineEdit* TagsLineEditOverlay::addTagsLineEdit() const
{
    return static_cast<AddTagsLineEdit*>(m_widget);
}

QWidget* TagsLineEditOverlay::createWidget()
{
    //const bool animate = KGlobalSettings::graphicEffectsLevel() & KGlobalSettings::SimpleAnimationEffects;
    AddTagsLineEdit* const lineEdit = new AddTagsLineEdit(parentWidget());
    lineEdit->setClickMessage("Name");
    lineEdit->setReadOnly(false);

    TagModel* const model           = new TagModel(AbstractAlbumModel::IncludeRootAlbum, this);
    model->setCheckable(true);
    model->setRootCheckable(false);
    lineEdit->setModel(model);

    return lineEdit;
}

void TagsLineEditOverlay::setActive(bool active)
{
    AbstractWidgetDelegateOverlay::setActive(active);

    if (active)
    {
        connect(addTagsLineEdit(), SIGNAL(returnPressed(QString)),
                this, SLOT(slotTagChanged(QString)));

        if (view()->model())
            connect(view()->model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                    this, SLOT(slotDataChanged(QModelIndex,QModelIndex)));
    }
    else
    {
        // widget is deleted

        if (view() && view()->model())
        {
            disconnect(view()->model(), 0, this, 0);
        }
    }
}

void TagsLineEditOverlay::visualChange()
{
    if (m_widget && m_widget->isVisible())
    {
        updatePosition();
    }
}

void TagsLineEditOverlay::hide()
{
    //delegate()->setRatingEdited(QModelIndex());
    AbstractWidgetDelegateOverlay::hide();
    //kDebug() << "Hide called, probably mouse left";

    if (!m_widget->hasFocus())
    {
        m_widget->releaseKeyboard();
        m_widget->releaseMouse();
        addTagsLineEdit()->clear();
    }
}

void TagsLineEditOverlay::updatePosition()
{
    if (!m_index.isValid() || !m_widget)
    {
        return;
    }

    QRect thumbrect = delegate()->ratingRect();
    //kDebug() << "updatePosition called, probably a mouseover : " << thumbrect;
    QRect rect      = thumbrect;

    if (rect.width() > addTagsLineEdit()->width() )
    {
        int offset = (rect.width() - addTagsLineEdit()->width()) / 2;
        rect.adjust(offset, 0, -offset, 0);
    }

    QRect visualRect = m_view->visualRect(m_index);
    rect.translate(visualRect.topLeft());

    m_widget->setFixedSize(rect.width() + 2, rect.height() + 5);
    m_widget->move(rect.topLeft());
    m_widget->setFocus();
}

void TagsLineEditOverlay::updateTag()
{
    if (!m_index.isValid())
    {
        return;
    }

    ImageInfo info = ImageModel::retrieveImageInfo(m_index);
    kDebug() << "called updateTag()";
    //TODO: ADD ratingWidget()->setRating(info.rating());
}

void TagsLineEditOverlay::slotTagChanged(int tagId)
{
    kDebug() << "Tag changed";

    if (m_widget && m_widget->isVisible() && m_index.isValid())
    {
        emit tagEdited(m_index, tagId);
    }
}

void TagsLineEditOverlay::slotTagChanged(const QString& name)
{
    kDebug() << "Tag changed";

    if (m_widget && m_widget->isVisible() && m_index.isValid())
    {
        emit tagEdited(m_index, name);
    }
}

void TagsLineEditOverlay::slotDataChanged(const QModelIndex& /*topLeft*/, const QModelIndex& /*bottomRight*/)
{
/*
    if (m_widget && m_widget->isVisible() && QItemSelectionRange(topLeft, bottomRight).contains(m_index))
        updateTag();
*/
}

void TagsLineEditOverlay::slotEntered(const QModelIndex& index)
{
    AbstractWidgetDelegateOverlay::slotEntered(index);

    // see bug 228810, this is a small workaround
    if (m_widget && m_widget->isVisible() && m_index.isValid() && index == m_index)
    {
        addTagsLineEdit()->setVisible(true);
    }

    m_index = index;

    updatePosition();
    //updateTag();

    //delegate()->setRatingEdited(m_index);
    view()->update(m_index);
}

} // namespace Digikam
