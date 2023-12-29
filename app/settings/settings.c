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
#include <X11/Xatom.h>


#include "config.h"


#define DPI_FALLBACK        96


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
static void init_settings_value         (GDSettingsManager* manager);

static char settings_byte_order         (void);
static gchar settings_get_typecode      (GVariant *value);
static GVariant* settings_setting_get   (SettingsSettings* setting);
static void align_string                (GString* string, gint alignment);
static void setting_store               (SettingsSettings* setting, GString* buffer);
static void update_property             (GString *props, const gchar* key, const gchar* value);

static void terminate_cb                (void* data);
static void device_added_cb             (GdkSeat* uSeat, GdkDevice* device, GDSettingsManager* manager);
static void device_removed_cb           (GdkSeat* uSeat, GdkDevice* device, GDSettingsManager* manager);

static Atom get_atom_settings           (Display* display, int screen, bool onlyExists);        // _GD_SETTINGS_SETTINGS
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

    // 初始化 settings value
    init_settings_value (manager);

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

void gd_settings_manager_set_int(GDSettingsManager *manager, const char *key, int value)
{
    g_return_if_fail(manager && key);

    SettingsSettings* val = g_hash_table_lookup (manager->settings, key);
    if (!val) {
        val = g_new0(SettingsSettings, 1);
        g_assert(val);
        val->name = g_strdup(key);
        g_assert(val->name);
        val->value[1] = NULL;
    }

    GVariant* valT = val->value[0];
    if (valT) {
        int valTT = g_variant_get_int32(valT);
        if (valTT == value) {
            return;
        }
        g_variant_unref (valT);
    }

    val->value[0] = g_variant_new_int32(value);
    val->lastChangeSerial = manager->serial;
    g_hash_table_insert (manager->settings, key, val);
}

void gd_settings_manager_set_string(GDSettingsManager *manager, const char *key, const char* value)
{
    g_return_if_fail(manager && key && value);

    SettingsSettings* val = g_hash_table_lookup (manager->settings, key);
    if (!val) {
        val = g_new0(SettingsSettings, 1);
        g_assert(val);
        val->name = g_strdup(key);
        g_assert(val->name);
        val->value[1] = NULL;
    }

    GVariant* valT = val->value[0];
    if (valT) {
        const char* valTT = g_variant_get_type_string (valT);
        if (0 == g_strcmp0 (valTT, value)) {
            return;
        }
        g_variant_unref (valT);
    }

    val->value[0] = g_variant_new_string (value);
    val->lastChangeSerial = manager->serial;

    g_hash_table_insert (manager->settings, key, val);
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
    manager->settingsAtom = get_atom_settings (display, screen, false);
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
    snprintf (buf, sizeof(buf) - 1, "_XSETTINGS_S%d", screen);

    return XInternAtom (display, buf, onlyExists);
}

static Atom get_atom_settings (Display* display, int screen, bool onlyExists)
{
    return XInternAtom (display, "_XSETTINGS_SETTINGS", onlyExists);
}

