//
// Created by dingjing on 23-12-27.
//

#include "settings.h"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>

#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <gdk/x11/gdkx11screen.h>
#include <gdk/x11/gdkx11display.h>

typedef void (*SettingsTerminateFunc) (void* udata);

typedef struct SettingsColor                SettingsColor;
typedef struct SettingsSettings             SettingsSettings;

typedef enum
{
    SETTINGS_TYPE_INT     = 0,
    SETTINGS_TYPE_STRING  = 1,
    SETTINGS_TYPE_COLOR   = 2
} SettingsType;

struct SettingsColor
{
    unsigned short red, green, blue, alpha;
};

struct SettingsSettings
{
    char*               name;
    GVariant*           value[2];
    unsigned long       lastChangeSerial;
};

struct _GDSettingsManager
{
    GObject                 parent;

    guint                   startIdleID;
    guint                   deviceAddedID;
    guint                   deviceRemovedID;

    // 设置需要
    int                     screen;
    Window                  window;
    unsigned long           serial;
    Display*                display;
    GdkSeat*                userSeat;
    GHashTable*             settings;
    GVariant*               overrides;

    Atom                    managerAtom;
    Atom                    settingsAtom;
    Atom                    selectionAtom;

    SettingsTerminateFunc   terminate;
    bool                    terminateFlag;

};

#define GD_SETTINGS_ERROR       gd_settings_error_quark ()

enum
{
    GD_SETTINGS_ERROR_INIT
};

static void     gd_settings_manager_class_init  (GDSettingsManagerClass*    klass);
static void     gd_settings_manager_init        (GDSettingsManager*         manager);
static void     gd_settings_manager_finalize    (GObject*                   object);

G_DEFINE_TYPE (GDSettingsManager, gd_settings_manager, G_TYPE_OBJECT)

static GQuark gd_settings_error_quark   (void);
static bool check_settings_is_running   (Display* display, int screen);
static void init_settings               (GDSettingsManager* manager, Display* display, int screen);

static void terminate_cb                (void* data);
static void device_added_cb             (GdkSeat* uSeat, GdkDevice* device, GDSettingsManager* manager);
static void device_removed_cb           (GdkSeat* uSeat, GdkDevice* device, GDSettingsManager* manager);

static Atom get_atom_settings           (Display* display, bool onlyExists);                    // _GD_SETTINGS_SETTINGS
static Atom get_atom_manager            (Display* display, bool onlyExists);                    // _GD_SETTINGS_MANAGER
static Atom get_atom_selection          (Display* display, int screen, bool onlyExists);        // _GD_SETTINGS_S<screen num>


static void settings_setting_free       (SettingsSettings* setting);


static gpointer managerObj = NULL;



static void gd_settings_manager_class_init (GDSettingsManagerClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = gd_settings_manager_finalize;
}

static void gd_settings_manager_init (GDSettingsManager *manager)
{
}

static void gd_settings_manager_finalize (GObject *object)
{
    g_return_if_fail (object != NULL);
    g_return_if_fail (GD_IS_SETTINGS_MANAGER (object));

    GDSettingsManager* manager = GD_SETTINGS_MANAGER (object);
    g_return_if_fail (manager != NULL);

    gd_settings_manager_stop (manager);

    if (manager->startIdleID != 0) {
        g_source_remove (manager->startIdleID);
    }

    G_OBJECT_CLASS (gd_settings_manager_parent_class)->finalize (object);
}


bool gd_settings_manager_start(GDSettingsManager *manager, GError **error)
{
    // 检测是否已经运行
    GdkDisplay* dsp = gdk_display_get_default();
    GdkScreen* screen = gdk_screen_get_default();
    if (check_settings_is_running (gdk_x11_display_get_xdisplay(dsp), gdk_x11_screen_get_screen_number(screen))) {
        if (error) {
            g_set_error (error, GD_SETTINGS_ERROR, GD_SETTINGS_ERROR_INIT, "Could not initialize settings manager.");
        }
        return false;
    }

    // 初始化 settings
    init_settings (manager, gdk_x11_display_get_xdisplay (dsp), gdk_x11_screen_get_screen_number (screen));



    return true;
}

