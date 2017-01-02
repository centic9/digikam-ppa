/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2016-09-29
 * Description : migration page from digikam4
 *
 * Copyright (C) 2016 by Antonio Larrosa <alarrosa at suse dot com>
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

#include "migratefromdigikam4page.h"

// Qt includes

#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QStringList>
#include <QVBoxLayout>
#include <QApplication>
#include <QStyle>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

// KDE includes

#include <klocalizedstring.h>
#include <kdelibs4configmigrator.h>
#include <kdelibs4migration.h>

// Local includes

#include "digikam_debug.h"
#include "dbengineparameters.h"
#include "dwidgetutils.h"

namespace Digikam
{

MigrateFromDigikam4Page::MigrateFromDigikam4Page(FirstRunDlg* const dlg)
    : FirstRunDlgPage(dlg, i18n("Migration from digiKam 4") ),
      m_migrateBehavior(0L), m_migrate(0L), m_createnew(0L)
{
    const int spacing = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);

    DVBox* const vbox    = new DVBox(this);
    QLabel* const title = new QLabel(vbox);
    title->setWordWrap(true);
    title->setText(i18n("<qt>"
                        "<p><h1><b>Migrate configuration and metadata from digiKam 4</b></h1></p>"
                        "<p>You can choose here if you want to use the configuration and albums from digiKam 4 in digiKam 5. "
                        "Please note the following warnings:</p>"
                        "<p>Migration is done <b>at your own risk</b>. Digikam developers "
                        "don't recommend it and don't support it. On the other hand, creating "
                        "a new configuration might result in loss of tags and other metadata that wasn't embedded inside "
                        "the pictures and was only available in digiKam 4's database.</p>"
                        "<p>In either case you're recommended to backup "
                        "the configuration files and databases before proceeding.</p>"
                        "</qt>"));

    QWidget* const btns      = new QWidget(vbox);
    QVBoxLayout* const vlay  = new QVBoxLayout(btns);

    m_migrateBehavior = new QButtonGroup(btns);
    m_migrate     = new QRadioButton(btns);
    m_migrate->setText(i18n("Migrate configuration from digiKam 4"));
    m_migrate->setChecked(true);
    connect(m_migrate, SIGNAL(toggled(bool)), this, SLOT(migrationToggled(bool)) );
    m_migrateBehavior->addButton(m_migrate);

    m_createnew = new QRadioButton(btns);
    m_createnew->setText(i18n("Create a new configuration"));
    m_migrateBehavior->addButton(m_createnew);

    vlay->addWidget(m_migrate);
    vlay->addWidget(m_createnew);
    vlay->setContentsMargins(spacing, spacing, spacing, spacing);
    vlay->setSpacing(spacing);

    connect(m_migrateBehavior, SIGNAL(buttonClicked(int)), this, SIGNAL(completeChanged()));

    setPageWidget(vbox);
}

MigrateFromDigikam4Page::~MigrateFromDigikam4Page()
{
}

