#include <X11/XF86keysym.h>

/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx = 3;     /* border pixel of windows */
static const unsigned int gappx = 2;        /* gaps between windows */
static const unsigned int snap = 32;        /* snap pixel */
static const int showbar = 1;               /* 0 means no bar */
static const int topbar = 1;                /* 0 means bottom bar */
static const int usealtbar = 1;             /* 1 means use non-dwm status bar */
static const char *altbarclass = "Polybar"; /* Alternate bar class name */
static const char *alttrayname = "tray";    /* Polybar tray instance name */
static const char *altbarcmd = "$HOME/.dwmpobar"; /* Alternate bar launch command */
/*  Display modes of the tab bar: never shown, always shown, shown only in  */
/*  monocle mode in the presence of several windows.                        */
/*  Modes after showtab_nmodes are disabled.                                */
enum showtab_modes
{
    showtab_never,
    showtab_auto,
    showtab_nmodes,
    showtab_always
};
static const int showtab = showtab_auto; /* Default tab bar show mode */
static const int toptab = False;         /* False means bottom tab bar */

static const char *fonts[] = {
    "BlexMono Nerd Font:size=9:antialias=true:autohint=true",
    "Sarasa UI SC:size=8:antialias=true:autohint=true",
    "JoyPixels:size=10:antialias=true:autohint=true"
};

static const char dmenufont[] = "Sarasa UI SC:size=10:antialias=true:autohint=true";

static char normbgcolor[] = "#2E3440";
static char normbordercolor[] = "#3B4252";
static char normfgcolor[] = "#ECEFF4";
static char selfgcolor[] = "#D8DEE9";
static char selbordercolor[] = "#5E81AC";
static char selbgcolor[] = "#5E81AC";
static char termcol0[] = "#3b4252";  /* black   */
static char termcol1[] = "#bf616a";  /* red     */
static char termcol2[] = "#a3be8c";  /* green   */
static char termcol3[] = "#ebcb8b";  /* yellow  */
static char termcol4[] = "#81a1c1";  /* blue    */
static char termcol5[] = "#b48ead";  /* magenta */
static char termcol6[] = "#88c0d0";  /* cyan    */
static char termcol7[] = "#e5e9f0";  /* white   */
static char termcol8[] = "#4c566a";  /* black   */
static char termcol9[] = "#bf616a";  /* red     */
static char termcol10[] = "#a3be8c"; /* green   */
static char termcol11[] = "#ebcb8b"; /* yellow  */
static char termcol12[] = "#81a1c1"; /* blue    */
static char termcol13[] = "#b48ead"; /* magenta */
static char termcol14[] = "#8fbcbb"; /* cyan    */
static char termcol15[] = "#eceff4"; /* white   */

static char *termcolor[] = {
    termcol0,  termcol1,  termcol2,  termcol3,  termcol4,  termcol5,
    termcol6,  termcol7,  termcol8,  termcol9,  termcol10, termcol11,
    termcol12, termcol13, termcol14, termcol15,
};

static char *colors[][3] = {
    /*               fg           bg           border   */
    [SchemeNorm] = {normfgcolor, normbgcolor, normbordercolor},
    [SchemeSel] = {selfgcolor, selbgcolor, selbordercolor},
};

/* tagging */
// static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
static const char *tags[] = {"1", "", "", "", "", "", "", "", ""};

