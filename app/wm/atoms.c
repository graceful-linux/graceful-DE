//
// Created by dingjing on 1/10/24.
//

#include "atoms.h"

#include <glib.h>

#include "log.h"


typedef struct AtomInfo     AtomInfo;

struct AtomInfo
{
    Atom                atom;
    AtomEnum            atomEnum;
    char*               atomName;
};


static GRWLock          gsAtomLock;
static GHashTable*      gsAtomAN = NULL;                        // Atom         -> Atom Enum
static GHashTable*      gsAtomNA = NULL;                        // Atom Name    -> Atom Enum
static AtomInfo*        gsAtomInfo[ATOM_ATOM_LAST] = {NULL};


static Atom         get_atom_atom (AtomEnum eum);
static char*        get_atom_atom_name (AtomEnum eum);
static AtomEnum     get_atom_enum_by_name (const char* name);
static void         register_atom (Display* dsp, AtomEnum eum, const char* atomName);

bool gwm_atoms_atom_init(Display *dsp)
{
    g_return_val_if_fail(G_UNLIKELY (dsp), false);

    static gsize init = 0;

    if (G_UNLIKELY(g_once_init_enter (&init))) {
        g_rw_lock_init (&gsAtomLock);
        gsAtomAN = g_hash_table_new_full (g_int64_hash, g_int64_equal, NULL, NULL);
        gsAtomNA = g_hash_table_new_full (g_str_hash, g_str_equal, NULL, NULL);
        if (!gsAtomAN || !gsAtomNA) {
            return false;
        }
        g_once_init_leave(&init, 1);
    }
    else {
        return true;
    }

    // WM Atom
    {
        register_atom (dsp, ATOM_WM_PROTOCOLS, "WM_PROTOCOLS");
        register_atom (dsp, ATOM_WM_DELETE, "WM_DELETE_WINDOW");
        register_atom (dsp, ATOM_WM_STATE, "WM_STATE");
        register_atom (dsp, ATOM_WM_TAKE_FOCUS, "WM_TAKE_FOCUS");
        register_atom (dsp, ATOM_WM_WINDOW_ROLE, "WM_WINDOW_ROLE");
    }

    // NET Atom
    {
        register_atom (dsp, ATOM_NET_SUPPORTED, "_NET_SUPPORTED");
        register_atom (dsp, ATOM_NET_WM_NAME, "_NET_WM_NAME");
        register_atom (dsp, ATOM_NET_WM_STATE, "_NET_WM_STATE");
        register_atom (dsp, ATOM_NET_WM_CHECK, "_NET_SUPPORTING_WM_CHECK");
        register_atom (dsp, ATOM_NET_WM_FULL_SCREEN, "_NET_WM_STATE_FULLSCREEN");
        register_atom (dsp, ATOM_NET_ACTIVE_WINDOW, "_NET_ACTIVE_WINDOW");
        register_atom (dsp, ATOM_NET_WM_WINDOW_TYPE, "_NET_WM_WINDOW_TYPE");
        register_atom (dsp, ATOM_NET_WM_ICON, "_NET_WM_ICON");
        register_atom (dsp, ATOM_NET_SYSTEM_TRAY, "_NET_SYSTEM_TRAY_S0");
        register_atom (dsp, ATOM_NET_SYSTEM_TRAY_OP, "_NET_SYSTEM_TRAY_OPCODE");
        register_atom (dsp, ATOM_NET_SYSTEM_TRAY_ORIENTATION, "_NET_SYSTEM_TRAY_ORIENTATION");
        register_atom (dsp, ATOM_NET_SYSTEM_TRAY_VISUAL, "_NET_SYSTEM_TRAY_VISUAL");
        register_atom (dsp, ATOM_NET_WM_WINDOW_TYPE_DOCK, "_NET_WM_WINDOW_TYPE_DOCK");
        register_atom (dsp, ATOM_NET_SYSTEM_TRAY_ORIENTATION_HORIZ, "_NET_SYSTEM_TRAY_ORIENTATION_HORZ");
        register_atom (dsp, ATOM_NET_DESKTOP_NAMES, "_NET_DESKTOP_NAMES");
        register_atom (dsp, ATOM_NET_DESKTOP_VIEWPORT, "_NET_DESKTOP_VIEWPORT");
        register_atom (dsp, ATOM_NET_NUMBER_OF_DESKTOPS, "_NET_NUMBER_OF_DESKTOPS");
        register_atom (dsp, ATOM_NET_CURRENT_DESKTOP, "_NET_CURRENT_DESKTOP");
        register_atom (dsp, ATOM_NET_CLIENT_LIST, "_NET_CLIENT_LIST");
        register_atom (dsp, ATOM_NET_CLIENT_LIST_STACKING, "_NET_CLIENT_LIST_STACKING");
    }

    // Common Atom
    {
        register_atom (dsp, ATOM_COMMON_UTF8_STRING, "UTF8_STRING");
    }

    // Custom Atom
    {
        register_atom (dsp, ATOM_CUSTOM_DATE_TIME, "CUSTOM_STATUS_TIME");
    }

    // X Atom
    {
        register_atom(dsp, ATOM_X_MANAGER, "MANAGER");
        register_atom(dsp, ATOM_X_EMBED, "_XEMBED");
        register_atom(dsp, ATOM_X_EMBED_INFO, "_XEMBED_INFO");
    }

    return true;
}

