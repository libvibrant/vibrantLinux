cmake_minimum_required(VERSION 3.16)

project(vibrantLinux LANGUAGES CXX VERSION 2.2.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

find_package(XCB REQUIRED COMPONENTS EWMH)
find_package(PkgConfig REQUIRED)
pkg_check_modules(vibrant REQUIRED vibrant)

qt_standard_project_setup()

qt_add_executable(vibrantLinux
    ${vibrantLinux_RESOURCES}
    src/autostart.cpp
    src/autostart.h
    src/displaymanager.cpp
    src/displaymanager.h
    src/displaytab.cpp
    src/displaytab.h
    src/entryeditor.cpp
    src/entryeditor.h
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/procscanner.cpp
    src/procscanner.h
    src/programinfo.h
)

qt_add_resources(vibrantLinux assets
    PREFIX "/"
    BASE assets
    FILES assets/io.github.libvibrant.vibrantLinux.png assets/io.github.libvibrant.vibrantLinux.desktop
)

target_compile_definitions(vibrantLinux PUBLIC
    VIBRANT_LINUX_VERSION="${CMAKE_PROJECT_VERSION}"
)

target_link_libraries(vibrantLinux PRIVATE Qt6::Core Qt6::Widgets ${XCB_LIBRARIES} ${vibrant_LIBRARIES})

install(TARGETS vibrantLinux DESTINATION ${CMAKE_INSTALL_BINDIR})

install(FILES assets/io.github.libvibrant.vibrantLinux.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)

install(FILES assets/io.github.libvibrant.vibrantLinux.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/512x512/apps)
install(FILES assets/io.github.libvibrant.vibrantLinux.appdata.xml DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)