static const Rule rules[] = {
    /* xprop(1):
     *    WM_CLASS(STRING) = instance, class
     *    WM_NAME(STRING) = title
     */
    /* class  instance  title   tags mask  isfloating  monitor */
    {"firefox-nightly", NULL, NULL, 1 << 0, 0, 0},
    {"firefox", NULL, NULL, 1 << 0, 0, 0},
    {"Typora", NULL, NULL, 1 << 0, 0, 1},

    {"jetbrains-*", "JetBrains Toolbox", NULL, 1 << 1, 1, 0},
    {"jetbrains-*", "sun-awt-X11-XFramePeer", NULL, 1 << 1, 0, 0},
    {"jetbrains-*", "jetbrains-*", "win0", 1 << 1, 1, 0},
    {"jetbrains-*", NULL, "Welcome to*", 1 << 1, 1, 0},
    {"jetbrains-*", NULL, "Welcome to*", 1 << 1, 1, 0},
    {"jetbrains-idea", NULL, NULL, 1 << 1, 0, 0},
    {"jetbrains-clion", NULL, NULL, 1 << 1, 0, 0},
    {"jetbrains-pycharm", NULL, NULL, 1 << 1, 0, 0},
    {"jetbrains-webstorm", NULL, NULL, 1 << 1, 0, 0},

    {"Google-chrome", "google-chrome", NULL, 1 << 2, 0, 0},

    {"Steam", NULL, NULL, 1 << 4, 0, 0},

    {"discord", "discord", NULL, 1 << 6, 0, 1},

    {"TelegramDesktop", NULL, NULL, 1 << 7, 0, 1},

    {"stalonetray", NULL, NULL, 1 << 8, 1, 0},

    {"wemeetapp", NULL, NULL, 0, 1, -1},
    {"xdman-Main", NULL, NULL, 0, 1, -1},
    {"copyq", NULL, NULL, 0, 1, -1},
    {"pavucontrol", NULL, NULL, 0, 1, -1},
    {"Nitrogen", NULL, NULL, 0, 1, -1},
    {"lxappearance", NULL, NULL, 0, 1, -1},
    {"Org.gnome.Nautilus", NULL, NULL, 0, 1, -1},

};

/* layout(s) */
static const float mfact = 0.50; /* factor of master area size [0.05..0.95] */
static const int nmaster = 1;    /* number of clients in master area */
static const int resizehints =
    1; /* 1 means respect size hints in tiled resizals */

#include "layouts.c"
#include "tcl.c"
static const Layout layouts[] = {
    /* symbol     arrange function */
    {"[]=", tile}, /* first entry is default */
    {"><>", NULL}, /* no layout function means floating behavior */
    {"[M]", monocle}, {"HHH", grid},        {"|||", tcl},
    {"TTT", bstack},  {"===", bstackhoriz},

};

/**
 * @brief 按键定义(必须按下的是 Win 键)
 *  MOD1: 常规操作
 *
 *  Grave(`)
 *
 */
#define MOD1_KEY        Mod4Mask                        // Mod4Mask(win键), Mod1Mask(Alt), Mod5Mask(option)
#define MOD2_KEY        (MOD1_KEY | ShiftMask)
//#define

#define TARGET_KEYS(KEY, TAG)                                                     \
    {MOD1_KEY, KEY, view, {.ui = 1 << TAG}},                                      \
    {MOD1_KEY | ControlMask, KEY, toggleview, {.ui = 1 << TAG}},                  \
    {MOD1_KEY | ShiftMask, KEY, tag, {.ui = 1 << TAG}},                           \
    {MOD1_KEY | ControlMask | ShiftMask, KEY, toggletag, {.ui = 1 << TAG}},

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define CMD(cmd)                                                                \
{                                                                               \
    .v = (const char *[]) { "/bin/sh", "-c", cmd, NULL }                        \
}

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *termcmd[] = {"terminator", NULL};
static const char scratchpadname[] = "scratchpad";

static const char *windowswitchcmd[] = {"rofi", "-show", "window", NULL};

static const char *dmenucmd[] = {"dmenu_run_history", NULL};
static const char *toggletraycmd[] = {"toggletray", NULL};

static const char *upvol[] = {"/usr/bin/pactl", "set-sink-volume", "0", "+3%", NULL};
static const char *downvol[] = {"/usr/bin/pactl", "set-sink-volume", "0", "-3%", NULL};
static const char *mutevol[] = {"/usr/bin/pactl", "set-sink-mute", "0", "toggle", NULL};

