project(graceful-wm C)


add_executable(graceful-wm drw.c dwm.c util.c)
target_include_directories(graceful-wm PRIVATE
        ${XFT_INCLUDE_DIRS}
        ${X11_INCLUDE_DIRS}
        ${YAJL_INCLUDE_DIRS}
        ${XINERAMA_INCLUDE_DIRS}
        ${FONT_CONFIG_INCLUDE_DIRS}
)
target_link_libraries(graceful-wm
        ${XFT_LIBRARIES}
        ${X11_LIBRARIES}
        ${YAJL_LIBRARIES}
        ${XINERAMA_LIBRARIES}
        ${FONT_CONFIG_LIBRARIES}
)


add_executable(graceful-wm-msg dwm-msg.c)
target_include_directories(graceful-wm-msg PRIVATE
        ${YAJL_INCLUDE_DIRS}
)
target_link_libraries(graceful-wm-msg
        ${YAJL_LIBRARIES}
)
