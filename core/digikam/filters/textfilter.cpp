/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2011-02-23
 * Description : a widget to filter album contents by text query
 *
 * Copyright (C) 2011-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "textfilter.moc"

// Qt includes

#include <QAction>
#include <QToolButton>

// KDE includes

#include <klocale.h>
#include <kmenu.h>
#include <kiconloader.h>

namespace Digikam
{

class TextFilter::Private
{
public:

    Private()
    {
        imageNameAction        = 0;
        imageTitleAction       = 0;
        imageCommentAction     = 0;
        tagNameAction          = 0;
        albumNameAction        = 0;
        optionsBtn             = 0;
        optionsMenu            = 0;
        searchTextBar          = 0;
        imageAspectRatioAction = 0;
        imagePixelSizeAction   = 0;
    }

    QAction*       imageNameAction;
    QAction*       imageTitleAction;
    QAction*       imageCommentAction;
    QAction*       tagNameAction;
    QAction*       albumNameAction;
    QAction*       imageAspectRatioAction;
    QAction*       imagePixelSizeAction;

    QToolButton*   optionsBtn;

    KMenu*         optionsMenu;

    SearchTextBar* searchTextBar;
};

TextFilter::TextFilter(QWidget* const parent)
    : KHBox(parent), d(new Private)
{
    d->searchTextBar = new SearchTextBar(this, "AlbumIconViewFilterSearchTextBar");
    d->searchTextBar->setTextQueryCompletion(true);
    d->searchTextBar->setToolTip(i18n("Text quick filter (search)"));
    d->searchTextBar->setWhatsThis(i18n("Enter search patterns to quickly filter this view on "
                                        "file names, captions (comments), and tags"));

    d->optionsBtn = new QToolButton(this);
    d->optionsBtn->setToolTip( i18n("Text Search Fields"));
    d->optionsBtn->setIcon(KIconLoader::global()->loadIcon("configure", KIconLoader::Toolbar));
    d->optionsBtn->setPopupMode(QToolButton::InstantPopup);
    d->optionsBtn->setWhatsThis(i18n("Defines which fields to search for the text in."));

    d->optionsMenu            = new KMenu(d->optionsBtn);
    d->imageNameAction        = d->optionsMenu->addAction(i18n("Image Name"));
    d->imageNameAction->setCheckable(true);
    d->imageTitleAction       = d->optionsMenu->addAction(i18n("Image Title"));
    d->imageTitleAction->setCheckable(true);
    d->imageCommentAction     = d->optionsMenu->addAction(i18n("Image Comment"));
    d->imageCommentAction->setCheckable(true);
    d->tagNameAction          = d->optionsMenu->addAction(i18n("Tag Name"));
    d->tagNameAction->setCheckable(true);
    d->albumNameAction        = d->optionsMenu->addAction(i18n("Album Name"));
    d->albumNameAction->setCheckable(true);
    d->imageAspectRatioAction = d->optionsMenu->addAction(i18n("Image Aspect Ratio"));
    d->imageAspectRatioAction->setCheckable(true);
    d->imagePixelSizeAction   = d->optionsMenu->addAction(i18n("Image Pixel Size"));
    d->imagePixelSizeAction->setCheckable(true);
    d->optionsBtn->setMenu(d->optionsMenu);

    setMargin(0);
    setSpacing(0);

    connect(d->searchTextBar, SIGNAL(signalSearchTextSettings(SearchTextSettings)),
            this, SLOT(slotSearchFieldsChanged()));

    connect(d->optionsMenu, SIGNAL(triggered(QAction*)),
            this, SLOT(slotSearchFieldsChanged()));
}

TextFilter::~TextFilter()
{
    delete d;
}

SearchTextBar* TextFilter::searchTextBar() const
{
    return d->searchTextBar;
}

SearchTextFilterSettings::TextFilterFields TextFilter::searchTextFields()
{
    int fields = SearchTextFilterSettings::None;

    if (d->imageNameAction->isChecked())
    {
        fields |= SearchTextFilterSettings::ImageName;
    }
    if (d->imageTitleAction->isChecked())
    {
        fields |= SearchTextFilterSettings::ImageTitle;
    }
    if (d->imageCommentAction->isChecked())
    {
        fields |= SearchTextFilterSettings::ImageComment;
    }
    if (d->tagNameAction->isChecked())
    {
        fields |= SearchTextFilterSettings::TagName;
    }
    if (d->albumNameAction->isChecked())
    {
        fields |= SearchTextFilterSettings::AlbumName;
    }
    if (d->imageAspectRatioAction->isChecked())
    {
        fields |= SearchTextFilterSettings::ImageAspectRatio;
    }
    if (d->imagePixelSizeAction->isChecked())
    {
        fields |= SearchTextFilterSettings::ImagePixelSize;
    }

    return (SearchTextFilterSettings::TextFilterFields)fields;
}

void TextFilter::setsearchTextFields(SearchTextFilterSettings::TextFilterFields fields)
{
    d->imageNameAction->setChecked(fields & SearchTextFilterSettings::ImageName);
    d->imageTitleAction->setChecked(fields & SearchTextFilterSettings::ImageTitle);
    d->imageCommentAction->setChecked(fields & SearchTextFilterSettings::ImageComment);
    d->tagNameAction->setChecked(fields & SearchTextFilterSettings::TagName);
    d->albumNameAction->setChecked(fields & SearchTextFilterSettings::AlbumName);
    d->imageAspectRatioAction->setChecked(fields & SearchTextFilterSettings::ImageAspectRatio);
    d->imagePixelSizeAction->setChecked(fields & SearchTextFilterSettings::ImagePixelSize);
}

void TextFilter::slotSearchFieldsChanged()
{
    SearchTextFilterSettings settings(d->searchTextBar->searchTextSettings());
    settings.textFields = searchTextFields();

    emit signalSearchTextFilterSettings(settings);
}

void TextFilter::reset()
{
    d->searchTextBar->setText(QString());
    setsearchTextFields(SearchTextFilterSettings::All);
}

}  // namespace Digikam
