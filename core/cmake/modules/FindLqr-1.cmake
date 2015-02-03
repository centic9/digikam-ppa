#  Try to find the lqr-1 library
#  Once done this will define
#
#  LQR-1_FOUND        - system has the lqr-1 library
#  LQR-1_INCLUDE_DIRS - the lqr-1 library include directory
#  LQR-1_LIBRARIES    - Link these to use the lqr-1 library
#
#  Copyright (c) 2009 Pino Toscano <pino@kde.org>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

IF (LQR-1_LIBRARIES AND LQR-1_INCLUDE_DIRS)
    # in cache already
    SET(LQR-1_FOUND TRUE)
ELSE (LQR-1_LIBRARIES AND LQR-1_INCLUDE_DIRS)
    # use pkg-config to get the directories and then use these values
    # in the FIND_PATH() and FIND_LIBRARY() calls
    IF (NOT WIN32)
        INCLUDE(UsePkgConfig)
        PKGCONFIG(lqr-1 _lqrIncDir _lqrLinkDir _lqrLinkFlags _lqrCflags)
    ENDIF (NOT WIN32)

    FIND_PATH(LQR-1_INCLUDE_DIR
                NAMES         lqr.h
                PATH_SUFFIXES lqr-1
                PATHS         ${_lqrIncDir}
             )

    FIND_LIBRARY(LQR-1_LIBRARY
                 NAMES lqr-1
                 PATHS ${_lqrLinkDir}
                )

    IF (LQR-1_LIBRARY AND LQR-1_INCLUDE_DIR)
        FIND_PACKAGE(GLIB2)

        IF (GLIB2_FOUND)
            INCLUDE(CheckCXXSourceCompiles)
            SET(CMAKE_REQUIRED_INCLUDES "${LQR-1_INCLUDE_DIR}" "${GLIB2_INCLUDE_DIR}")

            CHECK_CXX_SOURCE_COMPILES("
    #include <lqr.h>

    int main()
    {
    LqrImageType t = LQR_RGB_IMAGE;
    return 0;
    }
    " HAVE_LQR_0_4)

            IF (HAVE_LQR_0_4)
                INCLUDE_DIRECTORIES(${GLIB2_INCLUDE_DIR})
                SET(LQR-1_FOUND TRUE)
            ENDIF (HAVE_LQR_0_4)

        ENDIF (GLIB2_FOUND)

    ENDIF (LQR-1_LIBRARY AND LQR-1_INCLUDE_DIR)

    IF (LQR-1_FOUND)
        SET(LQR-1_LIBRARIES
            ${LQR-1_LIBRARIES}
            ${LQR-1_LIBRARY}
           )
        SET(LQR-1_INCLUDE_DIRS
            ${LQR-1_INCLUDE_DIR}
           )
    ENDIF (LQR-1_FOUND)

    FIND_PACKAGE_HANDLE_STANDARD_ARGS(Lqr-1 DEFAULT_MSG LQR-1_INCLUDE_DIRS LQR-1_LIBRARIES)

    # show the LQR-1_INCLUDE_DIRS and LQR-1_LIBRARIES variables only in the advanced view
    MARK_AS_ADVANCED(LQR-1_INCLUDE_DIRS LQR-1_LIBRARIES)

ENDIF (LQR-1_LIBRARIES AND LQR-1_INCLUDE_DIRS)

