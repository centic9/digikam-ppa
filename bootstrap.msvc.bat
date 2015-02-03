@ECHO OFF

REM  Copyright (c) 2008-2012, Gilles Caulier, <caulier dot gilles at gmail dot com>
REM 
REM  Redistribution and use is allowed according to the terms of the BSD license.
REM  For details see the accompanying COPYING-CMAKE-SCRIPTS file.

REM Adjust there the right path to KDE4 install directory.
SET KDE4_INSTALL_DIR=C:\KDE
SET TARGET_INSTALL_DIR=%KDE4_INSTALL_DIR%

SET PATH=%PATH%;%KDE4_INSTALL_DIR%\bin;%KDE4_INSTALL_DIR%\plugins
SET KDEDIRS=%KDE4_INSTALL_DIR%
SET KDE4_LIB_DIRS=%KDE4_INSTALL_DIR%
SET KDE4_LIB_DIR=%KDE4_INSTALL_DIR%
SET KDEROOT=%KDE4_INSTALL_DIR%
SET QTMAKESPEC=%KDE4_INSTALL_DIR%\mkspecs\win32-g++
SET QT_PLUGIN_PATH=%KDE4_INSTALL_DIR%\plugins
SET QTDIR=%KDE4_INSTALL_DIR%
SET QT_INSTALL_DIR=%KDE4_INSTALL_DIR%

IF NOT EXIST "build" md "build"

cd "build"

REM Microsoft Visual C++ command line compiler.
cmake -G "NMake Makefiles" . ^
      -DCMAKE_BUILD_TYPE=relwithdebinfo ^
      -DCMAKE_INSTALL_PREFIX=%KDE4_INSTALL_DIR% ^
      -DKDE4_BUILD_TESTS=on ^
      -DDIGIKAMSC_USE_PRIVATE_KDEGRAPHICS=on ^
      -DDIGIKAMSC_COMPILE_DOC=off ^
      -DENABLE_LCMS2=off ^
      -DENABLE_RAWSPEED=on ^
      -Wno-dev ^
      -DEXPAT_LIBRARY=%KDE4_INSTALL_DIR%/lib/expat.lib ^
      -DCMAKE_INCLUDE_PATH=%KDE4_INSTALL_DIR%/include ^
      -DCMAKE_LIBRARY_PATH=%KDE4_INSTALL_DIR%/lib ^
      ..
