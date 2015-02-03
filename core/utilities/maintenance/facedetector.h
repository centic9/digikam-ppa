/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2010-07-18
 * Description : batch face detection
 *
 * Copyright (C) 2010      by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * Copyright (C) 2010-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef BATCHFACEDETECTOR_H
#define BATCHFACEDETECTOR_H

// Local includes

#include "maintenancetool.h"

namespace Digikam
{

class DImg;
class FacePipelinePackage;
class FaceScanSettings;
class ImageInfo;
class ImageInfoList;

class FaceDetector : public MaintenanceTool
{
    Q_OBJECT

public:

    explicit FaceDetector(const FaceScanSettings& settings, ProgressItem* const parent = 0);
    ~FaceDetector();

private Q_SLOTS:

    void slotStart();
    void slotContinueAlbumListing();
    void slotItemsInfo(const ImageInfoList&);
    void slotImagesSkipped(const QList<ImageInfo>&);
    void slotShowOneDetected(const FacePipelinePackage& package);
    void slotDone();
    void slotCancel();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // BATCHFACEDETECTOR_H
