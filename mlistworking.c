#include <stdio.h>
#include <stdlib.h>
#include "mentry.h"
#include "mlist.h"
#define Element 20
#define TSize 20

int ml_verbose=0;		/* if true, print diagnostics on stderr */

//similar to linked list (without actually linking the entries)
//size is the amount of entries currently in the list
//me_array is an array of mailing entries
typedef struct entry{
    unsigned int size;
    MEntry *entryArray[20];
}Entry;

typedef struct mlist{
    unsigned int size;
    Entry **table;
}MListT;

Entry* ml_reAdd(MList **ml, MEntry *me);

    unsigned int old_capacity = 0;
    unsigned int new_capacity = 0;

    //stepper variables
    unsigned int i=0;
    unsigned int j=0;

/* ml_create - created a new mailing list */
MList *ml_create(void){
   
    MList *mlist = NULL;
    unsigned int i=0;

    if (ml_verbose)
        fprintf(stderr, "MList: creating mailing list.\n" );

    //allocate memory for the hash table
    if ((mlist = malloc(sizeof(MList))) == NULL)
        return 0;

//set size to TSize, a constant
    mlist->size = TSize;

    //allocate memory for mlist table
    if ((mlist->table = malloc(sizeof(Entry) * mlist->size)) == NULL)
        return 0;

    //allocate memory for all the entry in the table
    for (i=0;i<mlist->size;i++){
        if ((mlist->table[i] = malloc(sizeof(Entry))) != NULL)
            mlist->table[i]->size = 0; 
    }
    return mlist;
}


/* ml_add - adds a new MEntry to the list;
 * returns 1 if successful, 0 if error (malloc)
 * returns 1 if it is a duplicate */
int ml_add(MList **ml, MEntry *me){

    //get the hash which we will use as an index in the hash table
    unsigned long hash = me_hash(me,(*ml)->size);

    //hash variable to be used when resizing
    unsigned long new_hash = 0 ;


    //variables to make life easier when adding
    Entry *add_list = (*ml) -> table[hash];
   


    if(ml_verbose)
        fprintf(stderr, "adding to the mlist\n");

    //look if the mentry is already in the table, return 1 if so
    if(ml_lookup((*ml),me) != NULL)
        return 1;

    if(Element == add_list->size){

    add_list = ml_reAdd(ml,me);
    }

    //no problems, just add to the list
    add_list->entryArray[add_list->size] = me;
    (add_list->size)++;
    return 1; //successful
}


/* ml_lookup - looks for MEntry in the list, returns matching entry or NULL */
MEntry *ml_lookup(MList *ml, MEntry *me){
    unsigned long hashVal = me_hash(me,ml->size);
    MEntry *compare = NULL;
    int i;
    for(i = 0; i < ml->table[hashVal]->size; i++){
        compare = ml->table[hashVal]->entryArray[i];
        if(me_compare(compare,me)==0)
            return compare;
    }
    return NULL;
}


/* ml_destroy - destroy the mailing list */
void ml_destroy(MList *ml){
    
    int i = 0;
    int j = 0;
    unsigned int initialCap = ml->size;

    if (ml_verbose)
      fprintf(stderr, "ml_destroy...\n");

    for(i=0;i<initialCap;i++){
        for(j=0;j<ml->table[i]->size;j++)
           
		   //destroy and free the entry
            me_destroy(ml->table[i]->entryArray[j]);

        
        free(ml->table[i]);
    }
    
    free(ml->table);
    free(ml);
	ml = NULL;
}

Entry* ml_reAdd(MList **ml, MEntry *me){
     unsigned long hash = me_hash(me,(*ml)->size);

    //hash variable to be used when resizing
    unsigned long new_hash = 0 ;


    //variables to make life easier when adding
    Entry *add_list = (*ml) -> table[hash];
    Entry **resize_list = NULL;
   

    //used when resizing to hold the value of the mentry
    MEntry *stepper_mentry = NULL;

     new_capacity = (*ml)->size;
        (*ml)->size = ((*ml)->size)*2;//double the capacity since a list is full

        //if not enough memmory when resizing -> return 0
        if((resize_list = calloc((*ml)->size,sizeof(Entry*))) == NULL)
            return NULL;

        old_capacity = (*ml)->size;//keep the old capacity

        //allocate memory for the lists and set their size to 0
        for(i=0;i<old_capacity;i++){
            resize_list[i] = calloc(1,sizeof(Entry));
            resize_list[i]->size = 0;
        }

        //loop through all entries in the lists, get their new hash and add them
        //to the appropriate list
        for(i=0;i<new_capacity;i++){
            for(j=0;j<(*ml)->table[i]->size;j++){
                stepper_mentry = (*ml)->table[i]->entryArray[j];
                new_hash = me_hash(stepper_mentry,(*ml)->size);

                resize_list[new_hash]->entryArray[resize_list[new_hash]->size] = stepper_mentry;
                (resize_list[new_hash]->size)++;
            }
        }

        //free all allocated memory for the old lists & hash table
        for(i=0;i<new_capacity;i++)
            free((*ml)->table[i]);
        free((*ml)->table);

        //point to the new list & get their hash
        (*ml)->table = resize_list;
        hash = me_hash(me,(*ml)->size);
        add_list = (*ml)->table[hash];//point to the correct list (after resizing)
        return add_list;
}