static Atom get_atom_manager (Display* display, bool onlyExists)
{
    return XInternAtom (display, "MANAGER", onlyExists);
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

static void init_settings_value (GDSettingsManager* manager)
{
    g_return_if_fail(manager->display);

    Display*            dpy = manager->display;
    char                buf[32] = {0};
    double              dpi = DPI_FALLBACK * GD_SETTINGS_DPI * 1024;
    double              scaledDPI = dpi * 1024;

    {
        GString*            str = g_string_new (XResourceManagerString (dpy));

        g_snprintf (buf, sizeof(buf) - 1, "%d", (int) (dpi / 1024.0 + 0.5));
        printf ("dpi: %f, %s\n", dpi, buf);

        update_property (str, "Xft.dpi", buf);
        update_property (str, "Xft.antialias",      GD_SETTINGS_FONT_ANTIALIASING);
        update_property (str, "Xft.hinting",        GD_SETTINGS_FONT_IS_HINTING);
        update_property (str, "Xft.hintstyle",      GD_SETTINGS_FONT_HINTING);
        update_property (str, "Xft.rgba",           GD_SETTINGS_RGBA);
        update_property (str, "Xcursor.size",       g_ascii_dtostr (buf, sizeof(buf), (double) GD_SETTINGS_CURSOR_SIZE));
        update_property (str, "Xcursor.theme",      GD_SETTINGS_CURSOR_NAME);

        XChangeProperty (dpy, RootWindow(dpy, 0), XA_RESOURCE_MANAGER, XA_STRING, 8, PropModeReplace, (const unsigned char*) str->str, (int) str->len);
        XSync (dpy, false);
        XFlush (dpy);

        g_string_free(str, TRUE);
    }

    gd_settings_manager_set_string (manager, "Gtk/IMModule", GD_SETTINGS_IM_MODULE);
    gd_settings_manager_set_int (manager, "Xft/Antialias", (int) g_ascii_strtoll (GD_SETTINGS_FONT_ANTIALIASING, NULL, 10));
    gd_settings_manager_set_int (manager, "Xft/Hinting", (int) g_ascii_strtoll (GD_SETTINGS_FONT_IS_HINTING, NULL, 10));
    gd_settings_manager_set_string (manager, "Xft/HintStyle", GD_SETTINGS_FONT_HINTING);
    gd_settings_manager_set_int (manager, "Gdk/WindowScalingFactor", 1);
    gd_settings_manager_set_string (manager, "Xft/RGBA", GD_SETTINGS_RGBA);
    gd_settings_manager_set_int (manager, "Gtk/CursorThemeSize", GD_SETTINGS_CURSOR_SIZE);
    gd_settings_manager_set_string (manager, "Gtk/CursorThemeName", GD_SETTINGS_CURSOR_NAME);
    gd_settings_manager_set_string (manager, "Net/IconThemeName", GD_SETTINGS_GTK_THEME);

    gd_settings_manager_set_int (manager, "Gdk/UnscaledDPI", (int) dpi);
    gd_settings_manager_set_int (manager, "Xft/DPI", (int) scaledDPI);

    {
        GString *buffer;
        GHashTableIter iter;
        int n_settings;
        gpointer value;

        n_settings = g_hash_table_size (manager->settings);

        buffer = g_string_new (NULL);
        g_string_append_c (buffer, settings_byte_order ());
        g_string_append_c (buffer, '\0');
        g_string_append_c (buffer, '\0');
        g_string_append_c (buffer, '\0');

        g_string_append_len (buffer, (gchar *) &manager->serial, 4);
        g_string_append_len (buffer, (gchar *) &n_settings, 4);

        g_hash_table_iter_init (&iter, manager->settings);
        while (g_hash_table_iter_next (&iter, NULL, &value)) {
            setting_store (value, buffer);
        }

        for (int i = 0; i < buffer->len; ++i) {
            syslog(LOG_ERR, "%c", buffer->str[i]);
        }

        XChangeProperty (manager->display, manager->window, manager->settingsAtom, manager->settingsAtom, 8, PropModeReplace, (guchar *) buffer->str, buffer->len);
        g_string_free (buffer, TRUE);
        manager->serial++;
    }
}


static void update_property (GString *props, const gchar* key, const gchar* value)
{
    gchar* needle;
    size_t needleLen;
    gchar* found = NULL;

    needle = g_strconcat (key, ":", NULL);
    needleLen = strlen (needle);
    if (g_str_has_prefix (props->str, needle)) {
        found = props->str;
    }
    else {
        found = strstr (props->str, needle);
    }

    if (found) {
        gssize valueIdx;
        gchar* end;

        end = strchr (found, '\n');
        valueIdx = (gssize) ((found - props->str) + needleLen + 1);
        g_string_erase (props, valueIdx, end ? (gssize) (end - found - needleLen) : -1);
        g_string_insert (props, valueIdx, "\n");
        g_string_insert (props, valueIdx, value);
    }
    else {
        g_string_append_printf (props, "%s:\t%s\n", key, value);
    }

    g_free (needle);
}


static void setting_store (SettingsSettings* setting, GString* buffer)
{
    SettingsType type;
    GVariant *value;
    guint16 len16;

    value = settings_setting_get (setting);

    type = settings_get_typecode (value);

    g_string_append_c (buffer, type);
    g_string_append_c (buffer, 0);

    len16 = strlen (setting->name);
    g_string_append_len (buffer, (gchar *) &len16, 2);
    g_string_append (buffer, setting->name);
    align_string (buffer, 4);

    g_string_append_len (buffer, (gchar *) &setting->lastChangeSerial, 4);

    if (type == SETTINGS_TYPE_STRING) {
        const gchar *string;
        gsize stringLen;
        guint32 len32;

        string = g_variant_get_string (value, &stringLen);
        len32 = stringLen;
        g_string_append_len (buffer, (gchar *) &len32, 4);
        g_string_append (buffer, string);
        align_string (buffer, 4);
    }
    else {
        g_string_append_len (buffer, g_variant_get_data (value), g_variant_get_size (value));
    }
}

static void align_string (GString* string, gint alignment)
{
    while ((string->len % alignment) != 0) {
        g_string_append_c (string, '\0');
    }
}

static gchar settings_get_typecode (GVariant *value)
{
    switch (g_variant_classify (value))
    {
        case G_VARIANT_CLASS_INT32: {
            return SETTINGS_TYPE_INT;
        }
        case G_VARIANT_CLASS_STRING: {
            return SETTINGS_TYPE_STRING;
        }
        case G_VARIANT_CLASS_TUPLE: {
            return SETTINGS_TYPE_COLOR;
        }
        default: {
            g_assert_not_reached ();
        }
    }
}

GVariant* settings_setting_get (SettingsSettings* setting)
{
    for (int i = G_N_ELEMENTS (setting->value) - 1; 0 <= i; i--) {
        if (setting->value[i]) {
            return setting->value[i];
        }
    }

    return NULL;
}

static char settings_byte_order (void)
{
    guint32 myInt = 0x01020304;
    return (*(char *)&myInt == 1) ? MSBFirst : LSBFirst;
}