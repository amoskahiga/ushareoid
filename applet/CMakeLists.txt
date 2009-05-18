project(plasma-ushareoid)
 
find_package(KDE4 REQUIRED)
include(KDE4Defaults)
include(MacroOptionalAddSubdirectory)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
   ${CMAKE_SOURCE_DIR}
   ${CMAKE_BINARY_DIR}
   ${KDE4_INCLUDES}
   )

add_subdirectory(icons)

set(ushareoid_SRCS ushareoid.cpp ushareoid.h configdialog.cpp configdialog.h)

kde4_add_ui_files(ushareoid_SRCS configdialog.ui)

kde4_add_plugin(plasma_applet_ushareoid ${ushareoid_SRCS})

target_link_libraries(plasma_applet_ushareoid ${KDE4_PLASMA_LIBS} 
					      ${KDE4_KDEUI_LIBS} 
					      ${KDE4_KIO_LIBS}
					      ${KDE4_KFILE_LIBS} 
    )

install(TARGETS plasma_applet_ushareoid
        DESTINATION ${PLUGIN_INSTALL_DIR})
 
install(FILES plasma-applet-ushareoid.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})