Atom gwm_atoms_get_atom_by_enum(Display *dsp, int atomEnum)
{
    g_return_val_if_fail(G_LIKELY (dsp && ((atomEnum >= 0) && (atomEnum < ATOM_ATOM_LAST))), None);

    gwm_atoms_atom_init (dsp);

    return get_atom_atom (atomEnum);
}

Atom gwm_atoms_get_atom_by_name(Display *dsp, const char *atomName, bool ifExists)
{
    g_return_val_if_fail(G_LIKELY (dsp && atomName), None);

    gwm_atoms_atom_init (dsp);

    Atom atom = None;
    AtomEnum atomEnum = get_atom_enum_by_name (atomName);
    if (atomEnum < ATOM_ATOM_LAST) {
        atom = get_atom_atom (atomEnum);
    }

    if (None == atom) {
        atom = XInternAtom (dsp, atomName, ifExists);
    }

    return atom;
}

static void register_atom (Display* dsp, AtomEnum eum, const char* atomName)
{
    g_return_if_fail(G_UNLIKELY(dsp && atomName && (eum >=0 && eum < ATOM_ATOM_LAST)));

    g_rw_lock_writer_lock (&gsAtomLock);
    if (!gsAtomInfo[eum]) {
        Atom atom = XInternAtom (dsp, atomName, false);
        if (None != atom) {
            AtomInfo* info = g_malloc0 (sizeof(AtomInfo));
            info->atom = atom;
            info->atomEnum = eum;
            info->atomName = g_strdup(atomName);
            if (G_UNLIKELY((!info->atomName))
                || g_hash_table_contains (gsAtomAN, (void*) &(info->atom))
                || g_hash_table_contains (gsAtomNA, (void*) (info->atomName))) {
                g_free (info);
                LOG_WARNING("Memory malloc error or hash error!");
            }
            else {
                g_hash_table_insert (gsAtomAN, (void*) &(info->atom), (void*) (info->atomEnum));
                g_hash_table_insert (gsAtomNA, (void*) (info->atomName), (void*) (info->atom));
            }
        }
    }
    g_rw_lock_writer_unlock (&gsAtomLock);
}

static Atom get_atom_atom (AtomEnum eum)
{
    g_return_val_if_fail(G_LIKELY ((eum >= 0) && (eum < ATOM_ATOM_LAST)), None);

    Atom atom = None;

    g_rw_lock_reader_lock (&gsAtomLock);
    AtomInfo* info = gsAtomInfo[eum];
    if (info) {
        atom = info->atom;
    }
    g_rw_lock_reader_unlock (&gsAtomLock);

    return atom;
}

static char* get_atom_atom_name (AtomEnum eum)
{
    g_return_val_if_fail(G_LIKELY ((eum >= 0) && (eum < ATOM_ATOM_LAST)), NULL);

    char* name = NULL;

    g_rw_lock_reader_lock (&gsAtomLock);
    AtomInfo* info = gsAtomInfo[eum];
    if (info) {
        name = g_strdup(info->atomName);
    }
    g_rw_lock_reader_unlock (&gsAtomLock);

    return name;
}

static AtomEnum get_atom_enum_by_name (const char* name)
{
    g_return_val_if_fail(G_LIKELY (name), ATOM_ATOM_UNKNOWN);

    AtomEnum eum = ATOM_ATOM_UNKNOWN;

    g_rw_lock_reader_lock (&gsAtomLock);
    if (g_hash_table_contains (gsAtomNA, name)) {
        eum = (AtomEnum) g_hash_table_find (gsAtomNA, name, NULL);
    }
    g_rw_lock_reader_unlock (&gsAtomLock);

    return eum;
}
