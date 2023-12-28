//
// Created by dingjing on 23-12-27.
//
#include <locale.h>
#include <glib-unix.h>

#include "settings.h"


static gboolean handle_sigterm(gpointer udata);


int main (int argc, char* argv[])
{
    setlocale(LC_ALL, "");

    g_autoptr(GError) error = NULL;

    gdk_set_allowed_backends ("x11");
    g_unsetenv ("GDK_BACKEND");

    if (!gtk_init_with_args (&argc, &argv, "graceful-settings", NULL, NULL, &error)) {
        if (NULL != error) {
            syslog (LOG_ERR, "%s", error->message);
        }
        exit (1);
    }

    {
        // handle SIGTERM
        g_autoptr(GSource) source = g_unix_signal_source_new(SIGTERM);
        g_source_set_callback (source, handle_sigterm, NULL, NULL);
        g_source_attach (source, NULL);
    }

    g_autoptr(GDSettingsManager) gds = gd_settings_manager_new();

    gd_settings_manager_start (gds, &error);
    if (error) {
        syslog (LOG_ERR, "%s", error->message);
        return 2;
    }

    gtk_main ();

    gd_settings_manager_stop (gds);

    return 0;
}

static gboolean handle_sigterm(gpointer udata)
{
    if (gtk_main_level() > 0) {
        gtk_main_quit();
    }

    return G_SOURCE_REMOVE;
}
