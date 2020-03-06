#ifndef ARRAY_H
#define ARRAY_H
#include <mqueue.h>
#include <stdio.h>
#include <unistd.h>
#define MAXSIZE 100
#define VECTORNAME 10
#define SERVERQ "/SERVER"

struct request
{
    int operation;
    char name [VECTORNAME];
    int arg2;
    int arg3;
    char q_name[MAXSIZE];
};


int init(char * name, int N);
int set(char * name,int i,int value);
int get(char * name,int i, int * value);
int destroy (char * name);

#endif