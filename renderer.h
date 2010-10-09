#ifndef RENDERER_H
#define RENDERER_H

extern int screen_width;
extern int screen_height;
extern double view_scale;
extern int paused;
extern int single_step;
extern int render_all_debug_lines;
extern struct ship *picked;
extern int simple_graphics;

void init_gl13(void);
void render_gl13(void);
void reshape_gl13(int width, int height);
void zoom(int x, int y, double f);
struct ship *pick(int x, int y);

#endif
