project(graceful-wm C)

include(${CMAKE_SOURCE_DIR}/app/common/common.cmake)

add_executable(graceful-wm
        graceful-error.h
        graceful-wm-app.c graceful-wm-app.h
        drw.c dwm.c util.c ${COMMON_SRC})
target_include_directories(graceful-wm PRIVATE
        ${XCB_INCLUDE_DIRS}
        ${XFT_INCLUDE_DIRS}
        ${X11_INCLUDE_DIRS}
        ${YAJL_INCLUDE_DIRS}
        ${GLIB_INCLUDE_DIRS}
        ${X_EXT_INCLUDE_DIRS}
        ${PANGO_INCLUDE_DIRS}
        ${IMLIB_INCLUDE_DIRS}
        ${XINERAMA_INCLUDE_DIRS}
        ${X_RENDER_INCLUDE_DIRS}
        ${FONT_CONFIG_INCLUDE_DIRS}
)
target_link_libraries(graceful-wm -lXfixes
        ${XFT_LIBRARIES}
        ${XCB_LIBRARIES}
        ${X11_LIBRARIES}
        ${YAJL_LIBRARIES}
        ${GLIB_LIBRARIES}
        ${X_EXT_LIBRARIES}
        ${IMLIB_LIBRARIES}
        ${PANGO_LIBRARIES}
        ${X_RENDER_LIBRARIES}
        ${XINERAMA_LIBRARIES}
        ${FONT_CONFIG_LIBRARIES}
)
target_compile_definitions(graceful-wm PRIVATE
        -DXINERAMA
        -DAPP_NAME=\"graceful-wm\"
)


#add_executable(graceful-wm-msg patch/ipc/dwm-msg.c)
#target_include_directories(graceful-wm-msg PRIVATE
#        ${YAJL_INCLUDE_DIRS}
#)
#target_link_libraries(graceful-wm-msg
#        ${YAJL_LIBRARIES}
#)

install(TARGETS graceful-wm         DESTINATION /usr/bin)
#install(TARGETS graceful-wm-msg     DESTINATION /usr/bin)
