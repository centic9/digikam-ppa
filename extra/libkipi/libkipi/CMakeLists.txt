# ===========================================================
#
# This file is a part of digiKam project
# <a href="http://www.digikam.org">http://www.digikam.org</a>
#
# @date   2004-02-01
# @brief  KDE Image Plugin Interface
#
# @author Copyright (C) 2004-2014 by Gilles Caulier
#         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
#
# This program is free software; you can redistribute it
# and/or modify it under the terms of the GNU General
# Public License as published by the Free Software Foundation;
# either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# ============================================================

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/version.h.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/version.h)

SET(kipi_LIB_SRCS pluginloader.cpp
                  interface.cpp
                  imagecollection.cpp
                  imagecollectionshared.cpp
                  imageinfoshared.cpp
                  plugin.cpp
                  imageinfo.cpp
                  uploadwidget.cpp
                  configwidget.cpp
                  configwidget_p.cpp
                  imagecollectionselector.cpp
   )

KDE4_ADD_LIBRARY(kipi SHARED ${kipi_LIB_SRCS})

TARGET_LINK_LIBRARIES(kipi
                      ${KDE4_KPARTS_LIBS}
                      ${KDE4_KFILE_LIBS}
                      ${QT_QTGUI_LIBRARY}
                     )

SET_TARGET_PROPERTIES(kipi PROPERTIES VERSION ${KIPI_LIB_SO_VERSION_STRING} SOVERSION ${KIPI_LIB_SO_CUR_VERSION})

INSTALL(TARGETS kipi ${INSTALL_TARGETS_DEFAULT_ARGS} )

# desktop file which define plugin service model, used by tool from kipi-plugins.
INSTALL(FILES kipiplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})

# Check if compilation is done through digiKam SC or not. If yes, do not install header files.
IF(NOT DIGIKAMSC_USE_PRIVATE_KDEGRAPHICS)

    INSTALL(FILES plugin.h
                  pluginloader.h
                  interface.h
                  imageinfo.h
                  imagecollection.h
                  imageinfoshared.h
                  imagecollectionshared.h
                  imagecollectionselector.h
                  uploadwidget.h
                  configwidget.h
                  libkipi_export.h
                  ${CMAKE_CURRENT_BINARY_DIR}/version.h
            DESTINATION ${INCLUDE_INSTALL_DIR}/libkipi
            COMPONENT Devel
           )
ENDIF()