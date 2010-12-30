#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <complex.h>
#include <string.h>
#include <sys/time.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stdint.h>

#include <glib.h>
#include <glib-object.h>
#include "risc.h"
#include "ship.h"
#include "util.h"

FILE *trace_file = NULL;

static guint64 thread_ns(void)
{
#ifdef CLOCK_THREAD_CPUTIME_ID
	struct timespec ts;
	if (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts)) {
		perror("glock_gettime");
		abort();
	}
	return ts.tv_nsec + ts.tv_sec*(1000*1000*1000);
#else
	return 0;
#endif
}

void debug_hook(lua_State *L, lua_Debug *a)
{
	if (a->event == LUA_HOOKCOUNT) {
		lua_getglobal(L, "debug_count_hook");
		lua_call(L, 0, 0);
		lua_yield(L, 0);
	} else if (a->event == LUA_HOOKLINE) {
		RISCShip *s = risc_ship_lua_ship(L);
		unsigned long elapsed = thread_ns() - s->line_start_time;
		if (lua_getinfo(L, "nSl", a) == 0) abort();
		if (s->line_start_time != 0) {
			fprintf(trace_file, "%ld\t%u\t%s\n", elapsed, s->api_id, s->line_info);
		}
		snprintf(s->line_info, sizeof(s->line_info), "%s\t%s:%d",
				     a->name, a->short_src, a->currentline);
		s->line_start_time = thread_ns();
	}
}
