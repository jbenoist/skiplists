#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "skiplist.h"

int int_cmp(void *l, void *r) {
	return *((int *)l) - *((int *)r);
}

char *int_str(void *i) {
	char *ptr;
	asprintf(&ptr, "%d", *(int *)i);
	return ptr;
}

#define COUNT 10000000

int main(int argc, char **argv)
{
	int *data = malloc(COUNT * sizeof(*data));
	skiplist_t *s = skiplist_create(int_cmp, NULL, NULL);

	for (int i = 0; i < COUNT; i++) {
		*(data + i) = i;
		skiplist_add(s, data + i);
	}
	skiplist_stats(s, int_str);
	skiplist_find(s, &data[1], NULL);
	free(data);
	skiplist_destroy(s);
}
