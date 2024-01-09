//
// Created by dingjing on 1/9/24.
//

#include "graceful-wm-app.h"

#include <time.h>
#include <wait.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <locale.h>
#include <stdbool.h>
#include <sys/time.h>
#include <sys/types.h>
#include <X11/Xproto.h>

#include "log.h"
#include "macro.h"


typedef struct _GWMApp      GWMApp;

struct _GWMApp
{
    GObject                 parent;

    Display*                display;
    xcb_connection_t*       xcbConn;
    Window                  rootWindow;
    int                     defaultScreen;

    bool                    isRunning;
};

G_DEFINE_TYPE(GWMApp, gwm_app, G_TYPE_OBJECT)

static void gwm_app_class_init              (GWMAppClass* klass);
static void gwm_app_init                    (GWMApp* obj);
static void gwm_app_finalize                (GWMApp* obj);


static void timer_handler(int signum);

int x_error_handler (Display *dpy, XErrorEvent *ee);
int x_error_handle_has_wm(Display* dsp, XErrorEvent* ee);

static GWMApp*                      gsApp = NULL;
static XErrorHandler                gsDefaultXErrorHandler = NULL;


GWMApp *gwm_app_new(int argc, char **argv)
{
    {
        // 解析命令行参数
        g_autoptr(GError) error = NULL;
        bool isVersion = 0;
        GOptionEntry entries[] = {
            {"version", 'v', 0, G_OPTION_ARG_NONE, &isVersion, "Display " APP_NAME "'s version."},
            G_OPTION_ENTRY_NULL
        };
        g_autoptr(GOptionContext) ctx = g_option_context_new (NULL);
        g_option_context_add_main_entries (ctx, entries, APP_NAME);
        if (!g_option_context_parse (ctx, &argc, &argv, &error)) {
            g_print ("parse cmd error: %s\n", error->message);
            LOG_DIE("parse cmd error: %s", error->message);
        }
        if (isVersion) {
            g_print (APP_NAME " " VERSION "\n");
            exit (0);
        }
    }

    // FIXME:// 检测当前是否有同样的实例存在

    static GWMApp *app = NULL;
    static gsize initVal = 0;

    if (g_once_init_enter(&initVal)) {
        if (!app) {
            app = g_object_new (GWM_TYPE_APP, NULL);
            gsApp = g_object_ref(app);
        }
        g_once_init_leave(&initVal, 1);
    }

    return app;
}

void gwm_app_exit(GWMApp *app)
{
    g_return_if_fail(G_UNLIKELY (app));

}

bool gwm_app_initialize(GWMApp *app, GError **error)
{
    g_return_val_if_fail(G_UNLIKELY (app), false);

    // 检测是否支持本地化
    {
        if (!setlocale(LC_CTYPE, "") || !XSupportsLocale()) {
            GWM_SET_ERROR(error, GWM_ERROR_NOT_SUPPORTED_LOCALE);
            return false;
        }
    }

    // 初始化 display 连接
    {
        if (!(app->display = XOpenDisplay(NULL))) {
            GWM_SET_ERROR(error, GWM_ERROR_CANNOT_OPEN_DISPLAY);
            return false;
        }

        app->defaultScreen = DefaultScreen(app->display);

        if (BadWindow == (app->rootWindow = RootWindow(app->display, app->defaultScreen))) {
            GWM_SET_ERROR(error, GWM_ERROR_CANNOT_GET_ROOT_WINDOW);
            return false;
        }
    }

    // 获取 xcb 连接
    {
        if (!(app->xcbConn = XGetXCBConnection(app->display))) {
            GWM_SET_ERROR(error, GWM_ERROR_CANNOT_CONNECT_WITH_XCB);
            return false;
        }
    }

    // 检测是否有其它 窗口管理器 运行
    {
        gsDefaultXErrorHandler = XSetErrorHandler(x_error_handle_has_wm);
        XSelectInput(app->display, DefaultRootWindow(app->display), SubstructureRedirectMask);
        XSync(app->display, False);
        if (!app->isRunning) {
            GWM_SET_ERROR(error, GWM_ERROR_CANNOT_CONNECT_WITH_XCB);
            return false;
        }
        XSetErrorHandler (x_error_handler);
        XSync(app->display, False);
    }

    // Bar 栏显示时间
    {
        // FIXME:// 重新实现时间显示
        struct itimerval timer;

        struct sigaction timerSA;
        sigemptyset(&timerSA.sa_mask);

        timerSA.sa_flags = 0;
        timerSA.sa_handler = timer_handler;

        sigaction(SIGALRM, &timerSA, NULL);

        // 配置时间间隔
        timer.it_value.tv_sec = 1;
        timer.it_value.tv_usec = 0;
        timer.it_interval.tv_sec = 1;   // 每秒更新一次
        timer.it_interval.tv_usec = 0;
        setitimer (ITIMER_REAL, &timer, NULL);
    }

    // 忽略某些信号
    {
        struct sigaction sa;
        sigemptyset(&sa.sa_mask);

        sa.sa_flags = SA_NOCLDSTOP | SA_NOCLDWAIT | SA_RESTART;
        sa.sa_handler = SIG_IGN;
        sigaction(SIGCHLD, &sa, NULL);
    }

    // 等待所有子进程退出
    {
        while (waitpid(-1, NULL, WNOHANG) > 0);
    }

    // java
    {
        putenv("_JAVA_AWT_WM_NONREPARENTING=1");
    }

    return true;
}

