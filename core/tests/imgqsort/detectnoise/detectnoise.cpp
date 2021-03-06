/* ============================================================
 * 
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 28-07-2013
 * Description : Detects noise test program
 *
 * Copyright (C) 2013-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2013-2014 by Gowtham Ashok <gwty93 at gmail dot com>
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

// Pragma directives to reduce warnings from OpenCV header files.
#if not defined(__APPLE__) && defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#endif

#if defined(__APPLE__) && defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnon-virtual-dtor"
#pragma clang diagnostic ignored "-Woverloaded-virtual"
#pragma clang diagnostic ignored "-Wcast-align"
#endif

// OpenCV includes

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

// Restore warnings
#if not defined(__APPLE__) && defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#if defined(__APPLE__) && defined(__clang__)
#pragma clang diagnostic pop
#endif

// C++ includes

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace cv;
using namespace std;

// Global variable
Mat src, src_gray;
Mat dst, detected_edges;

int     edgeThresh    = 1;
double  lowThreshold  = 0.035;   // given in research paper
int     ratio_value   = 3;
int     kernel_size   = 3;
char    window_name[] = "Edge Map";

void CannyThreshold(int, void*)
{
    // Reduce noise with a kernel 3x3
    blur( src_gray, detected_edges, Size(3,3) );

    // Canny detector
    Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio_value, kernel_size );

    // Using Canny's output as a mask, we display our result
    dst = Scalar::all(0);

    src.copyTo( dst, detected_edges);
    imshow( window_name, dst );
}

/** @function main */
int main( int /*argc*/, char** argv )
{
    // Load an image
    src = imread( argv[1] );

    if ( !src.data )
    {
        return -1;
    }

    // Create a matrix of the same type and size as src (for dst)
    dst.create( src.size(), src.type() );

    // Convert the image to grayscale
    cvtColor( src, src_gray, CV_BGR2GRAY );

    // Apply Canny Edge Detector to get the edges
    CannyThreshold(0, 0);

    double maxval;
    double average    = mean(detected_edges)[0];
    int* const maxIdx = (int* )malloc(sizeof(detected_edges));

    // To find the maximum edge intensity value

    minMaxIdx(detected_edges, 0, &maxval, 0, maxIdx);

    double blurresult = average/maxval;

    cout << "The average of the edge intensity is " << average    << std::endl;
    cout << "The maximum of the edge intensity is " << maxval     << std::endl;
    cout << "The result of the edge intensity is "  << blurresult << std::endl;

    // Wait until user exit program by pressing a key
    waitKey(0);

    return 0;
}
