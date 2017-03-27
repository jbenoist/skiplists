#ifndef _JB_SKIPLIST_H
#define _JB_SKIPLIST_H

typedef struct quad skipitem_t;
typedef struct skiplist skiplist_t;

typedef void *(*skip_dup)(void *);
typedef void (*skip_free)(void *);
typedef char *(skip_str)(void *);
typedef int (*skip_cmp)(void *, void *);

typedef enum { SKIPITEM_PREV, SKIPITEM_CUR, SKIPITEM_NEXT } skipitem_pos_t ;

skiplist_t *skiplist_create(skip_cmp, skip_dup, skip_free);
skipitem_t *skiplist_add(skiplist_t *s, void *data);

void skiplist_del(skipitem_t *item);
void skiplist_destroy(skiplist_t *s);

void skiplist_consistency(skiplist_t *s);
void skiplist_stats(skiplist_t *s, skip_str);

void *skiplist_next(skiplist_t *s, skipitem_t **item);
void *skiplist_item_data(struct quad *q, skipitem_pos_t pos);
void *skiplist_find(skiplist_t *s, void *data, skipitem_t **item);

#endif
