//
// Created by dingjing on 1/9/24.
//

#ifndef GRACEFUL_DE_GRACEFUL_WM_APP_H
#define GRACEFUL_DE_GRACEFUL_WM_APP_H

#include <glib.h>
#include <stdbool.h>
#include <X11/Xlib.h>
#include <glib-object.h>
#include <X11/Xlib-xcb.h>

#include "log.h"


G_BEGIN_DECLS

#define GWM_TYPE_APP                            (gwm_app_get_type())

G_DECLARE_FINAL_TYPE(GWMApp, gwm_app, GWM, APP, GObject)

GWMApp*             gwm_app_new                 (int argc, char* argv[]);
bool                gwm_app_initialize          (GWMApp* app, GError** error);
void                gwm_app_exit                (GWMApp* app);

Display*            gwm_get_display             (GWMApp* app);
xcb_connection_t*   gwm_get_xcb_connection      (GWMApp* app);


G_END_DECLS

#endif //GRACEFUL_DE_GRACEFUL_WM_APP_H
