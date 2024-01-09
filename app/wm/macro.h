//
// Created by dingjing on 1/9/24.
//

#ifndef GRACEFUL_DE_MACRO_H
#define GRACEFUL_DE_MACRO_H
#include <glib.h>

#include "graceful-error.h"


#define GWM_ERROR_QUARK                                 (g_quark_from_static_string("gwm-error-" APP_NAME))


#define GWM_CLEAR_ERROR(error) \
{ \
    if (error) { \
        if (*error) { \
            g_error_free (*error); \
            *error = NULL; \
        }\
    } \
}

#define GWM_SET_ERROR(error, code) \
{ \
    GWM_CLEAR_ERROR(error); \
    if (error) { \
        if (G_LIKELY((code >= GWM_ERROR_MIN) && (code < GWM_ERROR_MAX))) { \
            *error = g_error_new (GWM_ERROR_QUARK, gError[code].errorCode, gError[code].errorStr); \
        } \
    } \
}
#endif //GRACEFUL_DE_MACRO_H
