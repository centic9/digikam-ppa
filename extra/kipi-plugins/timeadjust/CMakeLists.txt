SUBDIRS(icons)

SET(kipiplugin_timeadjust_PART_SRCS plugin_timeadjust.cpp
                                    timeadjustdialog.cpp
                                    clockphotodialog.cpp
                                    actionthread.cpp
                                    task.cpp
                                    myimagelist.cpp
                                    settingswidget.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_timeadjust ${kipiplugin_timeadjust_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_timeadjust
                      ${KDE4_KPARTS_LIBS}
                      ${KDE4_THREADWEAVER_LIBRARIES}
                      ${QT_AND_KDECORE_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      kipiplugins
                     )

CONFIGURE_FILE(kipiplugin_timeadjust.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_timeadjust.desktop)

INSTALL(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_timeadjust.desktop DESTINATION ${SERVICES_INSTALL_DIR})
INSTALL(TARGETS kipiplugin_timeadjust                                     DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES   kipiplugin_timeadjustui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)