project(graceful-settings C)

include(${CMAKE_SOURCE_DIR}/app/common/common.cmake)
add_executable(graceful-settings main.c settings.c ${COMMON_SRC})

target_link_libraries(graceful-settings -lX11 -lXfixes
        ${GTK_LIBRARIES}
        ${GDK_X11_LIBRARIES}
)

target_include_directories(graceful-settings PRIVATE
        ${GTK_INCLUDE_DIRS}
        ${GDK_X11_INCLUDE_DIRS}
)

target_compile_definitions(graceful-settings PRIVATE
        -DAPP_NAME=\"graceful-wm\"
)

install(TARGETS graceful-settings   DESTINATION /usr/bin)