GDSettingsManager* gd_settings_manager_new(void)
{
    if (managerObj) {
        g_object_ref(managerObj);
    }
    else {
        managerObj = g_object_new (GD_TYPE_SETTINGS_MANAGER, NULL);
        g_object_add_weak_pointer (managerObj, (gpointer*) &managerObj);
    }

    return GD_SETTINGS_MANAGER (managerObj);
}

void gd_settings_manager_stop(GDSettingsManager *manager)
{

}

static bool check_settings_is_running (Display* display, int screen)
{
    Atom selectionAtom = get_atom_selection (display, screen, False);

    return (None != XGetSelectionOwner (display, selectionAtom));
}

static void init_settings (GDSettingsManager* manager, Display* display, int screen)
{
    Time timeStamp = CurrentTime;
    XClientMessageEvent xev;

    XFixesSetClientDisconnectMode (display, XFixesClientDisconnectFlagTerminate);

    manager->display = display;
    manager->screen = screen;
    manager->selectionAtom = get_atom_selection (display, screen, false);
    manager->settingsAtom = get_atom_settings (display, false);
    manager->managerAtom = get_atom_manager (display, false);

    manager->terminate = terminate_cb;
    manager->terminateFlag = false;

    manager->settings = g_hash_table_new_full (g_str_hash, g_str_equal, NULL, (GDestroyNotify) settings_setting_free);
    manager->serial = 0;
    manager->overrides = NULL;

    manager->window = XCreateSimpleWindow (display, RootWindow(display, screen), 0, 0, 10, 10, 0, WhitePixel(display, screen), WhitePixel(display, screen));
    XSelectInput (display, manager->window, PropertyChangeMask);
    XSetSelectionOwner (display, manager->selectionAtom, manager->window, timeStamp);

    if (XGetSelectionOwner (display, manager->selectionAtom) == manager->window) {
        xev.type = ClientMessage;
        xev.window = RootWindow(display, screen);
        xev.message_type = manager->managerAtom;
        xev.format = 32;
        xev.data.l[0] = (long) timeStamp;
        xev.data.l[1] = (long) manager->selectionAtom;
        xev.data.l[2] = (long) manager->window;
        xev.data.l[3] = 0;
        xev.data.l[4] = 0;

        XSendEvent (display, RootWindow(display, screen), false, StructureNotifyMask, (XEvent*) & xev);
    }
    else {
        manager->terminate (&(manager->terminateFlag));
    }

    manager->userSeat = gdk_display_get_default_seat (gdk_display_get_default());
    manager->deviceAddedID = g_signal_connect(G_OBJECT(manager->userSeat), "device-added", G_CALLBACK(device_added_cb), manager);
    manager->deviceRemovedID = g_signal_connect(G_OBJECT(manager->userSeat), "device-removed", G_CALLBACK(device_removed_cb), manager);
}

static GQuark gd_settings_error_quark (void)
{
    return g_quark_from_static_string ("gd-settings-error-quark");
}

static Atom get_atom_selection (Display* display, int screen, bool onlyExists)
{
    char buf[64] = {0};
    snprintf (buf, sizeof(buf) - 1, "_GD_SETTINGS_S%d", screen);

    return XInternAtom (display, buf, onlyExists);
}

static Atom get_atom_settings (Display* display, bool onlyExists)
{
    return XInternAtom (display, "_GD_SETTINGS_SETTINGS", onlyExists);
}

static Atom get_atom_manager (Display* display, bool onlyExists)
{
    return XInternAtom (display, "_GD_SETTINGS_MANAGER", onlyExists);
}

static void terminate_cb (void* data)
{
    gboolean* terminated = data;
    if (*terminated) {
        return;
    }

    *terminated = TRUE;
    gtk_main_quit();
}

static void settings_setting_free (SettingsSettings* setting)
{
    for (int i = 0; i < G_N_ELEMENTS(setting->value); ++i) {
        if (setting->value[i]) {
            g_variant_unref (setting->value[i]);
        }
    }

    g_free (setting->name);
    g_free(setting);
}

static void device_added_cb (GdkSeat* uSeat, GdkDevice* device, GDSettingsManager* manager)
{
    if (GDK_SOURCE_TOUCHPAD == gdk_device_get_source (device)) {
        //
    }
}

static void device_removed_cb (GdkSeat* uSeat, GdkDevice* device, GDSettingsManager* manager)
{
    if (GDK_SOURCE_TOUCHSCREEN == gdk_device_get_source (device)) {
        //
    }
}