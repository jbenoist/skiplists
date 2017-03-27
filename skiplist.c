#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include <assert.h>

#include "quad.h"
#include "skiplist.h"

struct skiplist {
	struct quad *Q;
	void *(*dup)(void *);
	void (*free)(void *);
	int (*compare)(void *, void *);
};

#define GHETTO_STACKMAX 64
#define GHETTO_COMPARE(cmp, l, r)  (l == NULL ? -1 : cmp(l, r))

struct skiplist *skiplist_create(int (*compare)(void *, void*), void *(dup)(void *), void (*free)(void *))
{
	struct skiplist *s = calloc(1, sizeof(*s));

	if (s) {
		s->Q = quad_new(NULL);
		s->dup = dup;
		s->free = free;
		s->compare = compare;
	}
	return s;
}

static struct quad *find_successor(struct quad *head, struct quad *pos, int (*compare)(void *, void *), void *data)
{
	while (pos->next != head && GHETTO_COMPARE(compare, pos->next->data, data) < 0) {
		pos = pos->next;
	}
	return pos;
}

void *skiplist_find(struct skiplist *s, void *data, struct quad **item)
{
	void *data_found = NULL;
	struct quad *successor;
	struct quad *cur = s->Q->down;
	struct quad *level = s->Q->down;

	do {
		successor = find_successor(level, cur, s->compare, data);
		level = level->down;
		cur = successor->down;
	} while (level != s->Q->down);

	if (!GHETTO_COMPARE(s->compare, successor->next->data, data)) {
		successor = successor->next;
		data_found = successor->data;
	}

	if (item) {
		*item = successor;
	}

	return data_found;
}

struct quad *skiplist_add(struct skiplist *s, void *data)
{
	bool cont = true;
	int ghetto_stack_idx = -1;
	struct quad *level = s->Q->down;
	struct quad *successor = s->Q->down;
	struct quad *ghetto_stack[GHETTO_STACKMAX];
	struct quad *elem = s->dup ? quad_new(s->dup(data)) : quad_new(data);
	struct quad *elem_bottom = elem;

	do {
		successor = find_successor(level, successor, s->compare, data);
		ghetto_stack[++ghetto_stack_idx] = successor;
		level = level->down;
		successor = successor->down;
	} while (level != s->Q->down);

	successor = ghetto_stack[ghetto_stack_idx--];
	quad_h_add(successor, elem);

	for (level = s->Q->up ; cont && random() & 1; level = level->up) {

		struct quad *up_elem = quad_new(elem->data);

		if (ghetto_stack_idx != -1) {
			successor = ghetto_stack[ghetto_stack_idx--];
		} else {
			cont = false;
			quad_v_add(level->down, quad_new(NULL));
			level = successor = level->down;
		}
		quad_h_add(successor, up_elem);
		quad_v_add(elem, up_elem);
		elem = up_elem;
	}
	return elem_bottom;
}

void skiplist_del(struct quad *q)
{
	struct quad *cur = q;
	struct quad *root = q->prev;

	do {
		struct quad *up = cur->up;
		if (cur->next == cur->prev) {
			struct quad *zero = cur->next;
			if (zero == root) {
				quad_init(root);
			} else {
				quad_v_del(zero);
				quad_h_del(zero);
				free(zero);
			}
		}
		quad_h_del(cur);
		free(cur);
		cur = up;
	} while (cur != q);
}

void skiplist_destroy(struct skiplist *s)
{
	struct quad *pos = s->Q->down;
	struct quad *level = s->Q->down;

	do {
		struct quad *cur = level;
		struct quad *down = level->down;

		do {
			struct quad *next = cur->next;
			if (s->free && cur->data && level == s->Q) {
				s->free(cur->data);
			}
			free(cur);
			cur = next;
		} while (cur != level);

		level = down;

	} while (level != pos);

	free(s);
}

void skiplist_consistency(struct skiplist *s) {
	for (struct quad *cur = s->Q->next; cur != s->Q; cur = cur->next) {
		assert(GHETTO_COMPARE(s->compare, cur->prev->data, cur->data) <= 0);
		struct quad *tmp = cur;
		do {
			assert(tmp->data == cur->data);
			tmp = tmp->up;
		} while (tmp != cur);
	}
}

void skiplist_stats(struct skiplist *s, char *(*datastr)(void *))
{
	int count;
	int height = 0;
	char *first = NULL;
	char *last = NULL;
	void _do_show(struct quad *level, struct quad *elem) {
		if (level && elem) {
			count++;
		} else if (level && !elem) {
			count = 0;
			if (level->prev->data && level->next->data) {
				last = datastr(level->prev->data);
				first = datastr(level->next->data);
			}
		} else {
			if (count == 0) {
				printf("#%d: no entries\n", height++);
			} else {
				printf("#%d: %d entries [%s:%s]\n", height++, count, first, last);
				free(first); free(last); first = NULL; last = NULL;
			}
		}
	}
	quad_foreach(s->Q, _do_show);
}

void *skiplist_next(skiplist_t *s, struct quad **it)
{
	struct quad *cur = *it ? *it : s->Q->next;
	*it = cur->next;
	return cur->data;
}

void *skiplist_item_data(struct quad *q, skipitem_pos_t pos)
{
	void *data = NULL;
	switch (pos) {
	case SKIPITEM_PREV:
		data = q->prev->data;
		break;
	case SKIPITEM_CUR:
		data = q->data;
		break;
	case SKIPITEM_NEXT:
		data = q->next->data;
		break;
	}
	return data;
}
