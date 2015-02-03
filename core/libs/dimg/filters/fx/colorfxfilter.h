/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-05-25
 * Description : Color FX threaded image filter.
 *
 * Copyright 2005-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright 2006-2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright 2010      by Martin Klapetek <martin dot klapetek at gmail dot com>
 *
 * Original Blur algorithms copyrighted 2004 by
 * Pieter Z. Voloshyn <pieter dot voloshyn at gmail dot com>.
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

#ifndef COLORFXFILTER_H
#define COLORFXFILTER_H

// Local includes

#include "digikam_export.h"
#include "dimgthreadedfilter.h"
#include "globals.h"

namespace Digikam
{

class RandomNumberGenerator;

class DIGIKAM_EXPORT ColorFXContainer
{

public:

    ColorFXContainer()
    {
        colorFXType = 0; // ColorFXFilter::Solarize
        level       = 0;
        iterations  = 2;
    };

    ~ColorFXContainer()
    {
    };

public:

    int colorFXType;
    int level;
    int iterations;
};

// ----------------------------------------------------------------------------------------------

class DIGIKAM_EXPORT ColorFXFilter : public DImgThreadedFilter
{

public:

    enum ColorFXFilterTypes
    {
        Solarize = 0,
        Vivid,
        Neon,
        FindEdges
    };

public:

    explicit ColorFXFilter(QObject* const parent = 0);
    explicit ColorFXFilter(DImg* const orgImage, QObject* const parent, const ColorFXContainer& settings=ColorFXContainer());

    ~ColorFXFilter()
    {
    };

    static QString          FilterIdentifier()
    {
        return "digikam:ColorFXFilter";
    }

    static QString          DisplayableName()
    {
        return I18N_NOOP("Color FX Filter");
    }

    static QList<int>       SupportedVersions()
    {
        return QList<int>() << 1;
    }

    static int              CurrentVersion()
    {
        return 1;
    }

    virtual QString         filterIdentifier() const
    {
        return FilterIdentifier();
    }

    virtual FilterAction    filterAction();

    void                    readParameters(const FilterAction& action);

private:

    void filterImage();

    void solarize(DImg* const orgImage, DImg* const destImage, int factor);
    void vivid(DImg* const orgImage, DImg* const destImage, int factor);
    void neon(DImg* const orgImage, DImg* const destImage, int Intensity, int BW);
    void findEdges(DImg* const orgImage, DImg* const destImage, int Intensity, int BW);
    void neonFindEdges(DImg* const orgImage, DImg* const destImage, bool neon, int Intensity, int BW);

private:

    ColorFXContainer m_settings;
};

}  // namespace Digikam

#endif /* COLORFXFILTER_H */
