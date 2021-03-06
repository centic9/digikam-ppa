/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-05-28
 * Description : a KIPI plugin to export pics through DLNA technology.
 *
 * Copyright (C) 2012 by Smit Mehta <smit dot meh at gmail dot com>
 * Copyright (C) 2011 by Tuomo Penttinen <tp at herqq dot org>
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

#ifndef MINIDLNA_CONFIG_H
#define MINIDLNA_CONFIG_H

// Qt includes

#include <QObject>
#include <QString>

// KDE includes

#include <kurl.h>

namespace KIPIDLNAExportPlugin
{

class MinidlnaServer : public QObject
{
    Q_OBJECT

public:

    MinidlnaServer(QObject* const parent=0);
    virtual ~MinidlnaServer();

    void setDirectories(const QStringList& directories);
    void setBinaryPath(const QString& path);
    void startMinidlnaServer();
    void generateConfigFile();

private:

    class Private;
    Private* const d;
};

} // namespace KIPIDLNAExportPlugin

#endif // MINIDLNA_CONFIG_H
