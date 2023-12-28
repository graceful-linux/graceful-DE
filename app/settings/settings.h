//
// Created by dingjing on 23-12-27.
//

#ifndef GRACEFUL_DE_SETTINGS_H
#define GRACEFUL_DE_SETTINGS_H
#include "config.h"

#include <syslog.h>
#include <stdbool.h>
#include <glib-object.h>

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GD_TYPE_SETTINGS_MANAGER            (gd_settings_manager_get_type())

G_DECLARE_FINAL_TYPE(GDSettingsManager, gd_settings_manager, GD, SETTINGS_MANAGER, GObject)

GDSettingsManager*      gd_settings_manager_new             (void);
bool                    gd_settings_manager_start           (GDSettingsManager* manager, GError** error);
void                    gd_settings_manager_stop            (GDSettingsManager* manager);

G_END_DECLS

#endif //GRACEFUL_DE_SETTINGS_H
