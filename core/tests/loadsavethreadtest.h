/** ===========================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * @date   2012-10-23
 * @brief  a command line tool to test DImg image loader
 *
 * @author Copyright (C) 2012-2014 by Gilles Caulier
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

#ifndef LOADSAVETHREADTEST_H
#define LOADSAVETHREADTEST_H

// Qt includes

#include <QString>
#include <QApplication>

// Local includes

#include "dimg.h"
#include "loadingdescription.h"
#include "loadsavethread.h"

using namespace Digikam;

class LoadSaveThreadTest : public QApplication
{
    Q_OBJECT

public:

    LoadSaveThreadTest(int& argc, char** argv);

private Q_SLOTS:

    void slotImageLoaded(const LoadingDescription&, const DImg&);
    void slotImageSaved(const QString&, bool);
    void slotLoadingProgress(const LoadingDescription&, float);
    void slotSavingProgress(const QString&, float);

private:

    LoadSaveThread* m_thread;
};

#endif /* LOADSAVETHREADTEST_H */