static const char *upbrt[] = {"light", "-A", "5", NULL};
static const char *downbrt[] = {"light", "-U", "5", NULL};

/**
 * @brief 快捷键，依次是：功能切换键 + 按键 + 执行函数 + 执行函数的参数
 */
static Key keys[] = {
    {MOD1_KEY, XK_Return, spawn, {.v = termcmd}},
    {MOD1_KEY, XK_d, spawn, {.v = dmenucmd}},
    {MOD1_KEY, XK_w, spawn, {.v = windowswitchcmd}},
//    {MOD1_KEY | ShiftMask, XK_t, spawn, {.v = toggletraycmd}},
//    {MOD1_KEY, XK_b, togglebar, {0}},
    /* {MOD1_KEY, XK_o, incnmaster, {.i = -1}}, */
//    {MOD1_KEY | ControlMask, XK_m, focusmaster, {0}},
    {MOD1_KEY, XK_Right, focusstack, {.i = +1}},
    {MOD1_KEY, XK_Left,  focusstack, {.i = -1}},
//    {MOD1_KEY, XK_k, focusmon, {.i = -1}},
//    {MOD1_KEY, XK_l, focusmon, {.i = +1}},
//    {MOD1_KEY, XK_comma, setmfact, {.f = -0.05}},
//    {MOD1_KEY, XK_period, setmfact, {.f = +0.05}},
//    {MOD1_KEY | ControlMask, XK_Return, zoom, {0}},
    {MOD1_KEY, XK_Tab, view, {0}},
    {MOD1_KEY, XK_c, spawn, CMD("clion")},
    {MOD1_KEY, XK_q, killclient, {0}},
    {MOD1_KEY, XK_y, tabmode, {-1}},
    {MOD1_KEY, XK_t, setlayout, {.v = &layouts[0]}},
    {MOD1_KEY, XK_f, setlayout, {.v = &layouts[1]}},
    {MOD1_KEY, XK_m, setlayout, {.v = &layouts[2]}},
    {MOD1_KEY, XK_g, setlayout, {.v = &layouts[3]}},
    {MOD1_KEY, XK_u, setlayout, {.v = &layouts[4]}},
    {MOD1_KEY, XK_i, setlayout, {.v = &layouts[5]}},
    {MOD1_KEY, XK_o, setlayout, {.v = &layouts[6]}},
//    {MOD1_KEY | ShiftMask, XK_f, fullscreen, {0}},
//    {MOD1_KEY | ShiftMask, XK_space, togglefloating, {0}},
    {MOD1_KEY, XK_0, view, {.ui = ~0}},
    {MOD2_KEY, XK_0, tag, {.ui = ~0}},
    {MOD1_KEY | ShiftMask, XK_h, tagmon, {.i = -1}},
    {MOD1_KEY | ShiftMask, XK_l, tagmon, {.i = +1}},
    {MOD1_KEY, XK_F5, xrdb, {.v = NULL}},

    /* My Own App Start Ways */
//    {MOD1_KEY, XK_e, spawn, CMD("firefox-nightly")},
    {MOD2_KEY, XK_q, spawn, CMD("xkill")},
    {MOD2_KEY, XK_s, spawn, CMD("flameshot gui")},
    {MOD2_KEY, XK_n, spawn, CMD("nautilus")},
//    {MOD2_KEY, XK_m, spawn, CMD("st -c music -e ncmpcpp")},

//    {Mod1Mask | ControlMask, XK_Delete, spawn, CMD("betterlockscreen -l")},

//    {Mod1Mask | ShiftMask, XK_p, spawn, CMD("sh ~/.dwmpobar")},


    /* Mpd control */
//    {MOD1_KEY | ControlMask, XK_p, spawn, CMD("mpc toggle")},
//    {MOD1_KEY | ControlMask, XK_Left, spawn, CMD("mpc prev")},
//    {MOD1_KEY | ControlMask, XK_Right, spawn, CMD("mpc next")},

//    {Mod1Mask | ControlMask, XK_p, spawn, CMD("playerctl play-pause")},
//    {Mod1Mask | ControlMask, XK_Left, spawn, CMD("playerctl previous")},
//    {Mod1Mask | ControlMask, XK_Right, spawn, CMD("playerctl next")},

    /* XF86Keys */
//    {0, XF86XK_AudioMute, spawn, {.v = mutevol}},
//    {0, XF86XK_AudioLowerVolume, spawn, {.v = downvol}},
//    {0, XF86XK_AudioRaiseVolume, spawn, {.v = upvol}},
//    {0, XF86XK_MonBrightnessUp, spawn, {.v = upbrt}},
//    {0, XF86XK_MonBrightnessDown, spawn, {.v = downbrt}},

    {MOD1_KEY | ControlMask, XK_q, quit, {0}},
    {MOD1_KEY | ControlMask, XK_r, quit, {1}},

    TARGET_KEYS(XK_1, 0)
    TARGET_KEYS(XK_2, 1)
    TARGET_KEYS(XK_3, 2)
    TARGET_KEYS(XK_4, 3)
    TARGET_KEYS(XK_5, 4)
    TARGET_KEYS(XK_6, 5)
    TARGET_KEYS(XK_7, 6)
    TARGET_KEYS(XK_8, 7)
    TARGET_KEYS(XK_9, 8)
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle,
 * ClkClientWin, or ClkRootWin */
static Button buttons[] = {
    /* click                event mask      button          function argument
     */
    {ClkLtSymbol, 0, Button1, setlayout, {0}},
    {ClkLtSymbol, 0, Button3, setlayout, {.v = &layouts[2]}},
    {ClkWinTitle, 0, Button2, zoom, {0}},
    {ClkStatusText, 0, Button2, spawn, {.v = termcmd}},
    {ClkClientWin, MOD1_KEY, Button1, movemouse, {0}},
    {ClkClientWin, MOD1_KEY, Button2, togglefloating, {0}},
    {ClkClientWin, MOD1_KEY, Button3, resizemouse, {0}},
    {ClkTagBar, 0, Button1, view, {0}},
    {ClkTagBar, 0, Button3, toggleview, {0}},
    {ClkTagBar, MOD1_KEY, Button1, tag, {0}},
    {ClkTagBar, MOD1_KEY, Button3, toggletag, {0}},
    {ClkTabBar, 0, Button1, focuswin, {0}},
};

static const char *ipcsockpath = "/tmp/dwm.sock";
static IPCCommand ipccommands[] = {
    IPCCOMMAND(view, 1, {ARG_TYPE_UINT}),
    IPCCOMMAND(toggleview, 1, {ARG_TYPE_UINT}),
    IPCCOMMAND(tag, 1, {ARG_TYPE_UINT}),
    IPCCOMMAND(toggletag, 1, {ARG_TYPE_UINT}),
    IPCCOMMAND(tagmon, 1, {ARG_TYPE_UINT}),
    IPCCOMMAND(focusmon, 1, {ARG_TYPE_SINT}),
    IPCCOMMAND(focusstack, 1, {ARG_TYPE_SINT}),
    IPCCOMMAND(zoom, 1, {ARG_TYPE_NONE}),
    IPCCOMMAND(incnmaster, 1, {ARG_TYPE_SINT}),
    IPCCOMMAND(killclient, 1, {ARG_TYPE_SINT}),
    IPCCOMMAND(togglefloating, 1, {ARG_TYPE_NONE}),
    IPCCOMMAND(setmfact, 1, {ARG_TYPE_FLOAT}),
    IPCCOMMAND(setlayoutsafe, 1, {ARG_TYPE_PTR}),
    IPCCOMMAND(quit, 1, {ARG_TYPE_NONE})};
