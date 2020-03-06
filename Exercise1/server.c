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

struct request
{
    int operation;
    char name [VECTORNAME];
    int arg2;
    int arg3;
    char q_name[MAXSIZE];
};

typedef struct vector
{
    char name [VECTORNAME] ;
    int size;
    int * elements;
} Vector;

typedef struct Storage
{
    int capacity;
    int size;
    Vector * vectors;
} Storage;


pthread_mutex_t mutex_msg;
int msg_not_copied = TRUE; /* TRUE = 1 */
pthread_cond_t cond_msg;

pthread_mutex_t mutex_memmory;

Storage storage;

int find_vector(char * name){

    for(int ii = 0; ii< storage.size; ii++){
        if(strcmp(storage.vectors[ii].name,name)==0){
            return ii;
        }
    }

    return -1;
}


int function_selector (struct request * req) {
    int result;
    switch (req->operation)
    {
    case 0:
        /* init("vector",100) name, arg2*/
        printf("Init %s\n",req->name);
        result = find_vector(req->name);
        /* Vector does not exist */
        if(result==-1){
            Vector vector;
            strcpy(vector.name,req->name);
            vector.size = req->arg2;
            if((vector.elements = (int *)calloc(vector.size ,sizeof(int)))==NULL){
                perror("Error when allocating mememory for vector");
                return -1;
            }

            pthread_mutex_lock(&mutex_memmory);
            
            Vector * vector_Tmp;
            if(storage.size >= storage.capacity){
                storage.capacity *=2;
                if((vector_Tmp= (Vector *)realloc(storage.vectors,storage.capacity * sizeof(Vector)))==NULL){
                perror("Error when reallocating memory for vector storage");
                return -1;
                }
            storage.vectors = vector_Tmp;
            }
            storage.vectors[storage.size] = vector;
            storage.size ++;
            pthread_mutex_unlock(&mutex_memmory);
            return 1;
        }
        else if(result!=-1 && (storage.vectors[result].size == req->arg2)){
            return 0;
        }
        return -1;
    
    case 1:
        /* set("vector",0,40) */
        printf("Set %s\n",req->name);
        result = find_vector(req->name);
        if(result == -1 || (storage.vectors[result].size) -1 < req->arg2){
            return -1 ;
        }
        pthread_mutex_lock(&mutex_memmory);
        storage.vectors[result].elements[req-> arg2] = req->arg3;
        pthread_mutex_unlock(&mutex_memmory);

        return 0;
    
    case 2:
        /* get("vector",2,value) */
        printf("Get %s\n",req->name);
        result = find_vector(req->name);
        if(result == -1 || (storage.vectors[result].size-1) < req->arg2){
            return -1;
        }
        result = storage.vectors[result].elements[req->arg2];
        return result; 
    case 3:
        /* destroy("vector") */
        printf("Destroy %s\n",req->name);
        result = find_vector(req->name);
        if(result == -1 ){
            return -1;
        }
        pthread_mutex_lock(&mutex_memmory);
        free(storage.vectors[result].elements);
        for (int ii = result; ii < storage.size-1; ii++)
        {
            storage.vectors[ii] = storage.vectors[ii + 1];
        }
        storage.size = storage.size -1; 

        pthread_mutex_unlock(&mutex_memmory);

        return 1;
    }
    return -1;
}
void process_request(struct request * req){
    struct request req_local;
    mqd_t q_client;

    pthread_mutex_lock(&mutex_msg);
    // shallow copy but it is not needed deep copy because it is memorized in the struct req
    memcpy((struct request *)&req_local,(struct request *) req,sizeof(struct request));
    msg_not_copied = FALSE;
    pthread_cond_signal(&cond_msg);

    pthread_mutex_unlock(&mutex_msg);
    int result = function_selector(&req_local);

    if((q_client = mq_open(req_local.q_name,O_WRONLY))==-1){
        perror("Error when opening client queue");
        pthread_exit(0);
    }
    else
    {

        //print_state();
        mq_send(q_client,(char *)&result,sizeof(int),0);
        mq_close(q_client);
    }
    

    pthread_exit(0);
}

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
    mqd_t q_server;
    struct request req;
    struct mq_attr q_attr;
    pthread_attr_t t_attr;
    /* Initialization of the storage data structure */ 
    storage.size = 0;
    storage.capacity = VECTOR_INITIAL_CAPACITY;
    storage.vectors = malloc(sizeof(Vector) * storage.capacity);


    q_attr.mq_maxmsg = 10;
    q_attr.mq_msgsize = sizeof(struct request);

    if((q_server = mq_open(SERVERQ, O_CREAT | O_RDONLY, 0700, &q_attr))==-1){
        perror("Can not create server queue");
        return -1;
    }

    pthread_t thid;
    pthread_mutex_init(&mutex_memmory,NULL);

    pthread_mutex_init(&mutex_msg,NULL);
    pthread_cond_init(&cond_msg,NULL);
    pthread_attr_init(&t_attr);
    pthread_attr_setdetachstate(&t_attr,PTHREAD_CREATE_DETACHED);

    while (TRUE)
    {

        mq_receive(q_server,&req,sizeof(struct request),0);
        pthread_create(&thid,&t_attr,(void *) process_request,&req);
        pthread_mutex_lock(&mutex_msg);
        while(msg_not_copied){
            pthread_cond_wait(&cond_msg,&mutex_msg);
        }
        msg_not_copied = TRUE;
        pthread_mutex_unlock(&mutex_msg);
    }
    
    return 0;
}
