#ifndef RENDERER_H
#define RENDERER_H

extern SDL_Surface *screen;
extern int screen_width;
extern int screen_height;
extern complex double view_pos;
extern double view_scale;
extern int paused;
extern int single_step;
extern int render_all_debug_lines;
extern struct ship *picked;
extern int simple_graphics;

void render_gl13(void);

#endif
