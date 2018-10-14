/*
AP3 - Assignment 1a
Tan De Hui Adrian
This is my own work as defined in the Academic Ethics agreement 
I have signed.
*/

#include <stdio.h>
#include <stdlib.h>
#include "mlist.h"

#define CHAINSIZE 20;

typedef struct entry {
	int initiated;
	struct entry *next;
	MEntry *entry;
} Entry;

typedef struct mlist {
	int size;
	Entry **table;
} MList;

int ml_verbose = 0;
int size = CHAINSIZE;


//declare method
void *reCreate(MList *ml);
void *transfer(MList *ml, MList *new_ml);


MList *ml_create(void) {
	/** Declare mailing list */
	MList *ml;

	/** loop variable */
	int i;

	/** temp hashtable initialisation variable */
	Entry *entry;

	if (ml_verbose)
		fprintf(stderr, "create list\n");
		
	if ((ml = (MList *)malloc(sizeof(MList))) == NULL) {
		return ml;
	}

	ml->size = size;
	if ((ml->table = (Entry **)malloc(sizeof(Entry *) * size)) != NULL) {
		for (i = 0; i < size; i++) {
			ml->table[i] = (Entry *)malloc(sizeof(Entry));
			ml->table[i]->next = NULL;
		}
	}

	return ml;

}

/** adds MEntry to list,
Returns 1 if successful
Returns 0 if not successful
*/
int ml_add(MList **ml, MEntry *me) {
// if(me->full_address == NULL && me->house_number == NULL && me->postcode == NULL && me->surname == NULL){
// 		return 1;
// 	}
	if (ml_verbose)
		fprintf(stderr, "add entry to list\n");
		
	
	MList *m = *ml;
	unsigned long hashval;
	int i;
	Entry *current, *nextEntry;
	int counterSize = 0;

	//Look for existing
	if (ml_lookup(m, me) != NULL) {
		return 1;
	}

	//Allocate space
	if ((nextEntry = (Entry *)malloc(sizeof(Entry))) == NULL) {

		return 0;
	}
	nextEntry->next = NULL;

	
	hashval = me_hash(me, m->size);


	//Get correct linked list
	current = m->table[hashval];


	//Look for next empty and add in
	while (current->next != NULL) {
		current = current->next;
		counterSize++;
	}
	current->next = nextEntry;
	current->entry = me;

	//Check size of list and recreate if required
	if(counterSize > m->size){
		*ml = reCreate(m);
	}
	return 1;
	
	
}

//Look for existing entry
MEntry *ml_lookup(MList *ml, MEntry *me) {
	// if(me->full_address == NULL && me->house_number == NULL && me->postcode == NULL && me->surname == NULL){
	// 	return 1;
	// }
	unsigned long hashVal;
	Entry *entryP;

	int tempSize = ml->size;

	/** print statement if verbose */
	if (ml_verbose)
		fprintf(stderr, "mlist: look for existence of entry\n");
		


	hashVal = me_hash(me, ml->size);



	entryP = ml->table[hashVal];

	while (entryP->next != NULL) {
		if (me_compare(entryP->entry, me) == 0) {
			/**found match, return pointer */
			return entryP->entry;
		}
		else {
			/** not found, continue searching*/
			entryP = entryP->next;
		}
	}
	/** entry was not found, return NULL */
	ml->size = tempSize;
	return NULL;
}


void ml_destroy(MList *ml) {
	int i;
	Entry *currNode; /** pointer to node to delete */
	Entry *nextNode;	/** pointer to the next node to delete */

	if (ml_verbose)
		fprintf(stderr, "mlist: destroying list\n");

	/** loop through each hash entry, then loop through buckets, free'ing */
	for (i = 0; i < size; i++) {
		currNode = ml->table[i];
		while (currNode->next != NULL) {
			nextNode = currNode->next;
			me_destroy(currNode->entry);
			free(currNode);
			currNode = nextNode;
		}
		free(currNode);
	}
	/** free structures */
	free(ml->table);
	free(ml);
}

//create new table with X2 of size
void *reCreate(MList *ml){
	 if(ml_verbose)
	 	fprintf(stderr,"mlist: resizing hash table\n");
		printf("Resize \n");

	/** create a new mailing list with x2 size */	
	MList *new_ml;
	size = (ml->size) * 2;
	new_ml = ml_create();
	transfer(ml, new_ml);
	
}

void *transfer(MList *ml, MList *new_ml){
 
 	int i,j;
	int bucketcount;
	unsigned long hashVal;
	
	Entry *current;
	Entry *newC;
	Entry *addC;
	
// Go through every single entry
	for(i=0;i<ml->size;i++){
		current = ml->table[i];
		while(current->next!=NULL){

			/** stores location of next node to assign space to */
			newC = current->next;
			
			/** set next value of cursor to NULL to indicate last node */
			current->next = NULL;

			hashVal = me_hash(current->entry,size);
			//use new hash value index and store in updated list
			addC = new_ml->table[hashVal];

			/** loop through new mlist buckets to find where to put new entry */
			bucketcount=0;
			while(addC->next!=NULL){
				addC = addC->next;
			}
			
			if( (new_ml->table[hashVal]->initiated == 0) ){
				new_ml->table[hashVal] = current;
				new_ml->table[hashVal]->initiated = 1;
			} 
		
			else if( (new_ml->table[hashVal]->initiated == 1) ){
				new_ml->table[hashVal]->next = current;
				new_ml->table[hashVal]->initiated = 2;
			} else
				addC->next = current;

			/** update cursor to the next node */
			current = newC;
		}
		
	}
	free(ml->table);
	free(ml);

	return new_ml;
}