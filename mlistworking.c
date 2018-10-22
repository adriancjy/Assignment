#include <stdio.h>
#include <stdlib.h>
#include "mentry.h"
#include "mlist.h"
#define LISTELEMENTS 20
#define HASHBUCKETS 20

int ml_verbose=0;		/* if true, print diagnostics on stderr */

//similar to linked list (without actually linking the entries)
//size is the amount of entries currently in the list
//me_array is an array of mailing entries
typedef struct list{
    unsigned int size;
    MEntry *me_array[20];
}LList;

//holds the hash table, cap is the current capacity of the table
//list is a pointer to an array of LList elements
typedef struct mlist{
    unsigned int size;
    LList **list;
}MListT;


/* ml_create - created a new mailing list */
MList *ml_create(void){
    //initalize list, set to NULL for security
    MList *mlist = NULL;
    unsigned int i=0;

    if (ml_verbose)
        fprintf(stderr, "MList: creating zhe mailing list.\n" );

    //allocate memory for the hash table
    if ((mlist = calloc(1,sizeof(MList))) == NULL)
        return 0;

    mlist->size = HASHBUCKETS;//initial number of lists

    //allocate memory for the structure with points to all the lists
    if ((mlist->list = calloc(mlist->size, sizeof(LList))) == NULL)
        return 0;

    //allocate memory for every specific list
    for (i=0;i<mlist->size;i++){
        if ((mlist->list[i] = calloc(1,sizeof(LList))) != NULL)
            mlist->list[i]->size = 0; //set intial size of every list to 0 -> no elements
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
    LList *add_list = (*ml) -> list[hash];
    LList **resize_list = NULL;
    unsigned int old_capacity = 0;
    unsigned int new_capacity = 0;

    //stepper variables
    unsigned int i=0;
    unsigned int j=0;

    //used when resizing to hold the value of the mentry
    MEntry *stepper_mentry = NULL;

    if(ml_verbose)
        fprintf(stderr, "adding to the mlist\n");

    //look if the mentry is already in the table, return 1 if so
    if(ml_lookup((*ml),me) != NULL)
        return 1;

    while(LISTELEMENTS == add_list->size){ //loop until all linked lists are of correct size

    ml_readd((*ml), me);
        // new_capacity = (*ml)->size;
        // (*ml)->size = ((*ml)->size)*2;//double the capacity since a list is full

        // //if not enough memmory when resizing -> return 0
        // if((resize_list = calloc((*ml)->size,sizeof(LList*))) == NULL)
        //     return 0;

        // old_capacity = (*ml)->size;//keep the old capacity

        // //allocate memory for the lists and set their size to 0
        // for(i=0;i<old_capacity;i++){
        //     resize_list[i] = calloc(1,sizeof(LList));
        //     resize_list[i]->size = 0;
        // }

        // //loop through all entries in the lists, get their new hash and add them
        // //to the appropriate list
        // for(i=0;i<new_capacity;i++){
        //     for(j=0;j<(*ml)->list[i]->size;j++){
        //         stepper_mentry = (*ml)->list[i]->me_array[j];
        //         new_hash = me_hash(stepper_mentry,(*ml)->size);

        //         resize_list[new_hash]->me_array[resize_list[new_hash]->size] = stepper_mentry;
        //         (resize_list[new_hash]->size)++;
        //     }
        // }

        // //free all allocated memory for the old lists & hash table
        // for(i=0;i<new_capacity;i++)
        //     free((*ml)->list[i]);
        // free((*ml)->list);

        // //point to the new list & get their hash
        // (*ml)->list = resize_list;
        // hash = me_hash(me,(*ml)->size);
        // add_list = (*ml)->list[hash];//point to the correct list (after resizing)
    }

    //no problems, just add to the list
    add_list->me_array[add_list->size] = me;
    (add_list->size)++;
    return 1; //successful
}


/* ml_lookup - looks for MEntry in the list, returns matching entry or NULL */
MEntry *ml_lookup(MList *ml, MEntry *me){
    unsigned long hash = me_hash(me,ml->size);
    MEntry *compare_me = NULL;
    int i;
    for(i = 0; i < ml->list[hash]->size; i++){
        compare_me = ml->list[hash]->me_array[i];
        if(me_compare(compare_me,me)==0)
            return compare_me;
    }
    return NULL;
}

/* ml_destroy - destroy the mailing list */
void ml_destroy(MList *ml){
    //stepper variables: one for looping through the hash table
    //one for looping through the lists
    int i = 0;
    int j = 0;
    unsigned int initial_cap = ml->size;

    if (ml_verbose)
      fprintf(stderr, "ml_destroy...\n");

    for(i=0;i<initial_cap;i++){
        for(j=0;j<ml->list[i]->size;j++)
            //use function from mentry.c to destroy the mailing entry strucutre
            //along with address postcode and surname
            me_destroy(ml->list[i]->me_array[j]);

        //free each list when everything inside is free'd
        free(ml->list[i]);
    }
    //free the table and the array holding all the lists
    free(ml->list);
    free(ml);
}

int ml_readd(MList **ml, MEntry *me){
     //get the hash which we will use as an index in the hash table
    unsigned long hash = me_hash(me,(*ml)->size);

    //hash variable to be used when resizing
    unsigned long new_hash = 0 ;


    //variables to make life easier when adding
    LList *add_list = (*ml) -> list[hash];
    LList **resize_list = NULL;
    unsigned int old_capacity = 0;
    unsigned int new_capacity = 0;

    //stepper variables
    unsigned int i=0;
    unsigned int j=0;

    //used when resizing to hold the value of the mentry
    MEntry *stepper_mentry = NULL;
      new_capacity = (*ml)->size;
        (*ml)->size = ((*ml)->size)*2;//double the capacity since a list is full

        //if not enough memmory when resizing -> return 0
        if((resize_list = calloc((*ml)->size,sizeof(LList*))) == NULL)
            return 0;

        old_capacity = (*ml)->size;//keep the old capacity

        //allocate memory for the lists and set their size to 0
        for(i=0;i<old_capacity;i++){
            resize_list[i] = calloc(1,sizeof(LList));
            resize_list[i]->size = 0;
        }

        //loop through all entries in the lists, get their new hash and add them
        //to the appropriate list
        for(i=0;i<new_capacity;i++){
            for(j=0;j<(*ml)->list[i]->size;j++){
                stepper_mentry = (*ml)->list[i]->me_array[j];
                new_hash = me_hash(stepper_mentry,(*ml)->size);

                resize_list[new_hash]->me_array[resize_list[new_hash]->size] = stepper_mentry;
                (resize_list[new_hash]->size)++;
            }
        }

        //free all allocated memory for the old lists & hash table
        for(i=0;i<new_capacity;i++)
            free((*ml)->list[i]);
        free((*ml)->list);

        //point to the new list & get their hash
        (*ml)->list = resize_list;
        hash = me_hash(me,(*ml)->size);
        add_list = (*ml)->list[hash];//point to the correct list (after resizing)

}