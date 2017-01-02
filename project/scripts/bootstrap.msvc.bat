@ECHO OFF

REM  Copyright (c) 2008-2016, Gilles Caulier, <caulier dot gilles at gmail dot com>
REM
REM  Redistribution and use is allowed according to the terms of the BSD license.
REM  For details see the accompanying COPYING-CMAKE-SCRIPTS file.

REM Adjust there the right path to KF5 install directory.
SET KF5_INSTALL_DIR=C:\KF5
SET TARGET_INSTALL_DIR=%KF5_INSTALL_DIR%

SET PATH=%PATH%;%KF5_INSTALL_DIR%\bin;%KF5_INSTALL_DIR%\plugins
SET KDEDIRS=%KF5_INSTALL_DIR%
SET KDE4_LIB_DIRS=%KF5_INSTALL_DIR%
SET KDE4_LIB_DIR=%KF5_INSTALL_DIR%
SET KDEROOT=%KF5_INSTALL_DIR%
SET QTMAKESPEC=%KF5_INSTALL_DIR%\mkspecs\win32-g++
SET QT_PLUGIN_PATH=%KF5_INSTALL_DIR%\plugins
SET QTDIR=%KF5_INSTALL_DIR%
SET QT_INSTALL_DIR=%KF5_INSTALL_DIR%

IF NOT EXIST "build" md "build"

cd "build"

REM Microsoft Visual C++ command line compiler.
cmake -G "NMake Makefiles" . ^
      -DCMAKE_BUILD_TYPE=relwithdebinfo ^
      -DCMAKE_INSTALL_PREFIX=%KF5_INSTALL_DIR%\ ^
      -DBUILD_TESTING=ON ^
      -DDIGIKAMSC_CHECKOUT_PO=OFF ^
      -DDIGIKAMSC_CHECKOUT_DOC=OFF ^
      -DDIGIKAMSC_COMPILE_PO=OFF ^
      -DDIGIKAMSC_COMPILE_DOC=ON ^
      -DDIGIKAMSC_COMPILE_DIGIKAM=ON ^
      -DDIGIKAMSC_COMPILE_KIPIPLUGINS=ON ^
      -DDIGIKAMSC_COMPILE_LIBKIPI=ON ^
      -DDIGIKAMSC_COMPILE_LIBKSANE=ON ^
      -DDIGIKAMSC_COMPILE_LIBMEDIAWIKI=ON ^
      -DDIGIKAMSC_COMPILE_LIBKVKONTAKTE=ON ^
      -DENABLE_OPENCV3=OFF ^
      -DENABLE_KFILEMETADATASUPPORT=OFF ^
      -DENABLE_AKONADICONTACTSUPPORT=OFF ^
      -DENABLE_MYSQLSUPPORT=OFF ^
      -DENABLE_INTERNALMYSQL=OFF ^
      -DENABLE_MEDIAPLAYER=OFF ^
      -DEXPAT_LIBRARY=%KF5_INSTALL_DIR%/lib/expat.lib ^
      -DCMAKE_INCLUDE_PATH=%KF5_INSTALL_DIR%/include ^
      -DCMAKE_LIBRARY_PATH=%KF5_INSTALL_DIR%/lib ^
      ..
