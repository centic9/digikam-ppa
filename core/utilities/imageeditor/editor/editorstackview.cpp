/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2008-08-20
 * Description : A widget stack to embed editor view.
 *
 * Copyright (C) 2008-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "editorstackview.moc"

// Local includes

#include "dzoombar.h"
#include "canvas.h"
#include "thumbnailsize.h"
#include "graphicsdimgview.h"
#include "previewlayout.h"
#include "previewwidget.h"

namespace Digikam
{

class EditorStackView::Private
{

public:

    Private() :
        toolView(0),
        canvas(0)
    {
    }

    QWidget* toolView;
    Canvas*  canvas;
};

EditorStackView::EditorStackView(QWidget* const parent)
    : QStackedWidget(parent), d(new Private)
{
}

EditorStackView::~EditorStackView()
{
    delete d;
}

void EditorStackView::setCanvas(Canvas* const canvas)
{
    if (d->canvas)
    {
        return;
    }

    d->canvas = canvas;
    insertWidget(CanvasMode, d->canvas);

    connect(d->canvas, SIGNAL(signalZoomChanged(double)),
            this, SLOT(slotZoomChanged(double)));

    connect(d->canvas, SIGNAL(signalToggleOffFitToWindow()),
            this, SIGNAL(signalToggleOffFitToWindow()));
}

Canvas* EditorStackView::canvas() const
{
    return d->canvas;
}

void EditorStackView::setToolView(QWidget* const view)
{
    if (d->toolView)
    {
        removeWidget(d->toolView);
    }

    d->toolView = view;

    if (d->toolView)
    {
        insertWidget(ToolViewMode, d->toolView);
    }

    // TODO: to remove when PreviewWidget will be unused...
    PreviewWidget* const old_preview = previewWidget_old();

    if (old_preview)
    {
        connect(old_preview, SIGNAL(signalZoomFactorChanged(double)),
                this, SLOT(slotZoomChanged(double)));

        connect(old_preview, SIGNAL(signalToggleOffFitToWindow()),
                this, SIGNAL(signalToggleOffFitToWindow()));

        return;
    }

    GraphicsDImgView* const preview = previewWidget();

    if (preview)
    {
        connect(preview->layout(), SIGNAL(zoomFactorChanged(double)),
                this, SLOT(slotZoomChanged(double)));

        connect(preview->layout(), SIGNAL(fitToWindowToggled(bool)),
                this, SLOT(slotToggleOffFitToWindow(bool)));
    }
}

QWidget* EditorStackView::toolView() const
{
    return d->toolView;
}

int EditorStackView::viewMode() const
{
    return indexOf(currentWidget());
}

void EditorStackView::setViewMode(int mode)
{
    if (mode != CanvasMode && mode != ToolViewMode)
    {
        return;
    }

    setCurrentIndex(mode);
}

void EditorStackView::increaseZoom()
{
    if (viewMode() == CanvasMode)
    {
        d->canvas->slotIncreaseZoom();
    }
    else
    {
        PreviewWidget* const old_preview = previewWidget_old();

        if (old_preview)
        {
            old_preview->slotIncreaseZoom();
            return;
        }

        GraphicsDImgView* const preview = previewWidget();

        if (preview)
        {
            preview->layout()->increaseZoom();
        }
    }
}

void EditorStackView::decreaseZoom()
{
    if (viewMode() == CanvasMode)
    {
        d->canvas->slotDecreaseZoom();
    }
    else
    {
        PreviewWidget* const old_preview = previewWidget_old();

        if (old_preview)
        {
            old_preview->slotDecreaseZoom();
            return;
        }

        GraphicsDImgView* const preview = previewWidget();

        if (preview)
        {
            preview->layout()->decreaseZoom();
        }
    }
}

void EditorStackView::toggleFitToWindow()
{
    // Fit to window action is common place to switch view in this mode.
    // User want to see the same behavors between canvas and tool preview.
    // Both are toggle at the same time.
    d->canvas->toggleFitToWindow();

    PreviewWidget* const old_preview = previewWidget_old();

    if (old_preview)
    {
        old_preview->toggleFitToWindow();
        return;
    }

    GraphicsDImgView* const preview = previewWidget();

    if (preview)
    {
        preview->layout()->toggleFitToWindow();
    }
}

void EditorStackView::fitToSelect()
{
    if (viewMode() == CanvasMode)
    {
        d->canvas->fitToSelect();
    }
}

void EditorStackView::zoomTo100Percent()
{
    if (viewMode() == CanvasMode)
    {
        d->canvas->setZoomFactor(1.0);
    }
    else
    {
        PreviewWidget* const old_preview = previewWidget_old();

        if (old_preview)
        {
            old_preview->setZoomFactor(1.0);
            return;
        }

        GraphicsDImgView* const preview = previewWidget();

        if (preview)
        {
            preview->layout()->setZoomFactor(1.0);
        }
    }
}

void EditorStackView::setZoomFactor(double zoom)
{
    if (viewMode() == CanvasMode)
    {
        d->canvas->setZoomFactor(zoom);
    }
    else
    {
        PreviewWidget* const old_preview = previewWidget_old();

        if (old_preview)
        {
            old_preview->setZoomFactor(zoom);
            return;
        }

        GraphicsDImgView* const preview = previewWidget();

        if (preview)
        {
            preview->layout()->setZoomFactor(zoom);
        }
    }
}

double EditorStackView::zoomMax() const
{
    if (viewMode() == CanvasMode)
    {
        return d->canvas->zoomMax();
    }
    else
    {
        GraphicsDImgView* const preview  = previewWidget();
        PreviewWidget* const old_preview = previewWidget_old();

        if (old_preview)
        {
            return old_preview->zoomMax();
        }
        else if (preview)
        {
            return preview->layout()->maxZoomFactor();
        }
        else
        {
            return -1.0;
        }
    }
}

double EditorStackView::zoomMin() const
{
    if (viewMode() == CanvasMode)
    {
        return d->canvas->zoomMin();
    }
    else
    {
        GraphicsDImgView* const preview  = previewWidget();
        PreviewWidget* const old_preview = previewWidget_old();

        if (old_preview)
        {
            return old_preview->zoomMin();
        }
        else if (preview)
        {
            return preview->layout()->minZoomFactor();
        }
        else
        {
            return -1.0;
        }
    }
}

void EditorStackView::slotZoomSliderChanged(int size)
{
    if (viewMode() == ToolViewMode && !isZoomablePreview())
    {
        return;
    }

    double z = DZoomBar::zoomFromSize(size, zoomMin(), zoomMax());

    if (viewMode() == CanvasMode)
    {
        d->canvas->setZoomFactorSnapped(z);
    }
    else
    {
        PreviewWidget* const old_preview = previewWidget_old();

        if (old_preview)
        {
            old_preview->setZoomFactorSnapped(z);
            return;
        }

        GraphicsDImgView* const preview = previewWidget();

        if (preview)
        {
            return preview->layout()->setZoomFactorSnapped(z);
        }
    }
}

void EditorStackView::slotZoomChanged(double zoom)
{
    bool max, min;

    if (viewMode() == CanvasMode)
    {
        max = d->canvas->maxZoom();
        min = d->canvas->minZoom();
        emit signalZoomChanged(max, min, zoom);
    }
    else
    {
        PreviewWidget* const old_preview = previewWidget_old();

        if (old_preview)
        {
            max = old_preview->maxZoom();
            min = old_preview->minZoom();
            emit signalZoomChanged(max, min, zoom);
            return;
        }

        GraphicsDImgView* const preview = previewWidget();

        if (preview)
        {
            max = preview->layout()->atMaxZoom();
            min = preview->layout()->atMinZoom();
            emit signalZoomChanged(max, min, zoom);
        }
    }
}

void EditorStackView::slotToggleOffFitToWindow(bool b)
{
    if (b)
    {
        emit signalToggleOffFitToWindow();
    }
}

GraphicsDImgView* EditorStackView::previewWidget() const
{
    GraphicsDImgView* const preview = dynamic_cast<GraphicsDImgView*>(d->toolView);

    if (preview)
    {
        return preview;
    }

    return 0;
}

PreviewWidget* EditorStackView::previewWidget_old() const
{
    PreviewWidget* const preview = dynamic_cast<PreviewWidget*>(d->toolView);

    if (preview)
    {
        return preview;
    }

    return 0;
}

bool EditorStackView::isZoomablePreview() const
{
    return (previewWidget_old() || previewWidget());
}

}  // namespace Digikam
