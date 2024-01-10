//
// Created by dingjing on 1/10/24.
//

#ifndef GRACEFUL_DE_ATOMS_H
#define GRACEFUL_DE_ATOMS_H
#include <stdlib.h>
#include <stdbool.h>
#include <X11/Xlib.h>

typedef enum AtomEnum       AtomEnum;


/**
 * @brief
 *  所有的 Atom
 */
enum AtomEnum
{
    // WM Atom
    ATOM_WM_PROTOCOLS,
    ATOM_WM_DELETE,
    ATOM_WM_STATE,
    ATOM_WM_TAKE_FOCUS,
    ATOM_WM_WINDOW_ROLE,

    // Net Atom
    ATOM_NET_SUPPORTED,
    ATOM_NET_WM_NAME,
    ATOM_NET_WM_STATE,
    ATOM_NET_WM_CHECK,
    ATOM_NET_WM_FULL_SCREEN,
    ATOM_NET_ACTIVE_WINDOW,
    ATOM_NET_WM_WINDOW_TYPE,
    ATOM_NET_WM_ICON,
    ATOM_NET_SYSTEM_TRAY,
    ATOM_NET_SYSTEM_TRAY_OP,
    ATOM_NET_SYSTEM_TRAY_ORIENTATION,
    ATOM_NET_SYSTEM_TRAY_VISUAL,
    ATOM_NET_WM_WINDOW_TYPE_DOCK,
    ATOM_NET_SYSTEM_TRAY_ORIENTATION_HORIZ,
    ATOM_NET_DESKTOP_NAMES,
    ATOM_NET_DESKTOP_VIEWPORT,
    ATOM_NET_NUMBER_OF_DESKTOPS,
    ATOM_NET_CURRENT_DESKTOP,
    ATOM_NET_CLIENT_LIST,
    ATOM_NET_CLIENT_LIST_STACKING,

    // Common Atom
    ATOM_COMMON_UTF8_STRING,

    // Custom Atom
    ATOM_CUSTOM_DATE_TIME,

    // X Atom
    ATOM_X_MANAGER,
    ATOM_X_EMBED,
    ATOM_X_EMBED_INFO,

    // Atom Count
    // 后续添加的 Atom 不能位于 ATOM_ATOM_LAST 后
    ATOM_ATOM_LAST,

    // 特殊变量，不能使用
    ATOM_ATOM_UNKNOWN,
};


// TODO:// 后续对这些 Atom 和 value 创建索引，不必每次都 X 查服务器，提升效率
// NOTE:// 如何确保 X server 上指定 Atom 没有被删除（使用时候要注意）
bool gwm_atoms_atom_init (Display* dsp);
Atom gwm_atoms_get_atom_by_enum (Display* dsp, int atomEnum);
Atom gwm_atoms_get_atom_by_name (Display* dsp, const char* atomName, bool ifExists);


#endif //GRACEFUL_DE_ATOMS_H
