/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2010-06-21
 * @brief  CLI test program for trainign DB
 *
 * @author Copyright (C) 2014 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Qt includes

#include <QApplication>
#include <QDir>
#include <QImage>

// KDE includes

#include <kdebug.h>

// libkface includes

#include "libkface/recognitiondatabase.h"

using namespace KFaceIface;

// --------------------------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    RecognitionDatabase db = RecognitionDatabase::addDatabase(QDir::currentPath());

    QImage image(256, 256, QImage::Format_ARGB32);
    image.fill(Qt::red);

    Identity identity;

    // Populate database.

    for (int i=0 ; i < 100 ; i++)
    {
        QString name      = QString("face%1").arg(i);
        kDebug() << "Record Identity " << name << " to DB";
        QMap<QString, QString> attributes;
        attributes["name"] = name;
        identity           = db.addIdentity(attributes);
        db.train(identity, image, "test application");
    }

    // Check records in database.

    for (int i=0 ; i < 100 ; i++)
    {
        QString name = QString("face%1").arg(i);
        identity     = db.findIdentity("name", name);

        if (!identity.isNull())
        {
            kDebug() << "Identity " << name << " is present in DB";
        }
        else
        {
            kDebug() << "Identity " << name << " is absent in DB";
        }
    }

    // Process recognition in database.

    QList<Identity> list = db.recognizeFaces(QList<QImage>() << image);

    if (!list.empty())
    {
        foreach(Identity id, list)
        {
            kDebug() << "Identity " << id.attributes.value("name") << " recognized";
        }
    }
    else
    {
        kDebug() << "No Identity recognized from DB";
    }

    return 0;
}
