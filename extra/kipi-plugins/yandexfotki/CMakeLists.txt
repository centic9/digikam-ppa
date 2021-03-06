#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

include_directories(${QCA2_INCLUDE_DIR})

add_definitions(${QCA2_DEFINITIONS})

set(kipiplugin_yandexfotki_PART_SRCS
    plugin_yandexfotki.cpp
    logindialog.cpp
    yfalbum.cpp
    yfphoto.cpp
    yftalker.cpp
    yfwindow.cpp
    yfalbumdialog.cpp
    yandexrsa.cpp
    yandexauth.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_yandexfotki ${kipiplugin_yandexfotki_PART_SRCS})

target_link_libraries(kipiplugin_yandexfotki
                      ${KDCRAW_LIBRARIES}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${QCA2_LIBRARIES}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBS}
                      kipiplugins
                     )

configure_file(kipiplugin_yandexfotki.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_yandexfotki.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_yandexfotki.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_yandexfotki                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kipiplugin_yandexfotkiui.rc                                  DESTINATION ${DATA_INSTALL_DIR}/kipi)
