/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-10-12
 * Description : IPTC credits settings page.
 *
 * Copyright (C) 2006-2017 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef IPTC_CREDITS_H
#define IPTC_CREDITS_H

// Qt includes

#include <QWidget>
#include <QByteArray>

namespace Digikam
{

class IPTCCredits : public QWidget
{
    Q_OBJECT

public:

    explicit IPTCCredits(QWidget* const parent);
    ~IPTCCredits();

    void applyMetadata(QByteArray& iptcData);
    void readMetadata(QByteArray& iptcData);

Q_SIGNALS:

    void signalModified();

private:

    class Private;
    Private* const d;
};

}  // namespace Digikam

#endif // IPTC_CREDITS_H
