#include <stdlib.h>
#include <stdio.h>
#include <complex.h>
#include <sys/time.h>
#include <string.h>
#include <glib.h>

#include "team.h"

GList *all_teams = NULL;

struct team *team_create(const char *name, int color)
{
	struct team *t = g_slice_new(struct team);
	t->name = strdup(name);
	t->color = color;
	t->ships = 0;
	all_teams = g_list_append(all_teams, t);
	return t;
}

void team_destroy(struct team *t)
{
	all_teams = g_list_remove(all_teams, t);
	free(t->name);
	g_slice_free(struct team, t);
}

static int name_predicate(struct team *t, const char *name)
{
	return strcmp(name, t->name);
}

struct team *team_lookup(const char *name)
{
	GList *e = g_list_find_custom(all_teams, name, (GCompareFunc)name_predicate);
	return e ? e->data : NULL;
}
