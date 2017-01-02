/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2009-08-03
 * @brief  Tools for combining rotation operations
 *
 * @author Copyright (C) 2006-2016 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 * @author Copyright (C) 2004-2012 by Marcel Wiesweg
 *         <a href="mailto:marcel dot wiesweg at gmx dot de">marcel dot wiesweg at gmx dot de</a>
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

#ifndef META_ENGINE_ROTATION_H
#define META_ENGINE_ROTATION_H

// Qt includes

#include <QMatrix>

// Local includes

#include "digikam_export.h"
#include "metaengine.h"

namespace Digikam
{

class DIGIKAM_EXPORT MetaEngineRotation
{

public:

    /** This describes single transform primitives.
     *  Note some of the defined Exif rotation flags combine
     *  two of these actions.
     *  The enum values correspond to those defined
     *  as JXFORM_CODE in the often used the JPEG tool transupp.h.
     */
    enum TransformationAction
    {
        NoTransformation = 0, /// no transformation
        FlipHorizontal   = 1, /// horizontal flip
        FlipVertical     = 2, /// vertical flip
        Rotate90         = 5, /// 90-degree clockwise rotation
        Rotate180        = 6, /// 180-degree rotation
        Rotate270        = 7  /// 270-degree clockwise (or 90 ccw)
    };

public:

    /// Constructs the identity matrix (the matrix describing no transformation)
    MetaEngineRotation();
    /// Returns the matrix corresponding to the given TransformationAction
    MetaEngineRotation(TransformationAction action);
    /// Returns the matrix corresponding to the given TransformationAction
    MetaEngineRotation(MetaEngine::ImageOrientation exifOrientation);

    bool operator==(const MetaEngineRotation& ma) const;
    bool operator!=(const MetaEngineRotation& ma) const;

    /// Returns true of this matrix describes no transformation (is the identity matrix)
    bool isNoTransform() const;

    MetaEngineRotation& operator*=(const MetaEngineRotation& ma);

    /// Applies the given transform to this matrix
    MetaEngineRotation& operator*=(TransformationAction action);

    /// Applies the given transform actions to this matrix
    MetaEngineRotation& operator*=(QList<TransformationAction> actions);

    /// Applies the given Exif orientation flag to this matrix
    MetaEngineRotation& operator*=(MetaEngine::ImageOrientation exifOrientation);

    /** Returns the actions described by this matrix. The order matters.
     *  Not all possible matrices are supported, but all those that can be combined
     *  by Exif rotation flags and the transform actions above.
     *  If isNoTransform() or the matrix is not supported returns an empty list. */
    QList<TransformationAction> transformations() const;

    /** Returns the Exif orienation flag describing this matrix.
     *  Returns ORIENTATION_UNSPECIFIED if no flag matches this matrix.
     */
    MetaEngine::ImageOrientation exifOrientation() const;

    /// Returns a QMatrix representing this matrix
    QMatrix toMatrix() const;

    /// Returns a QMatrix for the given Exif orientation
    static QMatrix toMatrix(MetaEngine::ImageOrientation orientation);

    MetaEngineRotation(int m11, int m12, int m21, int m22);

protected:

    void set(int m11, int m12, int m21, int m22);

protected:

    int m[2][2];
};

}  // namespace Digikam

#endif  // META_ENGINE_ROTATION_H
