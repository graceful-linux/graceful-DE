Client* getpointerclient(void)
{
    Window dummy, win;
    int di;
    unsigned int dui;
    XQueryPointer(dpy, root, &dummy, &win, &di, &di, &di, &di, &dui);

    return win_to_client(win);
}
