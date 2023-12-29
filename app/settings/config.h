//
// Created by dingjing on 23-12-27.
//

#ifndef GRACEFUL_DE_CONFIG_H
#define GRACEFUL_DE_CONFIG_H

/**
 * @brief
 *  interface module (org.gnome.desktop.interface)
 *
 * key-value
 *  avatar-directories          |   []
 *  can-change-accels           |   false
 *  clock-format                |   '24h'
 *  clock-show-date             |   true
 *  clock-show-seconds          |   true
 *  clock-show-weekday          |   true
 *  color-scheme                |   'prefer-dark'
 *  cursor-blink                |   true
 *  cursor-blink-time           |   1200
 *  cursor-blink-timeout        |   10
 *  cursor-size                 |   24
 *  cursor-theme                |   'Bibata-Original-Classic'
 *  document-font-name          |   'WenQuanYi Micro Hei Mono Bold 13'
 *  enable-animations           |   true
 *  enable-hot-corners          |   true
 *  font-antialiasing           |   'grayscale'
 *  font-hinting                |   'full'
 *  font-name                   |   'DejaVu Sans Mono Bold 13'
 *  font-rgba-order             |   'rgb'
 *  gtk-color-palette           |   'black:white:gray50:red:purple:blue:light blue:green:yellow:orange:lavender:brown:goldenrod4:dodger blue:pink:light green:gray10:gray30:gray75:gray90'
 *  gtk-color-scheme            |   ''
 *  gtk-enable-primary-paste    |   true
 *  gtk-im-module               |   ''
 *  gtk-im-preedit-style        |   'callback'
 *  gtk-im-status-style         |   'callback'
 *  gtk-key-theme               |   'Default'
 *  gtk-theme                   |   'Mojave-Dark-solid'
 *  gtk-timeout-initial         |   200
 *  gtk-timeout-repeat          |   20
 *  icon-theme                  |   'McMojave-circle'
 *  locate-pointer              |   false
 *  menubar-accel               |   'F10'
 *  menubar-detachable          |   false
 *  menus-have-tearoff          |   false
 *  monospace-font-name         |   'WenQuanYi Micro Hei Mono Bold 13'
 *  overlay-scrolling           |   true
 *  scaling-factor              |   0
 *  show-battery-percentage     |   true
 *  text-scaling-factor         |   1.0
 *  toolbar-detachable          |   false
 *  toolbar-icons-size          |   'large'
 *  toolbar-style               |   'both-horiz'
 *  toolkit-accessibility       |   false
 */
#define     GD_SETTINGS_IM_MODULE                   "ibus"                                          // Gtk/IMModule
#define     GD_SETTINGS_COLOR_SCHEME                "prefer-dark"
#define     GD_SETTINGS_FONT_ANTIALIASING           "1"                                             // Xft/Antialias: NONE(0), GrayScale(1), RGBA(1)
#define     GD_SETTINGS_FONT_IS_HINTING             "1"                                             // Xft/Hinting: 1 or 0
#define     GD_SETTINGS_DPI                         1.0                                             // dpi
#define     GD_SETTINGS_FONT_HINTING                "hintfull"                                      // Xft/HintStyle: hintfull, hintmedium, hintslight, hintnone
#define     GD_SETTINGS_RGBA                        "rgba"                                          // Xft/RGBA: none, rgba, rgb, bgr, vrgb, vbgr
#define     GD_SETTINGS_CURSOR_SIZE                 24
#define     GD_SETTINGS_CURSOR_NAME                 "Bibata-Original-Classic"
#define     GD_SETTINGS_GTK_THEME                   "Mojave-Dark-solid"
#define     GD_SETTINGS_FONT_NAME                   "WenQuanYi Micro Hei Mono Bold 13"
#define     GD_SETTINGS_GTK_THEME                   "Mojave-Dark-solid"
#define     GD_SETTINGS_ICON_NAME                   "McMojave-circle"



#endif //GRACEFUL_DE_CONFIG_H
