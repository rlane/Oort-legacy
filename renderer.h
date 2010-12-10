#include <vector.h>

#ifndef RENDERER_H
#define RENDERER_H

extern int paused;
extern int single_step;
extern double view_scale;
extern struct ship *picked;
extern int render_all_debug_lines;

struct gfx_class {
	double rotfactor;
};

void init_gl13(void);
void reset_gl13(void);
void render_gl13(int paused, int render_all_debug_lines);
void reshape_gl13(int width, int height);
void zoom(int x, int y, double f);
void pick(int x, int y);
Vec2 S(Vec2 p);

#endif
