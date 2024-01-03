project(graceful-compton C)

include(${CMAKE_SOURCE_DIR}/app/common/common.cmake)

add_executable(graceful-compton compton.c c2.c dbus.c opengl.c xrescheck.c ${COMMON_SRC})
target_include_directories(graceful-compton PRIVATE
        ${GL_INCLUDE_DIRS}
        ${XFT_INCLUDE_DIRS}
        ${X11_INCLUDE_DIRS}
        ${DRM_INCLUDE_DIRS}
        ${GLIB_INCLUDE_DIRS}
        ${DBUS_INCLUDE_DIRS}
        ${PCRE_INCLUDE_DIRS}
        ${X_EXT_INCLUDE_DIRS}
        ${CONFIG_INCLUDE_DIRS}
        ${X_FIXES_INCLUDE_DIRS}
        ${X_RANDR_INCLUDE_DIRS}
        ${XINERAMA_INCLUDE_DIRS}
        ${X_DAMAGE_INCLUDE_DIRS}
        ${X_RENDER_INCLUDE_DIRS}
        ${X_COMPOSITE_INCLUDE_DIRS}
)
target_link_libraries(graceful-compton -lm -lrt
        ${GL_LIBRARIES}
        ${XFT_LIBRARIES}
        ${X11_LIBRARIES}
        ${DRM_LIBRARIES}
        ${GLIB_LIBRARIES}
        ${DBUS_LIBRARIES}
        ${PCRE_LIBRARIES}
        ${X_EXT_LIBRARIES}
        ${CONFIG_LIBRARIES}
        ${X_FIXES_LIBRARIES}
        ${X_RANDR_LIBRARIES}
        ${XINERAMA_LIBRARIES}
        ${X_DAMAGE_LIBRARIES}
        ${X_RENDER_LIBRARIES}
        ${X_COMPOSITE_LIBRARIES}
)
target_compile_definitions(graceful-compton PRIVATE
        -DCONFIG_C2
        -DCONFIG_XRC
        -DCONFIG_DBUS
        -DCONFIG_XSYNC
        -DCONFIG_GLX_SYNC
        -DCONFIG_XINERAMA
        -DCONFIG_VSYNC_DRM
        -DCONFIG_LIBCONFIG
        -DCONFIG_REGEX_PCRE
        -DCONFIG_VSYNC_OPENGL
        -DCONFIG_REGEX_PCRE_JIT
        -DCONFIG_VSYNC_OPENGL_FBO
        -DCONFIG_VSYNC_OPENGL_VBO
        -DCONFIG_VSYNC_OPENGL_GLSL
        -DAPP_NAME=\"graceful-compton\"
)


install(TARGETS graceful-compton    DESTINATION /usr/bin)
