/* See LICENSE file for copyright and license details. */
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

#include "drw.h"
#include "log.h"
#include "util.h"
#include "patches.h"

#if BIDI_PATCH
#include <fribidi.h>

static char fribidi_text[BUFSIZ] = "";

static void
apply_fribidi(const char *str)
{
    FriBidiStrIndex len = strlen(str);
    FriBidiChar logical[BUFSIZ];
    FriBidiChar visual[BUFSIZ];
    FriBidiParType base = FRIBIDI_PAR_ON;
    FriBidiCharSet charset;

    fribidi_text[0] = 0;
    if (len > 0) {
        charset = fribidi_parse_charset("UTF-8");
        len = fribidi_charset_to_unicode(charset, str, len, logical);
        fribidi_log2vis(logical, len, &base, visual, NULL, NULL, NULL);
        len = fribidi_unicode_to_charset(charset, visual, len, fribidi_text);
    }
}
#endif

#if BAR_POWERLINE_TAGS_PATCH || BAR_POWERLINE_STATUS_PATCH
Clr transcheme[3];
#endif // BAR_POWERLINE_TAGS_PATCH | BAR_POWERLINE_STATUS_PATCH

Drw* drw_create(Display *dpy, int screen, Window root, unsigned int w, unsigned int h, Visual *visual, unsigned int depth, Colormap cmap)
{
    Drw *drw = ecalloc(1, sizeof(Drw));

    drw->dpy = dpy;
    drw->screen = screen;
    drw->root = root;
    drw->w = w;
    drw->h = h;

    drw->visual = visual;
    drw->depth = depth;
    drw->cmap = cmap;
    drw->drawable = XCreatePixmap(dpy, root, w, h, depth);                                                              // 创建可绘制界面
    drw->picture = XRenderCreatePicture(dpy, drw->drawable, XRenderFindVisualFormat(dpy, visual), 0, NULL);             // 创建一个用于渲染的Picture对象，用于高级渲染操作，如：图像合成、阴影、透明度、颜色混合等
                                                                                                                        // XRender 是 X11 扩展模块，提供了一种高级的图形渲染功能
    drw->gc = XCreateGC(dpy, drw->drawable, 0, NULL);

    XSetLineAttributes(dpy, drw->gc, 1, LineSolid, CapRound, JoinRound);

    return drw;                                                                                                         // drw：整个 根窗口 绘制相关
}

void drw_resize(Drw *drw, unsigned int w, unsigned int h)
{
    if (!drw)
        return;

    drw->w = w;
    drw->h = h;
    if (drw->picture) {
        XRenderFreePicture(drw->dpy, drw->picture);
    }
    if (drw->drawable)
        XFreePixmap(drw->dpy, drw->drawable);
    drw->drawable = XCreatePixmap(drw->dpy, drw->root, w, h, drw->depth);
    drw->picture = XRenderCreatePicture(drw->dpy, drw->drawable, XRenderFindVisualFormat(drw->dpy, drw->visual), 0, NULL);
}

void drw_free(Drw *drw)
{
    XRenderFreePicture(drw->dpy, drw->picture);
    XFreePixmap(drw->dpy, drw->drawable);
    XFreeGC(drw->dpy, drw->gc);
    drw_fontset_free(drw->fonts);
    free(drw);
}

/* This function is an implementation detail. Library users should use
 * drw_font_create instead.
 */
static Fnt* xfont_create(Drw *drw, const char *fontName)
{
    Fnt* font = ecalloc(1, sizeof(Fnt));
    font->dpy = drw->dpy;

    PangoFontMap* fontMap = pango_xft_get_font_map(drw->dpy, drw->screen);
    PangoContext* context = pango_font_map_create_context(fontMap);
    PangoFontDescription* desc = pango_font_description_from_string(fontName);
    font->layout = pango_layout_new(context);
    pango_layout_set_font_description(font->layout, desc);

    PangoFontMetrics* metrics = pango_context_get_metrics(context, desc, pango_language_from_string ("en-us"));
    font->h = pango_font_metrics_get_height(metrics) / PANGO_SCALE;

    pango_font_metrics_unref(metrics);
    g_object_unref(context);

    return font;
}

