/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2010-06-16
 * @brief  The Database class wraps the libface database
 *
 * @author Copyright (C) 2010 by Aditya Bhatt
 *         <a href="mailto:adityabhatt1991 at gmail dot com">adityabhatt1991 at gmail dot com</a>
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
#include <QTime>

// OpenCV includes

#include "libopencv.h"

// KDE includes

#include <kdebug.h>

// libkface includes

#include "libkface/alignment-congealing/funnelreal.h"

// it's not exported...
#include "libkface/alignment-congealing/funnelreal.cpp"

using namespace KFaceIface;

QStringList toPaths(char** argv, int startIndex, int argc)
{
    QStringList files;

    for (int i=startIndex; i<argc; i++)
    {
        files << QString::fromLocal8Bit(argv[i]);
    }

    return files;
}

QList<cv::Mat> toImages(const QStringList& paths)
{
    QList<cv::Mat> images;

    foreach (const QString& path, paths)
    {
        QByteArray s = path.toLocal8Bit();
        images << cv::imread(std::string(s));
    }

    return images;
}

class OpenCVSideBySideDisplay
{
public:
    OpenCVSideBySideDisplay(int rows, int uiSize = 200)
        : uiSize(uiSize), currentRow(0)
    {
        bigImage = cv::Mat::zeros(uiSize*rows, 2*uiSize, CV_8UC3);
    }

    void add(const cv::Mat& left, const cv::Mat& right)
    {
        // Draw images side-by-side for later display
        QSize size(left.cols, left.rows);
        size.scale(uiSize, uiSize, Qt::KeepAspectRatio);
        cv::Size scaleSize(size.height(), size.width());

        const int top = currentRow*uiSize;
        cv::Mat scaledLeft, scaledRight;
        cv::resize(left, scaledLeft, scaleSize);
        cv::resize(right, scaledRight, scaleSize);
        if (scaledLeft.channels() == 1)
        {
            cv::cvtColor(scaledLeft, scaledLeft, CV_GRAY2BGR);
        }
        if (scaledRight.channels() == 1)
        {
            cv::cvtColor(scaledRight, scaledRight, CV_GRAY2BGR);
        }

        scaledLeft.copyTo(bigImage.colRange(0, scaledLeft.cols).rowRange(top, top+scaledLeft.rows));
        scaledRight.copyTo(bigImage.colRange(uiSize, uiSize+scaledRight.cols).rowRange(top, top+scaledRight.rows));

        currentRow++;
    }

    void show(const char* title = "images")
    {
        cv::namedWindow(title);
        cv::imshow(title, bigImage);
    }

    cv::Mat bigImage;
    const int uiSize;
    int currentRow;
};

// --------------------------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        kDebug() << "Bad Args!!!\nUsage: " << argv[0] << " align <image1> <image2> ... ";
        return 0;
    }

    QApplication app(argc, argv);

    QStringList paths     = toPaths(argv, 1, argc);
    QList<cv::Mat> images = toImages(paths);

    QTime time;
    time.start();
    FunnelReal funnel;
    kDebug() << "Setup of FunnelReal took" << time.restart();

    OpenCVSideBySideDisplay display(images.size());
    foreach (const cv::Mat& image, images)
    {
        cv::Mat aligned = funnel.align(image);
        display.add(image, aligned);
    }

    int elapsed = time.elapsed();
    kDebug() << "Alignment took" << elapsed << "for" << images.size() << "," << ((float)elapsed/images.size()) << "per image";

    display.show();
    app.exec();

    return 0;
}
