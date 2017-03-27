#ifndef _JB_QUAD_H
#define _JB_QUAD_H

struct quad {
	void *data;
	struct quad *up;
	struct quad *down;
	struct quad *next;
	struct quad *prev;
};

typedef void (*quad_visit)(struct quad *head, struct quad *elem);

struct quad *quad_new(void *);
void quad_init(struct quad *);
void quad_h_del(struct quad *);
void quad_v_del(struct quad *);
void quad_h_add(struct quad *, struct quad *);
void quad_v_add(struct quad *, struct quad *);
void quad_foreach(struct quad *, quad_visit);

#endif