static void xfont_free(Fnt *font)
{
    if (font->layout) {
        g_object_unref(font->layout);
    }

    free(font);
}

Fnt* drw_font_create(Drw* drw, const char font[])
{
    Fnt *fnt = NULL;

    if (!drw || !font) {
        LOG_WARNING("drw_font_create error: (!draw || !font");
        return NULL;
    }

    fnt = xfont_create(drw, font);

    return (drw->fonts = fnt);
}

void drw_fontset_free(Fnt *font)
{
    if (font) {
        xfont_free(font);
    }
}

void drw_clr_create(Drw *drw, Clr *dest, const char *clrname, unsigned int alpha)
{
    if (!drw || !dest || !clrname)
        return;

    if (!XftColorAllocName(drw->dpy, drw->visual, drw->cmap, clrname, dest)) {
        LOG_WARNING("warning, cannot allocate color '%s'", clrname);
    }

    dest->pixel = (dest->pixel & 0x00ffffffU) | (alpha << 24);
}

/* Wrapper to create color schemes. The caller has to call free(3) on the
 * returned color scheme when done using it. */
Clr* drw_scm_create(Drw *drw, char *clrnames[], const unsigned int alphas[], size_t clrcount)
{
    size_t i;
    Clr *ret;

    if (!drw || !clrnames || clrcount < 2 || !(ret = ecalloc(clrcount, sizeof(XftColor)))) {
        return NULL;
    }

    for (i = 0; i < clrcount; i++) {
        drw_clr_create(drw, &ret[i], clrnames[i], alphas[i]);
    }

    return ret;
}


void drw_setscheme(Drw *drw, Clr *scm)
{
    if (drw) {
        drw->scheme = scm;
    }
}

#if BAR_POWERLINE_TAGS_PATCH || BAR_POWERLINE_STATUS_PATCH
void
drw_settrans(Drw *drw, Clr *psc, Clr *nsc)
{
    if (drw) {
        transcheme[0] = psc[ColBg]; transcheme[1] = nsc[ColBg]; transcheme[2] = psc[ColBorder];
        drw->scheme = transcheme;
    }
}
#endif // BAR_POWERLINE_TAGS_PATCH | BAR_POWERLINE_STATUS_PATCH

void drw_rect(Drw *drw, int x, int y, unsigned int w, unsigned int h, int filled, int invert)
{
    if (!drw || !drw->scheme) {
        return;
    }

    XSetForeground(drw->dpy, drw->gc, invert ? drw->scheme[ColBg].pixel : drw->scheme[ColFg].pixel);
    if (filled) {
        XFillRectangle(drw->dpy, drw->drawable, drw->gc, x, y, w, h);
    }
    else {
        XDrawRectangle(drw->dpy, drw->drawable, drw->gc, x, y, w - 1, h - 1);
    }
}

#if BIDI_PATCH
int
_drw_text(Drw *drw, int x, int y, unsigned int w, unsigned int h, unsigned int lpad, const char *text, int invert, Bool markup)
#else
int drw_text(Drw *drw, int x, int y, unsigned int w, unsigned int h, unsigned int lpad, const char *text, int invert, Bool markup)
#endif // BIDI_PATCH
{
    char buf[1024];
    int ty;
    unsigned int ew;
    XftDraw *d = NULL;
    size_t i, len;
    int render = x || y || w || h;

    if (!drw || (render && !drw->scheme) || !text || !drw->fonts)
        return 0;

    if (!render) {
        w = ~w;
    }
    else {
        XSetForeground(drw->dpy, drw->gc, drw->scheme[invert ? ColFg : ColBg].pixel);
        XFillRectangle(drw->dpy, drw->drawable, drw->gc, x, y, w, h);
        d = XftDrawCreate(drw->dpy, drw->drawable, drw->visual, drw->cmap);

        x += lpad;
        w -= lpad;
    }

    len = strlen(text);

    if (len) {
        drw_font_getexts(drw->fonts, text, len, &ew, NULL, markup);
        /* shorten text if necessary */
        for (len = MIN(len, sizeof(buf) - 1); len && ew > w; len--)
            drw_font_getexts(drw->fonts, text, len, &ew, NULL, markup);

        if (len) {
            memcpy(buf, text, len);
            buf[len] = '\0';
            if (len < strlen(text))
                for (i = len; i && i > len - 3; buf[--i] = '.')
                    ; /* NOP */

            if (render) {
                ty = y + (h - drw->fonts->h) / 2;
                if (markup)
                    pango_layout_set_markup(drw->fonts->layout, buf, len);
                else
                    pango_layout_set_text(drw->fonts->layout, buf, len);
                pango_xft_render_layout(d, &drw->scheme[invert ? ColBg : ColFg],
                    drw->fonts->layout, x * PANGO_SCALE, ty * PANGO_SCALE);
                if (markup) /* clear markup attributes */
                    pango_layout_set_attributes(drw->fonts->layout, NULL);
            }
            x += ew;
            w -= ew;
        }
    }
    if (d)
        XftDrawDestroy(d);

    return x + (render ? w : 0);
}

