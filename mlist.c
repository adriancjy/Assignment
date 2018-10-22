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

    unsigned int oldCap = 0;
    unsigned int newCap = 0;

    //stepper variables
    unsigned int i=0;
    unsigned int j=0;
    unsigned long hashVal = 0;
    unsigned long newHash = 0 ;

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

   hashVal = me_hash(me,(*ml)->size);

    Entry *add_list = (*ml) -> table[hashVal];
   
    if(ml_verbose)
        fprintf(stderr, "adding to the mlist\n");

    //Look up if entry exists already
    if(ml_lookup((*ml),me) != NULL)
        return 1;

    if(Element == add_list->size){

    add_list = ml_reAdd(ml,me);
    }

    
    add_list->entryArray[add_list->size] = me;
    (add_list->size)++;
    return 1; 
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
    if (ml_verbose)
      fprintf(stderr, "List resizing...\n");
    printf("Resizing\n");

    hashVal = me_hash(me,(*ml)->size);

    
    Entry *addList = (*ml) -> table[hashVal];
    Entry **resized = NULL;
   

    //used when resizing to hold the value of the mentry
    MEntry *tempEntry = NULL;

     newCap = (*ml)->size;
     //double the size of the list
        (*ml)->size = ((*ml)->size)*2; 

       
        if((resized = calloc((*ml)->size,sizeof(Entry*))) == NULL)
            return NULL;

        oldCap = (*ml)->size;//keep the old capacity

        //allocate memory for the lists and set their size to 0
        for(i=0;i<oldCap;i++){
            resized[i] = calloc(1,sizeof(Entry));
            resized[i]->size = 0;
        }

        //Loop through all entries and add accordingly
        for(i=0;i<newCap;i++){
            for(j=0;j<(*ml)->table[i]->size;j++){
                tempEntry = (*ml)->table[i]->entryArray[j];
                newHash = me_hash(tempEntry,(*ml)->size);

                resized[newHash]->entryArray[resized[newHash]->size] = tempEntry;
                (resized[newHash]->size)++;
            }
        }

        for(i=0;i<newHash;i++)
            free((*ml)->table[i]);
        free((*ml)->table);

        //pointing the ml list to the new list, and then pointing the entry list to the ml list
        (*ml)->table = resized;
        hashVal = me_hash(me,(*ml)->size);
        addList = (*ml)->table[hashVal];
        return addList;
}