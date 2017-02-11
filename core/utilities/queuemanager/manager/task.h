/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2009-02-06
 * Description : Thread actions task.
 *
 * Copyright (C) 2009-2017 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012      by Pankaj Kumar <me at panks dot me>
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

#ifndef TASK_H
#define TASK_H

// Qt includes

#include <QUrl>

// Local includes

#include "actions.h"
#include "queuesettings.h"
#include "batchtoolutils.h"
#include "actionthreadbase.h"

namespace Digikam
{

class Task : public ActionJob
{
    Q_OBJECT

public:

    Task();
    ~Task();

    void setSettings(const QueueSettings& settings);
    void setItem(const AssignedBatchTools& tools);

Q_SIGNALS:

    void signalStarting(const Digikam::ActionData& ad);
    void signalFinished(const Digikam::ActionData& ad);

public Q_SLOTS:

    void slotCancel();

protected:

    void run();

private:

    void emitActionData(ActionData::ActionStatus st, const QString& mess=QString(), const QUrl& dest=QUrl());

private:

    class Private;
    Private* const d;
};

}  // namespace Digikam

#endif /* TASK_H */
