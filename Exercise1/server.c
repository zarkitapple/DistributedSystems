#include <mqueue.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAXSIZE 100
#define VECTORNAME 10
#define SERVERQ "/SERVER"
#define VECTOR_INITIAL_CAPACITY 2
#define TRUE 1
#define FALSE 0
/* This struct defines the members of a request on the server */
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

/* This struct defines the members of a Vector */
typedef struct vector
{
    /* Name of the vector maximum size VECTORNAME 10*/
    char name [VECTORNAME];
    /* Number of elements */
    int size;
    /* Pointer to vector int elements */
    int * elements;
} Vector;
/* This struct defines the members of the Storage where vectors reside */
typedef struct Storage
{
    /* Current capacity of the storage to minimize the number of reallocs*/
    int capacity;
    /* Occupied space */
    int size;
    /* Pointer to the Vector elements */
    Vector * vectors;
} Storage;

/* Definition on global scope for mutexes and Storage */
pthread_mutex_t mutex_msg;
int msg_not_copied = TRUE; /* TRUE = 1 */
pthread_cond_t cond_msg;

/* Mutex for protecting access to storage */
pthread_mutex_t mutex_memory;

Storage storage;

/* Returns the index of the vector if found in storage otherwise returns -1 */
int find_vector(char * name){
    for(int ii = 0; ii< storage.size; ii++){
        if(strcmp(storage.vectors[ii].name,name)==0){
            return ii;
        }
    }
    return -1;
}

/* This function process the operation on req->operation number*/
int function_selector (struct request * req) {
    int result;
    switch (req->operation)
    {

    case 0:
        /* init(name,N) */
        printf("Init %s\n",req->name);
        result = find_vector(req->name);
        /* If Vector does not exist create it */
        if(result==-1){
            Vector vector;
            strcpy(vector.name,req->name);
            vector.size = req->arg1;
            if((vector.elements = (int *)calloc(vector.size ,sizeof(int)))==NULL){
                perror("Error when allocating mememory for vector");
                return -1;
            }
            /* Lock the access to storage when a modification happens to protect its contents*/
            pthread_mutex_lock(&mutex_memory);
            /* Use a temporary vector when allocating memory to make sure in case realloc fails 
                we are still able to free the memory pointed to by storage.vectors to prevent memory leaks*/
            Vector * vector_Tmp;
            if(storage.size >= storage.capacity){
                storage.capacity *=2;
                if((vector_Tmp= (Vector *)realloc(storage.vectors,storage.capacity * sizeof(Vector)))==NULL){
                perror("Error when reallocating memory for vector storage");
                return -1;
                }
            storage.vectors = vector_Tmp;
            }
            /* New vector at the end of the array */
            storage.vectors[storage.size] = vector;
            storage.size ++;
            pthread_mutex_unlock(&mutex_memory);
            return 1;
        }
        /* if Vector exits and its size is the same as the new size */
        else if(result!=-1 && (storage.vectors[result].size == req->arg2)){
            return 0;
        }
        return -1;
    
    case 1:
        /* set(name,index,value) */
        printf("Set %s\n",req->name);
        result = find_vector(req->name);
        /* If vector does not exist or index is out of range */
        if(result == -1 || (storage.vectors[result].size) -1 < req->arg1){
            return -1 ;
        }
        /* Protect access to storage and set the value of the vector */
        pthread_mutex_lock(&mutex_memory);
        storage.vectors[result].elements[req-> arg1] = req->arg2;
        pthread_mutex_unlock(&mutex_memory);

        return 0;
    
    case 2:
        /* get(name,index) */
        printf("Get %s\n",req->name);
        result = find_vector(req->name);
        /* If vector does not exist or index is out of range */
        if(result == -1 || (storage.vectors[result].size-1) < req->arg1){
            return -1;
        }
        /* Return the vector element */
        result = storage.vectors[result].elements[req->arg1];
        return result; 
    case 3:
        /* destroy(name) */
        printf("Destroy %s\n",req->name);
        result = find_vector(req->name);
        /* If vector does not exist */
        if(result == -1 ){
            return -1;
        }
        /* Protect access to storage when deleting a vector */
        pthread_mutex_lock(&mutex_memory);
        free(storage.vectors[result].elements);
        /* For every vector after the vector being removed [1]=[2], [2]=[3]*/
        for (int ii = result; ii < storage.size-1; ii++)
        {
            storage.vectors[ii] = storage.vectors[ii + 1];
        }
        /* Decrease the storage size */ 
        storage.size = storage.size -1; 

        pthread_mutex_unlock(&mutex_memory);

        return 1;
    }
    return -1;
}
/* Start routine of a new thread, copies to local the request, proccesses the request
    and sends ouput to the client queue */
