/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2002-07-28
 * Description : main program from digiKam
 *
 * Copyright (C) 2002-2006 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2002-2017 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "digikam_config.h"

// Qt includes

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QSqlDatabase>
#include <QString>
#include <QStringList>
#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QMessageBox>

// KDE includes

#include <klocalizedstring.h>
#include <kaboutdata.h>

// Local includes

#include "metaengine.h"
#include "digikam_debug.h"
#include "dmessagebox.h"
#include "albummanager.h"
#include "firstrundlg.h"
#include "collectionlocation.h"
#include "collectionmanager.h"
#include "daboutdata.h"
#include "dbengineparameters.h"
#include "digikamapp.h"
#include "scancontroller.h"
#include "coredbaccess.h"
#include "thumbsdbaccess.h"
#include "facedbaccess.h"
#include "dxmlguiwindow.h"
#include "digikam_version.h"

using namespace Digikam;
using namespace FacesEngine;

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // if we have some local breeze icon resource, prefer it
    DXmlGuiWindow::setupIconTheme();

    KLocalizedString::setApplicationDomain("digikam");

    KAboutData aboutData(QString::fromLatin1("digikam"), // component name
                         i18n("digiKam"),                // display name
                         digiKamVersion());

    aboutData.setShortDescription(DAboutData::digiKamSlogan());;
    aboutData.setLicense(KAboutLicense::GPL);
    aboutData.setCopyrightStatement(DAboutData::copyright());
    aboutData.setOtherText(additionalInformation());
    aboutData.setHomepage(DAboutData::webProjectUrl().url());

    DAboutData::authorsRegistration(aboutData);

    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);
    parser.addVersionOption();
    parser.addHelpOption();
    aboutData.setupCommandLine(&parser);
    parser.addOption(QCommandLineOption(QStringList() <<  QLatin1String("download-from"),      i18n("Open camera dialog at <path>"),                                             QLatin1String("path")));
    parser.addOption(QCommandLineOption(QStringList() <<  QLatin1String("download-from-udi"),  i18n("Open camera dialog for the device with Solid UDI <udi>"),                   QLatin1String("udi")));
    parser.addOption(QCommandLineOption(QStringList() <<  QLatin1String("detect-camera"),      i18n("Automatically detect and open a connected gphoto2 camera")));
    parser.addOption(QCommandLineOption(QStringList() <<  QLatin1String("database-directory"), i18n("Start digikam with the SQLite database file found in the directory <dir>"), QLatin1String("dir")));

    parser.process(app);
    aboutData.processCommandLine(&parser);

    MetaEngine::initializeExiv2();

    // Check if Qt database plugins are available.

    if (!QSqlDatabase::isDriverAvailable(DbEngineParameters::SQLiteDatabaseType()) &&
        !QSqlDatabase::isDriverAvailable(DbEngineParameters::MySQLDatabaseType()))
    {
        if (QSqlDatabase::drivers().isEmpty())
        {
            QMessageBox::critical(qApp->activeWindow(),
                                  qApp->applicationName(),
                                  i18n("Run-time Qt SQLite or MySQL database plugin is not available. "
                                       "please install it.\n"
                                       "There is no database plugin installed on your computer."));
        }
        else
        {
            DMessageBox::showInformationList(QMessageBox::Warning,
                                             qApp->activeWindow(),
                                             qApp->applicationName(),
                                             i18n("Run-time Qt SQLite or MySQL database plugin are not available. "
                                                  "Please install it.\n"
                                                  "Database plugins installed on your computer are listed below."),
                                             QSqlDatabase::drivers());
        }

        qCDebug(DIGIKAM_GENERAL_LOG) << "QT Sql drivers list: " << QSqlDatabase::drivers();
        return 1;
    }

    QString commandLineDBPath;

    if (parser.isSet(QLatin1String("database-directory")))
    {
        QDir commandLineDBDir(parser.value(QLatin1String("database-directory")));

        if (!commandLineDBDir.exists())
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "The given database-directory does not exist or is not readable. Ignoring." << commandLineDBDir.path();
        }
        else
        {
            commandLineDBPath = commandLineDBDir.path();
        }
    }

    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(QLatin1String("General Settings"));
    QString version           = group.readEntry(QLatin1String("Version"), QString());
    QString iconTheme         = group.readEntry(QLatin1String("Icon Theme"), QString());
    KConfigGroup mainConfig   = config->group(QLatin1String("Album Settings"));

    QString            firstAlbumPath;
    DbEngineParameters params;

    // Run the first run assistant if we have no or very old config
    if (!mainConfig.exists() || (version.startsWith(QLatin1String("0.5"))))
    {
        FirstRunDlg firstRun;
        firstRun.show();

        if (firstRun.exec() == QDialog::Rejected)
        {
            return 1;
        }

        // parameters are written to config
        firstAlbumPath = firstRun.firstAlbumPath();

        if (firstRun.getDbEngineParameters().isSQLite())
        {
            AlbumManager::checkDatabaseDirsAfterFirstRun(firstRun.getDbEngineParameters().getCoreDatabaseNameOrDir(), firstAlbumPath);
        }
    }

    if (!commandLineDBPath.isNull())
    {
        // command line option set?
        params = DbEngineParameters::parametersForSQLiteDefaultFile(commandLineDBPath);
    }
    else
    {
        params = DbEngineParameters::parametersFromConfig(config);
        params.legacyAndDefaultChecks(firstAlbumPath);
        // sync to config, for all first-run or upgrade situations
        params.writeToConfig(config);
    }

    // initialize database
    AlbumManager::instance()->setDatabase(params, !commandLineDBPath.isNull(), firstAlbumPath);

    if (!iconTheme.isEmpty())
    {
        QIcon::setThemeName(iconTheme);
    }

    // create main window
    DigikamApp* const digikam = new DigikamApp();

    // If application storage place in home directory to save customized XML settings files do not exist, create it,
    // else QFile will not able to create new files as well.
    if (!QFile::exists(QStandardPaths::writableLocation(QStandardPaths::DataLocation)))
    {
        QDir().mkpath(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    }
    // If application cache place in home directory to save cached files do not exist, create it.
    if (!QFile::exists(QStandardPaths::writableLocation(QStandardPaths::CacheLocation)))
    {
        QDir().mkpath(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    }

    // Bug #247175:
    // Add a connection to the destroyed() signal when the digiKam mainwindow has been
    // closed. This should prevent digiKam from staying open in the background.
    //
    // Right now this is the easiest and cleanest fix for the described problem, but we might re-think the
    // solution later on, just in case there are better ways to do it.
    QObject::connect(digikam, SIGNAL(destroyed(QObject*)),
                     &app, SLOT(quit()));

    digikam->restoreSession();
    digikam->show();

    if (parser.isSet(QLatin1String("download-from")))
    {
        digikam->downloadFrom(parser.value(QLatin1String("download-from")));
    }
    else if (parser.isSet(QLatin1String("download-from-udi")))
    {
        digikam->downloadFromUdi(parser.value(QLatin1String("download-from-udi")));
    }
    else if (parser.isSet(QLatin1String("detect-camera")))
    {
        digikam->autoDetect();
    }

    int ret = app.exec();

    CoreDbAccess::cleanUpDatabase();
    ThumbsDbAccess::cleanUpDatabase();
    FaceDbAccess::cleanUpDatabase();
    MetaEngine::cleanupExiv2();

    return ret;
}
