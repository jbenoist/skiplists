#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "quad.h"

void quad_init(struct quad *q)
{
	q->next = q->prev = q->up = q->down = q;
}

struct quad *quad_new(void *data)
{
	struct quad *q = malloc(sizeof(*q));

	if (q) {
		q->data = data;
		quad_init(q);
	}
	return q;
}

void quad_h_del(struct quad *q)
{
	q->prev->next = q->next;
	q->next->prev = q->prev;
}

void quad_v_del(struct quad *q)
{
	q->down->up = q->up;
	q->up->down= q->down;
}

void quad_h_add(struct quad *q, struct quad *n)
{
	n->next = q->next;
	n->next->prev = n;

	q->next = n;
	n->prev = q;
}

void quad_v_add(struct quad *q, struct quad *n)
{
	n->up = q->up;
	n->up->down = n;

	q->up = n;
	n->down = q;
}

void quad_foreach(struct quad *q, void (*visit)(struct quad *, struct quad *))
{
	struct quad *c = q->down;
	do {
		visit(c, NULL);
		for (struct quad *x = c->next; x != c; x = x->next) {
			visit(c, x);
		}
		visit(NULL, c);
		c = c->down;
	} while (c != q->down);
}
