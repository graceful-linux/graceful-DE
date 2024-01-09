//
// Created by dingjing on 1/9/24.
//

#ifndef GRACEFUL_DE_GRACEFUL_ERROR_H
#define GRACEFUL_DE_GRACEFUL_ERROR_H
#include <glib/gi18n.h>

typedef struct GWMError             GWMError;
typedef enum GWMErrorCode           GWMErrorCode;

enum GWMErrorCode
{
    GWM_ERROR_MIN,
    GWM_ERROR_NOT_SUPPORTED_LOCALE,
    GWM_ERROR_CANNOT_OPEN_DISPLAY,
    GWM_ERROR_CANNOT_CONNECT_WITH_XCB,
    GWM_ERROR_WM_ALREADY_EXISTS,
    GWM_ERROR_CANNOT_GET_ROOT_WINDOW,
    GWM_ERROR_MAX,
};

struct GWMError
{
    GWMErrorCode    errorCode;
    const char*     errorStr;
};

static GWMError gError[] = {
    {GWM_ERROR_MIN, ""},
    {GWM_ERROR_NOT_SUPPORTED_LOCALE,        N_("No locale supported.")},
    {GWM_ERROR_CANNOT_OPEN_DISPLAY,         N_("Cannot connect to X server.")},
    {GWM_ERROR_CANNOT_CONNECT_WITH_XCB,     N_("Cannot connect to X server with xcb.")},
    {GWM_ERROR_WM_ALREADY_EXISTS,           N_("Another window manager already running.")},
    {GWM_ERROR_CANNOT_GET_ROOT_WINDOW,      N_("Cannot get root window.")},
    {GWM_ERROR_MAX, ""},
};


#endif //GRACEFUL_DE_GRACEFUL_ERROR_H
