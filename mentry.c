/*
AP3 - Assignment 1a
Tan De Hui Adrian
This is my own work as defined in the Academic Ethics agreement 
I have signed.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "mentry.h"
#pragma warning(disable : 4996)  
#define SURNAMECT 100
#define POSTALCT 20
#define MAXLINE 500

int checkForNum(char *copyLine);
char *spaceEntry(char *copyLine);
char *commaEntry(char *copyLine);


MEntry *me;
//This function takes in the entries in the text file and add to MEntry line by line.
MEntry *me_get(FILE *fd) {

	char entryLine[MAXLINE + 1] = { 0 };
	//To check and validate surname	
	/** loop counter */
	int i, j;

	if ((me = (MEntry *)malloc(sizeof(MEntry))) != NULL) {
		/** allocate memory on heap for entry */
		me->surname = malloc(sizeof(char)*SURNAMECT);
		me->postcode = malloc(sizeof(char)*POSTALCT);
		me->full_address = malloc(sizeof(char) * MAXLINE * 3);
	}

		/** import data into temp storage */
		fgets(entryLine, MAXLINE, fd);


		/** Nothing left in file to input, return null */
		if (entryLine[0] == '\0') {
			me_destroy(me);
			return NULL;
		}

		int checkNum = checkForNum(entryLine);
	
		//No number
		if (checkNum == 0) {
			if (strchr(entryLine, ',') != NULL) {
				
				me->surname = commaEntry(entryLine);
			}
			else {
				me->surname = spaceEntry(entryLine);
				
				
			}
		}
		else {
			printf("Not adding");
		}

		strcpy(me->full_address, entryLine);

		/** import house number to MEntry */
		fgets(entryLine, MAXLINE, fd);
	
		me->house_number = atoi(entryLine);
		strcat(me->full_address, entryLine);

		/** import postcode to MEntry removing spaces and non-alphanumeric chars */
		fgets(entryLine, MAXLINE, fd);
		for (i = 0, j = 0; entryLine[i] != '\n'; i++) {
			if (isalnum(entryLine[i]) && entryLine[i] != ' ') {
				me->postcode[j] = entryLine[i];
				j++;
			}
		}
		me->postcode[j] = '\0';
		strcat(me->full_address, entryLine);
		return me;

	}
	


void me_print(MEntry *me, FILE *fd) {
	fprintf(fd, "%s", me->full_address);
}


int me_compare(MEntry *me1, MEntry *me2) {
	int trueVal = 0;
	if (me1 == NULL || me2 == NULL) {
		return trueVal + 1;
	}
	if (strcmp(me1->surname, me2->surname) == 0 && strcmp(me1->postcode, me2->postcode) == 0 && (me1->house_number == me2->house_number)) {
		return trueVal;
	}
	else if (me1->house_number < me2->house_number) {

		return trueVal - 1;
	}
	else {
		return trueVal + 1;
	}
}

//Compute hash value with surname + zipcode + house number
unsigned long me_hash(MEntry *me, unsigned long size) {
	int i = 0;
	int hashVal = 3;
	unsigned long sum = 0;
	sum += (unsigned long)me->house_number;

	
	for (i = 0; me->surname[i] != '\0'; i++) {
		sum += (unsigned long)((me->surname[i]) * hashVal);
	}
	for (i = 0; me->postcode[i] != '\0'; i++) {
		sum += (unsigned long)((me->postcode[i])  * hashVal);
	}
	sum = sum % size;
	return sum; // Return value in between size and size - 1
}

//Checks me if it is empty, if so, free upon destroy method being called.
void me_destroy(MEntry *me) {
	if (me != NULL) {
		free(me->full_address);
		free(me->surname);
		free(me->postcode);
	}
	free(me);
	me = NULL;
}
//Get surname with comma
char *commaEntry(char *copyLine) {
	int y=0, z=0;
		for ( z = 0; copyLine[z] != ','; z++) {
					me->surname[y++] = copyLine[z];
				}
				me->surname[y] = '\0';

				return me->surname;
}
//Get surname with space
char *spaceEntry(char *copyLine){
	int count = 0;
				int y = 0,  z=0;
				for ( z = 0; copyLine[z]; z++) {
					if (copyLine[z] == ' ' && count < 2) {
						count += 1;
					}
					else if(copyLine[z] != ' ' && count == 2){
						me->surname[y++] = copyLine[z];
						

					}
				}
				me->surname[y-1] = '\0';

				return me->surname;
}

//check surname for number, if exist, return 1, else 0
int checkForNum(char *copyLine) {
	int length = strlen(copyLine);
	int retVal = 0;
	for (int i = 0; i < length - 1; i++) {
		if (copyLine[i] >= '0' && copyLine[i] < '9') {
			retVal + 1;
		}

	}
	return retVal;
}


