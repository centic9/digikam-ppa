/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2008-12-10
 * Description : tool tip widget for iconview, thumbbar, and folderview items
 *
 * Copyright (C) 2008-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "ditemtooltip.h"

// Qt includes

#include <QApplication>
#include <QDateTime>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QStyle>
#include <QStyleHintReturnMask>
#include <QStyleOptionFrame>
#include <QStylePainter>
#include <QToolTip>
#include <QVBoxLayout>
#include <QTextDocument>
#include <QByteArray>
#include <QBuffer>

// KDE includes

#include <klocale.h>
#include <kdebug.h>
#include <kglobalsettings.h>
#include <kglobal.h>
#include <kdeversion.h>
#include <kapplication.h>

namespace Digikam
{

DToolTipStyleSheet::DToolTipStyleSheet(const QFont& font)
    : maxStringLength(30)
{
    unavailable = i18n("unavailable");

    tipHeader   = QString("<qt><table cellspacing=\"0\" cellpadding=\"0\" width=\"250\" border=\"0\">");
    tipFooter   = QString("</table></qt>");

    headBeg     = QString("<tr bgcolor=\"%1\"><td colspan=\"2\">"
                          "<nobr><font size=\"-1\" color=\"%2\" face=\"%3\"><center><b>")
                  .arg(kapp->palette().color(QPalette::Base).name())
                  .arg(kapp->palette().color(QPalette::Text).name())
                  .arg(font.family());
    headEnd     = QString("</b></center></font></nobr></td></tr>");

    cellBeg     = QString("<tr><td><nobr><font size=\"-1\" color=\"%1\" face=\"%2\">")
                  .arg(kapp->palette().color(QPalette::ToolTipText).name())
                  .arg(font.family());
    cellMid     = QString("</font></nobr></td><td><nobr><font size=\"-1\" color=\"%1\" face=\"%2\">")
                  .arg(kapp->palette().color(QPalette::ToolTipText).name())
                  .arg(font.family());
    cellEnd     = QString("</font></nobr></td></tr>");

    cellSpecBeg = QString("<tr><td><nobr><font size=\"-1\" color=\"%1\" face=\"%2\">")
                  .arg(kapp->palette().color(QPalette::ToolTipText).name())
                  .arg(font.family());
    cellSpecMid = QString("</font></nobr></td><td><nobr><font size=\"-1\" color=\"%1\" face=\"%2\"><i>")
                  .arg(kapp->palette().color(QPalette::ToolTipText).name())
                  .arg(font.family());
    cellSpecEnd = QString("</i></font></nobr></td></tr>");
}

QString DToolTipStyleSheet::breakString(const QString& input) const
{
    QString str = input.simplified();
    str         = Qt::escape(str);

    if (str.length() <= maxStringLength)
    {
        return str;
    }

    QString br;

    int i     = 0;
    int count = 0;

    while (i < str.length())
    {
        if (count >= maxStringLength && str.at(i).isSpace())
        {
            count = 0;
            br.append("<br/>");
        }
        else
        {
            br.append(str.at(i));
        }

        ++i;
        ++count;
    }

    return br;
}

QString DToolTipStyleSheet::elidedText(const QString& str, Qt::TextElideMode elideMode) const
{
    if (str.length() <= maxStringLength)
    {
        return str;
    }

    switch (elideMode)
    {
        case Qt::ElideLeft:
            return "..." + str.right(maxStringLength-3);
        case Qt::ElideRight:
            return str.left(maxStringLength-3) + "...";
        case Qt::ElideMiddle:
            return str.left(maxStringLength / 2 - 2) + "..." + str.right(maxStringLength / 2 - 1);
        case Qt::ElideNone:
            return str.left(maxStringLength);
        default:
            return str;
    }
}

QString DToolTipStyleSheet::imageAsBase64(const QImage& img) const
{
    QByteArray byteArray;
    QBuffer    buffer(&byteArray);
    img.save(&buffer, "PNG");
    QString    iconBase64 = QString::fromLatin1(byteArray.toBase64().data());
    return QString("<img src=\"data:image/png;base64,%1\">").arg(iconBase64);
}

// --------------------------------------------------------------------------------------------------

class DItemToolTip::Private
{
public:

    Private() :
        tipBorder(5)
    {
        corner = 0;
    }

