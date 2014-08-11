ADD_SUBDIRECTORY(icons)
ADD_SUBDIRECTORY(data)

IF(HUPNP_FOUND AND HUPNPAV_FOUND)

    INCLUDE_DIRECTORIES(${HUPNP_INCLUDE_DIR})

ELSE(HUPNP_FOUND AND HUPNPAV_FOUND)

    ADD_DEFINITIONS(-DH_BUILD_UPNP_CORE_LIB)
    ADD_DEFINITIONS(-DH_BUILD_UPNP_LIGHTING_LIB)
    ADD_DEFINITIONS(-DH_BUILD_UPNP_AV_LIB)

    ADD_SUBDIRECTORY(extra)

    INCLUDE_DIRECTORIES(extra/hupnp/include)
    INCLUDE_DIRECTORIES(extra/hupnp_av/include)

ENDIF(HUPNP_FOUND AND HUPNPAV_FOUND)

SET(kipiplugin_dlnaexport_PART_SRCS
    finalpage.cpp
    plugin_dlnaexport.cpp
    hupnpmediaserver.cpp
    wizard.cpp
    welcomepage.cpp
    minidlnamediaserver.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_dlnaexport ${kipiplugin_dlnaexport_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_dlnaexport
                      ${KDCRAW_LIBRARIES}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBS}
                      ${QT_LIBRARIES}
                      ${QT_QTXMLPATTERNS_LIBRARY}
                      kipiplugins
                     )

IF(HUPNP_FOUND AND HUPNPAV_FOUND)
    TARGET_LINK_LIBRARIES(kipiplugin_dlnaexport
                          ${HUPNP_LIBS}
                          ${HUPNPAV_LIBS}
                         )

ELSE(HUPNP_FOUND AND HUPNPAV_FOUND)

    TARGET_LINK_LIBRARIES(kipiplugin_dlnaexport
                          HUpnp
                          HUpnpAv
                         )

ENDIF(HUPNP_FOUND AND HUPNPAV_FOUND)

CONFIGURE_FILE(kipiplugin_dlnaexport.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_dlnaexport.desktop)

INSTALL(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_dlnaexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})
INSTALL(TARGETS kipiplugin_dlnaexport                                     DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES   kipiplugin_dlnaexportui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)