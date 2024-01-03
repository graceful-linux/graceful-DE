
static bool gsUseArgb = false;
static Visual* visual = NULL;
static int depth;
static Colormap cmap;

void xinit_visual()
{
    int nItems;
    XVisualInfo *infos;
    XRenderPictFormat *fmt;

    XVisualInfo tpl = {
        .screen = screen,
        .depth = 32,
        .class = TrueColor
    };
    long masks = VisualScreenMask | VisualDepthMask | VisualClassMask;

    infos = XGetVisualInfo(dpy, masks, &tpl, &nItems);
    if (G_UNLIKELY(!infos)) {
        LOG_ERROR("XGetVisualInfo error!");
        return;
    }

    visual = NULL;
    for (int i = 0; i < nItems; i ++) {
        fmt = XRenderFindVisualFormat(dpy, infos[i].visual);
        if (fmt->type == PictTypeDirect && fmt->direct.alphaMask) {
            visual = infos[i].visual;
            depth = infos[i].depth;
            cmap = XCreateColormap(dpy, root, visual, AllocNone);
            gsUseArgb = true;
            break;
        }
    }

    XFree(infos);

    if (!visual) {
        visual = DefaultVisual(dpy, screen);
        depth = DefaultDepth(dpy, screen);
        cmap = DefaultColormap(dpy, screen);
    }
}