    const int   tipBorder;
    int         corner;
    QPixmap     corners[4];
};

DItemToolTip::DItemToolTip(QWidget* const parent)
    : QLabel(parent, Qt::ToolTip), d(new Private)
{
    hide();

    setBackgroundRole(QPalette::ToolTipBase);
    setPalette(QToolTip::palette());
    ensurePolished();
    setMargin(qMax(d->tipBorder, 1 + style()->pixelMetric(QStyle::PM_ToolTipLabelFrameWidth, 0, this)));
    setWindowOpacity(style()->styleHint(QStyle::SH_ToolTipLabel_Opacity, 0, this) / 255.0);
    setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    setFrameStyle(QFrame::StyledPanel);
/*
    Old-style box:
    setFrameStyle(QFrame::Plain | QFrame::Box);
    setLineWidth(1);
*/
    renderArrows();
}

DItemToolTip::~DItemToolTip()
{
    delete d;
}

void DItemToolTip::updateToolTip()
{
    renderArrows();

    QString contents = tipContents();
    //setWordWrap(Qt::mightBeRichText(contents));
    setText(contents);
    resize(sizeHint());
}

bool DItemToolTip::toolTipIsEmpty() const
{
    return(text().isEmpty());
}

void DItemToolTip::reposition()
{
    QRect rect = repositionRect();

    if (rect.isNull())
    {
        return;
    }

    QPoint pos = rect.center();
    // d->corner:
    // 0: upperleft
    // 1: upperright
    // 2: lowerleft
    // 3: lowerright

    d->corner = 0;
    // should the tooltip be shown to the left or to the right of the ivi ?

    QRect desk = KGlobalSettings::desktopGeometry(rect.center());

    if (rect.center().x() + width() > desk.right())
    {
        // to the left
        if (pos.x() - width() < 0)
        {
            pos.setX(0);
            d->corner = 4;
        }
        else
        {
            pos.setX( pos.x() - width() );
            d->corner = 1;
        }
    }

    // should the tooltip be shown above or below the ivi ?
    if (rect.bottom() + height() > desk.bottom())
    {
        // above
        pos.setY( rect.top() - height() - 5);
        d->corner += 2;
    }
    else
    {
        pos.setY( rect.bottom() + 5 );
    }

    move( pos );
}

void DItemToolTip::renderArrows()
{
    int w = d->tipBorder;

    // -- left top arrow -------------------------------------

    QPixmap& pix0 = d->corners[0];
    pix0          = QPixmap(w, w);
    pix0.fill(Qt::transparent);

    QPainter p0(&pix0);
    p0.setPen(QPen(kapp->palette().color(QPalette::Text), 1));

    for (int j=0; j<w; ++j)
    {
        p0.drawLine(0, j, w-j-1, j);
    }

    p0.end();

    // -- right top arrow ------------------------------------

    QPixmap& pix1 = d->corners[1];
    pix1          = QPixmap(w, w);
    pix1.fill(Qt::transparent);

    QPainter p1(&pix1);
    p1.setPen(QPen(kapp->palette().color(QPalette::Text), 1));

    for (int j=0; j<w; ++j)
    {
        p1.drawLine(j, j, w-1, j);
    }

    p1.end();

    // -- left bottom arrow ----------------------------------

    QPixmap& pix2 = d->corners[2];
    pix2          = QPixmap(w, w);
    pix2.fill(Qt::transparent);

    QPainter p2(&pix2);
    p2.setPen(QPen(kapp->palette().color(QPalette::Text), 1));

    for (int j=0; j<w; ++j)
    {
        p2.drawLine(0, j, j, j);
    }

    p2.end();

    // -- right bottom arrow ---------------------------------

    QPixmap& pix3 = d->corners[3];
    pix3          = QPixmap(w, w);
    pix3.fill(Qt::transparent);

    QPainter p3(&pix3);
    p3.setPen(QPen(kapp->palette().color(QPalette::Text), 1));

    for (int j=0; j<w; ++j)
    {
        p3.drawLine(w-j-1, j, w-1, j);
    }

    p3.end();
}

bool DItemToolTip::event(QEvent* e)
{
    switch ( e->type() )
    {
        case QEvent::Leave:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::FocusIn:
        case QEvent::FocusOut:
        case QEvent::Wheel:
            hide();
            break;
        default:
            break;
    }

    return QFrame::event(e);
}

void DItemToolTip::resizeEvent(QResizeEvent* e)
{
    QStyleHintReturnMask frameMask;
    QStyleOption option;
    option.init(this);

    if (style()->styleHint(QStyle::SH_ToolTip_Mask, &option, this, &frameMask))
    {
        setMask(frameMask.region);
    }

    update();
    QLabel::resizeEvent(e);
}

void DItemToolTip::paintEvent(QPaintEvent* e)
{
    {
        QStylePainter p(this);
        QStyleOptionFrame opt;
        opt.init(this);
        p.drawPrimitive(QStyle::PE_PanelTipLabel, opt);
    }

    QLabel::paintEvent(e);

    QPainter p(this);

    if (d->corner >= 4)
    {
        return;
    }

    QPixmap& pix = d->corners[d->corner];

    switch (d->corner)
    {
        case 0:
            p.drawPixmap( 3, 3, pix );
            break;
        case 1:
            p.drawPixmap( width() - pix.width() - 3, 3, pix );
            break;
        case 2:
            p.drawPixmap( 3, height() - pix.height() - 3, pix );
            break;
        case 3:
            p.drawPixmap( width() - pix.width() - 3, height() - pix.height() - 3, pix );
            break;
    }
}

}  // namespace Digikam
