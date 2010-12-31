#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <complex.h>
#include <string.h>
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