#if BIDI_PATCH
int
drw_text(Drw *drw, int x, int y, unsigned int w, unsigned int h, unsigned int lpad, const char *text, int invert, Bool markup)
{
    apply_fribidi(text);
    return _drw_text(drw, x, y, w, h, lpad, fribidi_text, invert, markup);
}
#endif // BIDI_PATCH

#if BAR_POWERLINE_TAGS_PATCH || BAR_POWERLINE_STATUS_PATCH
void
drw_arrow(Drw *drw, int x, int y, unsigned int w, unsigned int h, int direction, int slash)
{
    if (!drw || !drw->scheme)
        return;

    /* direction=1 draws right arrow */
    x = direction ? x : x + w;
    w = direction ? w : -w;
    /* slash=1 draws slash instead of arrow */
    unsigned int hh = slash ? (direction ? 0 : h) : h/2;

    XPoint points[] = {
        {x    , y      },
        {x + w, y + hh },
        {x    , y + h  },
    };

    XPoint bg[] = {
        {x    , y    },
        {x + w, y    },
        {x + w, y + h},
        {x    , y + h},
    };

    XSetForeground(drw->dpy, drw->gc, drw->scheme[ColBg].pixel);
    XFillPolygon(drw->dpy, drw->drawable, drw->gc, bg, 4, Convex, CoordModeOrigin);
    XSetForeground(drw->dpy, drw->gc, drw->scheme[ColFg].pixel);
    XFillPolygon(drw->dpy, drw->drawable, drw->gc, points, 3, Nonconvex, CoordModeOrigin);
}
#endif // BAR_POWERLINE_TAGS_PATCH | BAR_POWERLINE_STATUS_PATCH

void drw_map(Drw *drw, Window win, int x, int y, unsigned int w, unsigned int h)
{
    if (!drw) {
        return;
    }

    XCopyArea(drw->dpy, drw->drawable, win, drw->gc, x, y, w, h, x, y);
    XSync(drw->dpy, False);
}

unsigned int
drw_fontset_getwidth(Drw *drw, const char *text, Bool markup)
{
    if (!drw || !drw->fonts || !text)
        return 0;
    return drw_text(drw, 0, 0, 0, 0, 0, text, 0, markup);
}

void drw_font_getexts(Fnt *font, const char *text, unsigned int len, unsigned int *w, unsigned int *h, Bool markup)
{
    if (!font || !text)
        return;

    PangoRectangle r;
    if (markup)
        pango_layout_set_markup(font->layout, text, len);
    else
        pango_layout_set_text(font->layout, text, len);
    pango_layout_get_extents(font->layout, 0, &r);
    if (markup) /* clear markup attributes */
        pango_layout_set_attributes(font->layout, NULL);
    if (w)
        *w = r.width / PANGO_SCALE;
    if (h)
        *h = font->h;
}

Cur* drw_cur_create(Drw *drw, int shape)
{
    Cur *cur;

    if (!drw || !(cur = ecalloc(1, sizeof(Cur))))
        return NULL;

    cur->cursor = XCreateFontCursor(drw->dpy, shape);

    return cur;
}

void
drw_cur_free(Drw *drw, Cur *cursor)
{
    if (!cursor)
        return;

    XFreeCursor(drw->dpy, cursor->cursor);
    free(cursor);
}

