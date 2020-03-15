#ifndef _ARRAY_H
#define _ARRAY_H
#include <mqueue.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#define MAXSIZE 100
#define VECTORNAME 100
#define SERVERQ "/SERVER00383532"
/* This struct defines the members of a request to the server */
typedef struct request
{
    /* Code of the operation 0 init, 1 set, 2 get, 3 destroy */
    int operation;
    /* Name of the vector maximum size VECTORNAME 10*/
    char name [VECTORNAME];
    /* arguments*/
    int arg1;
    int arg2;
    /* Name of the client queue MAXSIZE 100*/
    char q_name[MAXSIZE];
} Request;

/* Response of the server to the client */
typedef struct response {
    /* server operation output */
    int output;
    /* returned data for get operation */
    int data;
} Response;

/* This service allows to initialize a distributed vector of N integers.
The function returns 1 when the vector is firstly created. If the vector is already created with the same
number of elements, the function returns 0. The function returns -1 on error, e.g., creating a
vector that exists with different size */
int init(char * name, int N);
/* This service inserts the value at position i of the vector name.
The function returns 0 on success and -1 on failure, for example, when inserting an element in an
invalid position or in a vector that does not exist */
int set(char * name,int i,int value);
/*  This service allows to retrieve the value of the i element of the vector name.
 The function returns 0 on success and -1 on failure, for example, you try to
retrieve an element in a invalid position or the vector does not exist. */
int get(char * name,int i, int * value);
/* This service allows to delete a previously created vector.
The functionreturns 1 on success and -1 on error.*/
int destroy (char * name);

#endif