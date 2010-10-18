#ifndef GLUTIL_H
#define GLUTIL_H

void glWrite(int x, int y, const char *str);
void glPrintf(int x, int y, const char *fmt, ...);
void glColor32(guint32 c);
void screenshot(const char *filename);
void font_init(void);

#endif
