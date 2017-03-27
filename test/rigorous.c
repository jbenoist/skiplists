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

void *int_dup(void *i) {
	int *p = malloc(sizeof(*p));
	memcpy(p, i, sizeof(int));
	return p;
}

char *int_str(void *i) {
	char *ptr;
	asprintf(&ptr, "%d", *(int *)i);
	return ptr;
}

#define COUNT 2000000

static void array_shuffle(int *array, int array_size)
{
	for (int i = array_size - 1; i >= 1; i--) {
		int tmp;
		int j = random() % i;
		tmp = array[j];
		array[j] = array[i];
		array[i] = tmp;
	}
}

void test(bool copy, bool alldeletes)
{
	int *v;
	int *track;
	int *track_copy;
	bool *seen;
	skiplist_t *s;
	skipitem_t *l;

	srandom(0);

	if (copy) {
		s = skiplist_create(int_cmp, int_dup, free);
	} else {
		s = skiplist_create(int_cmp, NULL, NULL);
	}

	printf("-- copies=%s delete-one-by-one=%s --\n", copy ? "yes" : "no", alldeletes ? "yes" : "no");

	track = calloc(COUNT, sizeof(int));
	seen = calloc(COUNT, sizeof(bool));

	printf("adding %d elements\n", COUNT);
	for (int i = 0; i < COUNT; i++ ) {
		track[i] = (int)random();
		skiplist_add(s, &track[i]);
	}
	skiplist_stats(s, int_str);
	puts("Checking consistency");
	skiplist_consistency(s);

	v = skiplist_find(s, (int *)&(int){2100429183}, &l);
	assert(v && *v == 2100429183);
	if (v) {
		puts("Removing one element found ont top");
		skiplist_del(l);
		if (copy) {
			free(v);
		}
	}
	puts("Checking consistency");
	skiplist_consistency(s);
	puts("Re-adding element");
	skiplist_add(s, &(int){2100429183});

	puts("Shuffling array");

	if (copy) {
		track_copy = track;
	} else {
		track_copy = malloc(COUNT * sizeof(*track_copy));
		memcpy(track_copy, track, sizeof(int) * COUNT);
	}
	array_shuffle(track_copy, COUNT);

	if (alldeletes) {
		puts("Start deleting en masse");
		for (int i = 0; i < COUNT; i++) {
			if (seen[i] == false) {
				v = skiplist_find(s, &track_copy[i], &l);
				assert(v && *v == track_copy[i]);
				if (copy) {
					free(v);
				}
				skiplist_del(l);
				seen[i] = true;
			}
		}
	}

	puts("Checking consistency");
	skiplist_consistency(s);
	puts("Destroy\n");
	skiplist_destroy(s);

	if (track_copy != track) {
		free(track_copy);
	}
	free(track);
	free(seen);

}

int main(int argc, char **argv)
{
	test(true, true);
	test(false, true);
	test(true, false);
	test(false, false);
}
