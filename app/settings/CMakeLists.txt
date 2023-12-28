project(graceful-settings C)

add_executable(graceful-settings main.c settings.c)

target_link_libraries(graceful-settings -lX11 -lXfixes
        ${GTK_LIBRARIES}
        ${GDK_X11_LIBRARIES}
)

target_include_directories(graceful-settings PRIVATE
        ${GTK_INCLUDE_DIRS}
        ${GDK_X11_INCLUDE_DIRS}
)