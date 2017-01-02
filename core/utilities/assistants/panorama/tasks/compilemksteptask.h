/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2012-03-15
 * Description : a plugin to create panorama by fusion of several images.
 *
 * Copyright (C) 2012-2015 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#ifndef COMPILEMKSTEPTASK_H
#define COMPILEMKSTEPTASK_H

// Local includes

#include "commandtask.h"

namespace Digikam
{

class CompileMKStepTask : public CommandTask
{
public:

    CompileMKStepTask(const QString& workDirPath, int id, const QUrl& mkUrl,
                      const QString& nonaPath, const QString& enblendPath,
                      const QString& makePath, bool preview);
    ~CompileMKStepTask();

public:

    const int                           id;

protected:

    void run(ThreadWeaver::JobPointer self, ThreadWeaver::Thread *thread) override;

private:

    const QUrl&                         mkUrl;
    const QString                       nonaPath;
    const QString                       enblendPath;
};

}  // namespace Digikam

#endif /* COMPILEMKSTEPTASK_H */
