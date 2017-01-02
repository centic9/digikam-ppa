/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2009-11-14
 * Description : database settings widget
 *
 * Copyright (C) 2009-2010 by Holger Foerster <Hamsi2k at freenet dot de>
 * Copyright (C) 2010-2016 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DATABASE_SETTINGS_WIDGET_H
#define DATABASE_SETTINGS_WIDGET_H

// Qt includes

#include <QString>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QUrl>
#include <QCheckBox>

// Local includes

#include "digikam_export.h"
#include "dwidgetutils.h"
#include "applicationsettings.h"
#include "dbengineparameters.h"

namespace Digikam
{

class DIGIKAM_EXPORT DatabaseSettingsWidget : public QWidget
{
    Q_OBJECT

public:

    enum DatabaseType
    {
        SQlite          = 0,
        MariadbInternal = 1,
        MysqlInternal   = 2,
        MysqlServer     = 3
    };

public:

    explicit DatabaseSettingsWidget(QWidget* const parent = 0);
    ~DatabaseSettingsWidget();

public:

    void setParametersFromSettings(const ApplicationSettings* const settings);
    DbEngineParameters getDbEngineParameters() const;

    void    setDatabaseType(int type);
    int     databaseType()    const;

    QString databaseBackend() const;

    void setDatabasePath(const QString& path);
    QString databasePath() const;

    DbEngineParameters orgDatabasePrm() const;

    /**
     * For Sqlite or MysqlInternal, check properties of local path to store database files.
     * For MysqlServer, check the network connection and database names.
     */
    bool checkDatabaseSettings();

private:

    void setupMainArea();
    void handleInternalServer(int index);
    void setDatabaseInputFields(int index);
    bool checkMysqlServerConnection(QString& error);
    bool checkMysqlServerConnectionConfig(QString& error);
    bool checkMysqlServerDbNamesConfig(QString& error);
    bool checkDatabasePath();

private Q_SLOTS:

    void slotHandleDBTypeIndexChanged(int index);
    void slotDatabasePathEditedDelayed();
    void slotDatabasePathEdited();
    void slotUpdateSqlInit();
    void slotCheckMysqlServerConnection();
    void slotResetMysqlServerDBNames();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif  // DATABASE_SETTINGS_WIDGET_H
