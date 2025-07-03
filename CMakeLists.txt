cmake_minimum_required(VERSION 3.16)

project(AlbionBuildRandomizer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt packages
find_package(Qt5 REQUIRED COMPONENTS Widgets)

# Set Qt plugin paths
set(QT_PLUGIN_PATH "${Qt5_DIR}/../../../plugins")
set(QT_QPA_PLATFORM_PLUGIN_PATH "${QT_PLUGIN_PATH}/platforms")

# Add source files
set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/buildconstructor.cpp
    src/buildconstructor.h
    src/build.cpp
    src/build.h
    src/buildpool.cpp
    src/buildpool.h
    src/buildcard.cpp
    src/buildcard.h
    src/addbuildialog.cpp
    src/addbuildialog.h
    src/itemslot.cpp
    src/itemslot.h
    src/organizerwidget.cpp
    src/organizerwidget.h
    src/rollbuildwidget.cpp
    src/rollbuildwidget.h
    src/starscalculatorwidget.cpp
    src/starscalculatorwidget.h
    src/leaderboard.cpp
    src/leaderboard.h
    src/leaderboardwidget.cpp
    src/leaderboardwidget.h
    src/playerstats.cpp
    src/playerstats.h
    src/playerstatsdialog.cpp
    src/playerstatsdialog.h
    src/playerstatwidget.cpp
    src/playerstatwidget.h
    src/gamesettings.cpp
    src/gamesettings.h
    src/settingsdialog.cpp
    src/settingsdialog.h
    src/roledialog.cpp
    src/roledialog.h
)

# Add resource files
qt5_add_resources(QRC_SOURCES resources.qrc)

# Create executable
add_executable(AlbionBuildRandomizer
    ${PROJECT_SOURCES}
    ${QRC_SOURCES}
)

# Link Qt libraries
target_link_libraries(AlbionBuildRandomizer PRIVATE Qt5::Widgets)

# Copy Qt plugins after build
add_custom_command(TARGET AlbionBuildRandomizer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:AlbionBuildRandomizer>/platforms"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${QT_QPA_PLATFORM_PLUGIN_PATH}/qwindows.dll"
        "$<TARGET_FILE_DIR:AlbionBuildRandomizer>/platforms/"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${Qt5_DIR}/../../../bin/Qt5Core.dll"
        "${Qt5_DIR}/../../../bin/Qt5Gui.dll"
        "${Qt5_DIR}/../../../bin/Qt5Widgets.dll"
        "$<TARGET_FILE_DIR:AlbionBuildRandomizer>/platforms/"
)

# Create qt.conf
file(WRITE ${CMAKE_BINARY_DIR}/qt.conf "
[Paths]
Plugins = .
")

# Copy qt.conf after build
add_custom_command(TARGET AlbionBuildRandomizer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_BINARY_DIR}/qt.conf"
        "$<TARGET_FILE_DIR:AlbionBuildRandomizer>/qt.conf"
)

# Install rules
include(GNUInstallDirs)
install(TARGETS AlbionBuildRandomizer
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

set_target_properties(AlbionBuildRandomizer PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
) 