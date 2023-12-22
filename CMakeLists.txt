cmake_minimum_required(VERSION 3.27)
project(graceful-DE C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_INCLUDE_CURRENT_DIR OFF)

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION_TWEAK 0)

add_definitions(-w -g -DVERSION=\"${PROJECT_VERSION}\")

find_package(PkgConfig)

pkg_check_modules(XFT           REQUIRED xft)
pkg_check_modules(X11           REQUIRED x11)
pkg_check_modules(YAJL          REQUIRED yajl)
pkg_check_modules(XINERAMA      REQUIRED xinerama)
pkg_check_modules(FONT_CONFIG   REQUIRED fontconfig)

add_subdirectory(app/wm)