Display *gwm_get_display(GWMApp *app)
{
    g_return_val_if_fail(G_UNLIKELY (app), NULL);

    return app->display;
}

xcb_connection_t *gwm_get_xcb_connection(GWMApp *app)
{
    g_return_val_if_fail(G_UNLIKELY (app), NULL);

    return app->xcbConn;
}


static void gwm_app_class_init (GWMAppClass* klass)
{
    GObjectClass* objClass = G_OBJECT_CLASS(klass);

    objClass->finalize = NULL;
    objClass->constructed = (void(*)(GObject*)) gwm_app_init;
    objClass->finalize = (void(*)(GObject*)) gwm_app_finalize;
}

static void gwm_app_init (GWMApp* obj)
{
    g_return_if_fail(G_UNLIKELY (obj));

    obj->display = NULL;
    obj->xcbConn = NULL;
    obj->rootWindow = BadWindow;

    obj->isRunning = true;
}

static void gwm_app_finalize (GWMApp* obj)
{
    g_return_if_fail(obj && GWM_IS_APP (obj));

    G_OBJECT_CLASS(gwm_app_parent_class)->finalize(obj);
}

int x_error_handle_has_wm(Display* dsp, XErrorEvent* ee)
{
    gsApp->isRunning = false;

    LOG_WARNING(APP_NAME": another window manager is already running");

    return 0;
}

int x_error_handler (Display *dpy, XErrorEvent *ee)
{
    if ((ee->error_code == BadWindow)
        || (ee->request_code == X_SetInputFocus && ee->error_code == BadMatch)
        || (ee->request_code == X_PolyText8 && ee->error_code == BadDrawable)
        || (ee->request_code == X_PolyFillRectangle && ee->error_code == BadDrawable)
        || (ee->request_code == X_PolySegment && ee->error_code == BadDrawable)
        || (ee->request_code == X_ConfigureWindow && ee->error_code == BadMatch)
        || (ee->request_code == X_GrabButton && ee->error_code == BadAccess)
        || (ee->request_code == X_GrabKey && ee->error_code == BadAccess)
        || (ee->request_code == X_CopyArea && ee->error_code == BadDrawable)) {
        return 0;
    }

    LOG_WARNING("request code=%d, error code=%d", ee->request_code, ee->error_code);

    return gsDefaultXErrorHandler(dpy, ee); /* may call exit */
}

static void timer_handler(int signum)
{
    Display* dpy = gsApp->display;
    Window root = gsApp->rootWindow;

    Atom utf8string = XInternAtom(dpy, "UTF8_STRING", False);

    time_t currentTime;
    time (&currentTime);

    struct tm* localTime = localtime (&currentTime);
    if (localTime) {
        char buf[32] = {0};
        snprintf (buf, sizeof(buf) - 1, "%4d-%02d-%02d %02d:%02d:%02d",
                  localTime->tm_year + 1900,
                  localTime->tm_mon + 1,
                  localTime->tm_mday,
                  localTime->tm_hour,
                  localTime->tm_min,
                  localTime->tm_sec
        );

        XChangeProperty(dpy, root, gsCustomStatusAtoms[CUSTOM_STATUS_TIME], utf8string, 8, PropModeReplace, (unsigned char*) (buf), (int) strlen (buf));

        {
            XEvent ev;
            ev.type = ClientMessage;
            ev.xclient.window = root;
            ev.xclient.message_type = gsCustomStatusAtoms[CUSTOM_STATUS_TIME];
            ev.xclient.format = 8;
            ev.xclient.data.l[0] = 0;
            ev.xclient.data.l[1] = 0;
            ev.xclient.data.l[2] = 0;
            ev.xclient.data.l[3] = 0;
            ev.xclient.data.l[4] = 0;
            XSendEvent (dpy, root, False, StructureNotifyMask, &ev);
            XFlush (dpy);
        }
    }
}
