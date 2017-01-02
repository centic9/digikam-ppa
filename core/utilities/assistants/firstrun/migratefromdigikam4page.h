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

#ifndef MIGRATEFROMDIGIKAM4_PAGE_H
#define MIGRATEFROMDIGIKAM4_PAGE_H

// Local includes

#include "firstrundlgpage.h"

class QRadioButton;
class QButtonGroup;
namespace Digikam
{

class MigrateFromDigikam4Page : public FirstRunDlgPage
{
Q_OBJECT

public:

    explicit MigrateFromDigikam4Page(FirstRunDlg* const dlg);
    ~MigrateFromDigikam4Page();

    /**
     * Returns true if the user selected to do a migration
     */
    bool isMigrationChecked() const;
    void doMigration();
    int nextId() const;

public Q_SLOTS:
    void migrationToggled(bool b);

protected:
    QButtonGroup *m_migrateBehavior;
    QRadioButton *m_migrate;
    QRadioButton *m_createnew;
};

}   // namespace Digikam

#endif /* MIGRATEFROMDIGIKAM4_PAGE_H */
