#include <stdio.h>
#include <stdlib.h>
#include "mentry.h"
#include "mlist.h"
#define Element 20
#define TSize 20

int ml_verbose=0;		

//create an array to hold 20 entries 
typedef struct entry{
    unsigned int size;
    MEntry *entryArray[20];
}Entry;


typedef struct mlist{
    unsigned int size;
    Entry **table;
}MListT;

//Declare to remove implicit warning
Entry *ml_readd(MList *ml, MEntry *me);

//Global variables reused
 	unsigned int oldCap = 0;
    unsigned int newCap = 0;

   // Variable for for loop to access entry and table
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

     //get hash value of me using size of list
    unsigned long hashVal = me_hash(me,(*ml)->size);


    Entry *addList = (*ml)->table[hashVal];

    if(ml_verbose)
        fprintf(stderr, "Adding to the mlist\n");

   
    if(ml_lookup((*ml),me) != NULL)
        return 1;

    //checks if addlist reaches size of 20, if so, increase table size
    if(Element == addList->size){ 

	//resizing list
    addList = ml_readd((*ml), me);
    }

    //addlist->size  is  0 initially, initialized in ml_create
    addList->entryArray[addList->size] = me;
    (addList->size)++;
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

Entry *ml_readd(MList *ml, MEntry *me){

     //get hash value of me using size of list
    unsigned long hashVal = me_hash(me, ml->size);

    //new hashVariable
    unsigned long newhashVal = 0 ;


    Entry *addList = ml -> table[hashVal];
    Entry **newList = NULL;
   

    //used when resizing to hold the value of the mentry
    MEntry *tempEntry = NULL;
      oldCap = ml->size;
        ml->size = (ml->size)*2;

       
        if((newList = calloc(ml->size,sizeof(Entry*))) == NULL)
		return NULL;
            

        newCap = ml->size;

        //allocate memory for the lists and set their size to 0
        for(i=0;i<oldCap;i++){
            newList[i] = calloc(1,sizeof(Entry));
            newList[i]->size = 0;
        }

        
        for(i=0;i<newCap;i++){
            for(j=0;j<ml->table[i]->size;j++){
                tempEntry = ml->table[i]->entryArray[j];
                newhashVal = me_hash(tempEntry,ml->size);

                newList[newhashVal]->entryArray[newList[newhashVal]->size] = tempEntry;
                (newList[newhashVal]->size)++;
            }
        }

        //free all allocated memory for the old lists & hash table
        for(i=0;i<newCap;i++)
            free(ml->table[i]);
        free(ml->table);

        //point to the new list & get their hash
        ml->table = newList;
        hashVal = me_hash(me,ml->size);
        addList = ml->table[hashVal];

		return addList; 
}