#include <vector.h>

#ifndef RENDERER_H
#define RENDERER_H

extern struct gfx_class *gfx_fighter_p;
extern struct gfx_class *gfx_mothership_p;
extern struct gfx_class *gfx_missile_p;
extern struct gfx_class *gfx_little_missile_p;
extern struct gfx_class *gfx_unknown_p;

struct gfx_class {
	double rotfactor;
};

void init_gl13(void);

#endif
