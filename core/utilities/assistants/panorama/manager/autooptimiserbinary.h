/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : Autodetect autooptimiser binary program and version
 *
 * Copyright (C) 2011-2016 by Benjamin Girault <benjamin dot girault at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef AUTOOPTIMISERBINARY_H
#define AUTOOPTIMISERBINARY_H

// Local includes

#include "dbinaryiface.h"

namespace Digikam
{

class AutoOptimiserBinary : public DBinaryIface
{

public:

    AutoOptimiserBinary()
        : DBinaryIface(QLatin1String("autooptimiser"),
                       QLatin1String("2010.4"),
                       QLatin1String("autooptimiser version "),
                       1,
                       QLatin1String("Hugin"),
                       QLatin1String("http://hugin.sourceforge.net/download/"),
                       QLatin1String("Panorama")
                      )
        {
            setup();
        }

    ~AutoOptimiserBinary()
    {
    }
};

} // namespace Digikam

#endif  // AUTOOPTIMISERBINARY_H
