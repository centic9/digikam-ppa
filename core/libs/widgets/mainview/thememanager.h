/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2004-08-02
 * Description : theme manager
 *
 * Copyright (C) 2006-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

// Qt includes

#include <QObject>
#include <QPixmap>
#include <QString>

// KDE includes

#include <ksharedconfig.h>

// Local includes

#include "digikam_export.h"

class KXmlGuiWindow;
class KActionMenu;

namespace Digikam
{

class Theme;

class DIGIKAM_EXPORT ThemeManager : public QObject
{
    Q_OBJECT

public:

    ~ThemeManager();
    static ThemeManager* instance();

    QString currentThemeName() const;
    void    setCurrentTheme(const QString& name);

    QString defaultThemeName() const;

    void    setThemeMenuAction(KActionMenu* const action);
    void    registerThemeActions(KXmlGuiWindow* const kwin);

Q_SIGNALS:

    void signalThemeChanged();

private Q_SLOTS:

    void slotChangePalette();
    void slotConfigColors();
    void slotSettingsChanged();

private:

    ThemeManager();

    void    populateThemeMenu();
    QPixmap createSchemePreviewIcon(const KSharedConfigPtr& config) const;
    QString currentKDEdefaultTheme() const;
    void    updateCurrentKDEdefaultThemePreview();

private:

    friend class ThemeManagerCreator;

    class Private;
    Private* const d;
};

}  // namespace Digikam

#endif /* THEMEMANAGER_H */