void MigrateFromDigikam4Page::doMigration()
{
  // Migrate digiKam config files from $KDEHOME/share/config/
  Kdelibs4ConfigMigrator migrator(QStringLiteral("digikam"));
  QStringList configFiles;
  configFiles << QStringLiteral("digikamrc")
              << QStringLiteral("digikam_tagsmanagerrc")
              << QStringLiteral("kipipluginsrc")
              << QStringLiteral("kipirc")
              << QStringLiteral("showfotorc");
  migrator.setConfigFiles( configFiles );
  migrator.migrate();

  // Migrate digiKam config files from $KDEHOME/share/apps/digikam/
  Kdelibs4Migration migration;
  QString oldappdatadir = migration.locateLocal("data", QStringLiteral("digikam"));
  QStringList oldAppFiles = QDir(oldappdatadir).entryList( QDir::Files | QDir::Readable | QDir::NoDotAndDotDot );

  Q_FOREACH( const QString &configFileName, oldAppFiles)
  {
     const QString newConfigLocation
                = QStandardPaths::writableLocation(QStandardPaths::DataLocation)
                  + QLatin1Char('/') + configFileName;

     if (QFile(newConfigLocation).exists()) {
               qCDebug(DIGIKAM_GENERAL_LOG) << newConfigLocation << " already exists. Skipping";
               continue;
     }

     QFileInfo fileInfo(newConfigLocation);
     QDir().mkpath(fileInfo.absolutePath());

     const QString oldConfigFile = oldappdatadir + QLatin1Char('/') + configFileName;
     if (!oldConfigFile.isEmpty()) {
         if (QFile(oldConfigFile).copy(newConfigLocation)) {
             qCDebug(DIGIKAM_GENERAL_LOG) << "Config file" << oldConfigFile << "was migrated to" << newConfigLocation;
         }
     }
  }

  // Migrate $KDEHOME/share/apps/kipi/geobookmarks.xml to ./.local/share/digikam/geobookmarks.xml
  QString oldGeobookmarksFile = migration.locateLocal("data", QStringLiteral("kipi/geobookmarks.xml"));
  const QString newGeobookmarksFile = QStandardPaths::writableLocation(QStandardPaths::DataLocation)
                                    + QStringLiteral("/geobookmarks.xml");

  if (QFile(newGeobookmarksFile).exists()) {
     qCDebug(DIGIKAM_GENERAL_LOG) << newGeobookmarksFile << " already exists. Skipping";
  } else {
     QFileInfo fileInfo(newGeobookmarksFile);
     QDir().mkpath(fileInfo.absolutePath());

     if (!oldGeobookmarksFile.isEmpty()) {
         if (QFile(oldGeobookmarksFile).copy(newGeobookmarksFile)) {
             qCDebug(DIGIKAM_GENERAL_LOG) << "Config file" << oldGeobookmarksFile << "was migrated to" << newGeobookmarksFile;
         }
     }
  }

  // Fix albumroot identifier since digiKam 5 doesn't interpret correctly
  // values like volumeid:?path=%2Fhome%2Fantonio%2FPictures and it needs
  // to be url-decoded.
  DbEngineParameters parameters = DbEngineParameters::parametersFromConfig(KSharedConfig::openConfig());
  QSqlDatabase databaseHandler = QSqlDatabase::addDatabase(parameters.databaseType, QStringLiteral("digikam4migration"));

  databaseHandler.setHostName(parameters.hostName);
  databaseHandler.setPort(parameters.port);
  databaseHandler.setDatabaseName(parameters.databaseNameCore);
  databaseHandler.setUserName(parameters.userName);
  databaseHandler.setPassword(parameters.password);
  databaseHandler.setConnectOptions(parameters.connectOptions);
  if (!databaseHandler.open()) {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Cannot open database:" << databaseHandler.lastError().text();
        return;
  }

  QSqlQuery query(QStringLiteral("SELECT id,identifier FROM albumroots"), databaseHandler);
  while (query.next()) {
        int id = query.value(0).toInt();
        QString identifier = query.value(1).toString();

        if (identifier.startsWith(QStringLiteral("volumeid:?path=%2F")))
        {
           QUrl url(identifier);
           url.setQuery(url.query(QUrl::FullyDecoded), QUrl::DecodedMode);
           qCDebug(DIGIKAM_GENERAL_LOG) << "Updating albumroot " << id << " from " << identifier << " to " << url.toString();
           QSqlQuery uquery(QStringLiteral("UPDATE albumroots SET identifier=? WHERE id=?"), databaseHandler);
           uquery.bindValue(0, url.toString());
           uquery.bindValue(1, id);
           uquery.exec();
        }
    }
  databaseHandler.close();

  qCDebug(DIGIKAM_GENERAL_LOG) << "Migration finished";
}

bool MigrateFromDigikam4Page::isMigrationChecked() const
{
  return m_migrate->isChecked();
}

void MigrateFromDigikam4Page::migrationToggled(bool b)
{
  setFinalPage(b);
}

int MigrateFromDigikam4Page::nextId() const
{
  if (m_migrate->isChecked())
     return -1;
  else
     return QWizardPage::nextId();
}

}   // namespace Digikam
