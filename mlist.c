#include <stdio.h>
#include <stdlib.h>
#include "mlist.h"

#define HASHSIZE 20;
#define CHAINSIZE 20;

typedef struct mlistnode {
	int size;
	struct mlistnode *next;
	MEntry *entry;
} Entry;

typedef struct mlist {
	int size;
	Entry **table;
} MList;

int ml_verbose = 0;
int size = HASHSIZE;
int lSize = 0;


MList *ml_create(void) {
	/** Declare mailing list */
	MList *ml;

	/** loop variable */
	int i;

	/** temp hashtable initialisation variable */
	Entry *entry;

	if (ml_verbose)
		fprintf(stderr, "mlist: creating mailing list\n");



	if ((ml = (MList *)malloc(sizeof(MList))) == NULL) {
		return ml;
	}

	if ((ml->table = (Entry **)malloc(sizeof(Entry *) * size)) != NULL) {
		for (i = 0; i < size; i++) {
			ml->table[i] = (Entry *)malloc(sizeof(Entry));
			ml->table[i]->next = NULL;
		}
	}

	/** Set hash pointer to bucket */
	ml->size = size;

	/** Set mlist size */
	ml->size = size;

	return ml;

}

/** adds MEntry to list,
Returns 1 if successful
Returns 0 if not successful
*/
int ml_add(MList **ml, MEntry *me) {
	MList *m = *ml;
	unsigned long hashval;
	int i;
	Entry *current, *nextEntry;

	

	/** check duplicates */
	if (ml_lookup(m, me) != NULL) {
		return 1;
	}

	/** allocate space for next entry */
	if ((nextEntry = (Entry *)malloc(sizeof(Entry))) == NULL) {

		return 0;
	}
	nextEntry->next = NULL;

	/** Compute hash value of item */
	hashval = me_hash(me, m->size);


	/** choose appropriate bucket array from hash table */
	current = m->table[hashval];


	/** loop until free bucket */
	while (current->next != NULL) {
		current = current->next;
	}
	current->next = nextEntry;
	current->entry = me;
	//Dynamic hash table
	//if (current->size <= HASHSIZE) {
	//	/** set next to an empty bucket, and the entry to mentry */
	//	current->next = nextEntry;
	//	current->entry = me;
	//	lSize++;
	//	current->size = lSize;
	//	
	//}
	//else {
	//	current->size = 2 * CHAINSIZE;
	//}
	//return 1;
}

/** looks for entry in ml matching me
if found, return pointer, if not return NULL */
MEntry *ml_lookup(MList *ml, MEntry *me) {
	unsigned long hashval;
	Entry *buck_cursor;

	int tempsize = ml->size;

	/** print statement if verbose */
	if (ml_verbose)
		fprintf(stderr, "mlist: ml_lookup() entered\n");

	

	/** calculate hashval of me */
	hashval = me_hash(me, ml->size);


	/** loop through cursor values checking entries */
	buck_cursor = ml->table[hashval];

	while (buck_cursor->next != NULL) {
		if (me_compare(buck_cursor->entry, me) == 0) {
			/**found match, return pointer */
			return buck_cursor->entry;
		}
		else {
			/** not found, continue searching*/
			buck_cursor = buck_cursor->next;
		}
	}

	/** entry was not found, return NULL */

	ml->size = tempsize;
	return NULL;
}


void ml_destroy(MList *ml) {
	int i;
	Entry *to_delete; /** pointer to node to delete */
	Entry *next_node;	/** pointer to the next node to delete */

	if (ml_verbose)
		fprintf(stderr, "mlist: ml_destroy() entered\n");

	/** loop through each hash entry, then loop through buckets, free'ing */
	for (i = 0; i < size; i++) {
		to_delete = ml->table[i];
		while (to_delete->next != NULL) {
			next_node = to_delete->next;
			me_destroy(to_delete->entry);
			free(to_delete);
			to_delete = next_node;
		}
		free(to_delete);
	}
	/** free structures */
	free(ml->table);
	free(ml);
}