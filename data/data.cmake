install(PROGRAMS    ${CMAKE_SOURCE_DIR}/data/start-graceful-linux       DESTINATION /usr/bin/)
install(FILES       ${CMAKE_SOURCE_DIR}/data/compton.conf               DESTINATION /etc/graceful-linux/)
install(FILES       ${CMAKE_SOURCE_DIR}/data/graceful-linux.desktop     DESTINATION /usr/share/xsessions/)
install(FILES       ${CMAKE_SOURCE_DIR}/data/graceful-wm-wallpaper.svg  DESTINATION /usr/share/themes/graceful-theme/wallpapers/)