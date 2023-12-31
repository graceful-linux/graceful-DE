cmake_minimum_required(VERSION 3.27)
project(graceful-DE C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_INCLUDE_CURRENT_DIR OFF)

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 3)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION_TWEAK 0)
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}-${PROJECT_VERSION_TWEAK}")

add_definitions(-w -g -D VERSION=\\"${PROJECT_VERSION}\\")

find_package(PkgConfig)

pkg_check_modules(GL            REQUIRED gl)
pkg_check_modules(XFT           REQUIRED xft)
pkg_check_modules(X11           REQUIRED x11)
pkg_check_modules(YAJL          REQUIRED yajl)
pkg_check_modules(X_EXT         REQUIRED xext)
pkg_check_modules(DBUS          REQUIRED dbus-1)
pkg_check_modules(DRM           REQUIRED libdrm)
pkg_check_modules(X_FIXES       REQUIRED xfixes)
pkg_check_modules(X_RANDR       REQUIRED xrandr)
pkg_check_modules(IMLIB         REQUIRED imlib2)
pkg_check_modules(X_RENDER      REQUIRED xrender)
pkg_check_modules(PCRE          REQUIRED libpcre)
pkg_check_modules(X_DAMAGE      REQUIRED xdamage)
pkg_check_modules(GTK           REQUIRED gtk+-3.0)
pkg_check_modules(GLIB          REQUIRED glib-2.0)
pkg_check_modules(XINERAMA      REQUIRED xinerama)
pkg_check_modules(CONFIG        REQUIRED libconfig)
pkg_check_modules(FONT_CONFIG   REQUIRED fontconfig)
pkg_check_modules(X_COMPOSITE   REQUIRED xcomposite)
pkg_check_modules(GDK_X11       REQUIRED gdk-x11-3.0)
pkg_check_modules(PANGO         REQUIRED pango pangoxft)
pkg_check_modules(XCB           REQUIRED xcb xcb-res x11-xcb)

include(data/data.cmake)
add_subdirectory(app/wm)
add_subdirectory(app/compton)
add_subdirectory(app/settings)
