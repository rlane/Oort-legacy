#ifndef TEAM_H
#define TEAM_H

struct team {
	int color;
	char *name;
	int ships;
};

struct team *team_create(const char *name, int color);
void team_destroy(struct team *t);
void team_shutdown(void);
struct team *team_lookup(const char *name);

extern GList *all_teams;

#endif
