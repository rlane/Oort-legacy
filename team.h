#ifndef TEAM_H
#define TEAM_H

struct team {
	int color;
	char *name;
	char *filename;
	int ships;
};

struct team *team_create(const char *name, const char *filename, int color);
void team_destroy(struct team *t);
void team_shutdown(void);
struct team *team_lookup(const char *name);

extern GList *all_teams;

#endif