void process_request(struct request * req){
    struct request req_local;
    mqd_t q_client;

    pthread_mutex_lock(&mutex_msg);
    /* shallow copy but deep copy is not needed as there are no pointers in the request struct */
    memcpy((struct request *)&req_local,(struct request *) req,sizeof(struct request));
    msg_not_copied = FALSE;
    pthread_cond_signal(&cond_msg);

    pthread_mutex_unlock(&mutex_msg);
    /* call function selector to process the request */
    int result = function_selector(&req_local);

    if((q_client = mq_open(req_local.q_name,O_WRONLY))==-1){
        perror("Error when opening client queue");
        pthread_exit(0);
    }
    else
    {

        //print_state();
        if((mq_send(q_client,(char *)&result,sizeof(int),0))==-1){
            perror("Error when sending output to client");
            pthread_exit(0);
        }
        if(( mq_close(q_client))==-1){
        perror("Can not close client queue descriptor");
            pthread_exit(0);
        }
    }
    
    pthread_exit(0);
}
/* Auxiliary function to print the state of the storage at a given point */
void print_state() {
     puts("Print state of the storage");
        printf("Storage size is %d\n",storage.size);
         for (int ii = 0; ii< storage.size; ii++){
            printf("Vector in storage %s\n", storage.vectors[ii].name);
            for (int jj = 0 ; jj<storage.vectors[ii].size; jj++){
                printf("Element in vector %s : %d\n", storage.vectors[ii].name, storage.vectors[ii].elements[jj]);

            }
        } 
}


int main(int argc, char const *argv[])
{
    /* Server queue attributes and variables */
    mqd_t q_server;
    Request req;
    struct mq_attr q_attr;
    /* Initialization of the storage data structure */ 
    storage.size = 0;
    storage.capacity = VECTOR_INITIAL_CAPACITY;
    storage.vectors = malloc(sizeof(Vector) * storage.capacity);
    if(storage.vectors==NULL){
        perror("Error when allocating initila memory for storage");
        return -1;
    }
    /* Server queue mmaximum number of current messages to 10 */
    q_attr.mq_maxmsg = 10;
    q_attr.mq_msgsize = sizeof(Request);

    if((q_server = mq_open(SERVERQ, O_CREAT | O_RDONLY, 0700, &q_attr))==-1){
        perror("Can not create server queue");
        return -1;
    }
    /* Thread attributes and mutexes attributes */ 
    pthread_attr_t t_attr;
    pthread_t thid;
    pthread_mutex_init(&mutex_memory,NULL);

    pthread_mutex_init(&mutex_msg,NULL);
    pthread_cond_init(&cond_msg,NULL);
    pthread_attr_init(&t_attr);
    /* Set thread as detached so it is idenpendent */
    pthread_attr_setdetachstate(&t_attr,PTHREAD_CREATE_DETACHED);

    while (TRUE)
    {
        if((mq_receive(q_server,(char *)&req,sizeof(Request),0))==-1){
            perror("Error when receiving message from client");
        }
        pthread_create(&thid,&t_attr,(void *) process_request,&req);
        /* Lock the main thread until the new thread has a local copy of the request */
        pthread_mutex_lock(&mutex_msg);
        while(msg_not_copied){
            pthread_cond_wait(&cond_msg,&mutex_msg);
        }
        msg_not_copied = TRUE;
        pthread_mutex_unlock(&mutex_msg);
    }
    return 0;
}
