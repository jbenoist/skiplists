#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "skiplist.h"

char *input[] = {
	"whinnied", "catouse", "pyke", "lbinit", "repined", "nonprecipitative", "akan",
	"poldavis", "hyperaccurateness", "superstitions", "robot", "unkindledness",
	"unexplorative", "entomostracan", "broils", "prejudges", "pavid", "properest",
	"reevidence", "physiopathology", "destrer", "asperated", "acceptilating",
	"seilenos", "faradized", "unreviled", "halfen", "farcialize", "gegenion",
	"conterminant", "asport", "sedarim", "liberals", "monopteroi",
};

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

int main(int argc, char **argv)
{
	char *word;
	skiplist_t *s;
	skipitem_t *it;

	/*
	 * Only the 'skip_cmp' callback is required,
	 *   - skip_dup duplicates the input (once, not for all levels)
	 *   - skip_free frees the data when the list is destroyed
	 */
	s = skiplist_create((skip_cmp)strcoll, (skip_dup)strdup, free);

	for (int i = 0; i < ARRAY_SIZE(input); i++) {
		/* 'add' returns a skipitem_t pointer */
		skiplist_add(s, input[i]);
	}

	/*
	 * For deletion, a skipitem_t is pointer is requied
	 * it can be searched for, or might have been saved when
	 * the item was added
	 */
	if ((word = skiplist_find(s, "broils", &it))) {
		skiplist_del(it);
		free(word);		/* on 'del' the caller frees */
	}

	/* In order traversal, it needs to be NULL  */
	it = NULL;
	while ((word = (char *)skiplist_next(s, &it))) {
		printf("%s ", word);
	}
	puts("");

	/* Nearest match */
	word = (char *)skiplist_find(s, "inexistent", &it);
	if (word == NULL) {
		/* it points to the predecessor */
		word = skiplist_item_data(it, SKIPITEM_CUR);
		printf("%s < inexistent < ", word);
		word = (char *)skiplist_item_data(it, SKIPITEM_NEXT);
		printf("%s\n", word);
	}

	/*
	 * Poor man's range search:
	 *  looking for entries [d:n] therefor [d:p)
	 */
	skipitem_t *it_end;
	skiplist_find(s, "d", &it);
	skiplist_find(s, "p", &it_end);

	/*
	 * it and it_end point to d and p's nearest
	 * predecessors since no such entry exists
	 */
	skiplist_next(s, &it);
	skiplist_next(s, &it_end);

	do {
		word = skiplist_next(s, &it);
		printf("%s\n", word);
	} while (it != it_end);

	/* free everything */
	skiplist_destroy(s);

}
