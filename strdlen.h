#ifndef __STRDLEN__
long unsigned strdlen(char *a) {
        char *s;
        for (s = a; *s != '\n' && *s != '\0'; s++);
        return s - a;
}
#endif
