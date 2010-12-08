#ifndef RENDERER_H
#define RENDERER_H

extern int paused;
extern int single_step;

struct gfx_class {
	double rotfactor;
};

void init_gl13(void);
void reset_gl13(void);
void render_gl13(int paused, int render_all_debug_lines);
void emit_particles(void);
void reshape_gl13(int width, int height);
void zoom(int x, int y, double f);
void pick(int x, int y);

#endif
