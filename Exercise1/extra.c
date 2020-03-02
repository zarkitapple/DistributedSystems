#include "array.h"
#include <stdlib.h>

int length (char *string){
 int j = 0;
 while (*string) {
    string++;
    j++;
}
 return j; 
}
int init(char * name, int N){
    if(name==NULL) {
        name = (int *)malloc(N*sizeof(int));
        return 1;
    }
    int nelements = length(name);
    
    if(nelements==N) {
        return 0;
    }
    else {
        return -1;
    }
    
}

int set(char * name,int i,int value) {
    if(name == NULL){
        return -1;
    }
    int nelements = length(name);
    if(i<0 || i>nelements){
        return -1;
    }
    name[i] = value;
    return 0;
}

int get(char * name,int i, int * value){
    if(name == NULL){
        return -1;
    }
    int nelements = length(name);
    if(i<0 || i>nelements){
        return -1;
    }
    *value = name[i]; 
    return 0;
}

int destroy (char * name){
    if(name == NULL){
        return -1;
    }
    free(name);
}
