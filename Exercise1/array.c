#include "array.h"
# include <stdlib.h>
# include <string.h>


int init(char * name, int N){
   mqd_t q_server;
    mqd_t q_client;
    struct request req;
    struct mq_attr q_attr;

   q_attr.mq_maxmsg = 1;
   q_attr.mq_msgsize = sizeof(int);
   sprintf(req.q_name,"/%d",getpid());
   int output;

   if((q_client = mq_open(req.q_name, O_CREAT | O_RDONLY,0700,&q_attr))==-1){
       perror("Can not create client queue");
       exit(-1);
   }
   if((q_server = mq_open(SERVERQ,O_WRONLY))==-1) {
       perror("Can not create open server queue");
        exit(-1);
   }

   req.operation = 0;
   strcpy(req.name,name);
   req.arg2 = N;

    mq_send(q_server, &req, sizeof(struct request),0);
    mq_receive(q_client,&output,sizeof(int),0);

    mq_close(q_server);
    mq_close(q_client);
    mq_unlink(req.q_name);
    return output;
}

int set(char * name,int i,int value){
     mqd_t q_server;
    mqd_t q_client;
    struct request req;
    struct mq_attr q_attr;

   q_attr.mq_maxmsg = 1;
   q_attr.mq_msgsize = sizeof(int);
   sprintf(req.q_name,"/%d",getpid());
   int output;

   if((q_client = mq_open(req.q_name, O_CREAT | O_RDONLY,0700,&q_attr))==-1){
       perror("Can not create client queue");
        exit(-1);
   }
   if((q_server = mq_open(SERVERQ,O_WRONLY))==-1) {
       perror("Can not create open server queue");
        exit(-1);
   }

   req.operation = 1;
   strcpy(req.name,name);
   req.arg2 = i;
   req.arg3 = value;
    mq_send(q_server, &req, sizeof(struct request),0);
    mq_receive(q_client,&output,sizeof(int),0);

    mq_close(q_server);
    mq_close(q_client);
    mq_unlink(req.q_name);
    return output;
}

int get(char * name,int i, int * value){
     mqd_t q_server;
    mqd_t q_client;
    struct request req;
    struct mq_attr q_attr;

   q_attr.mq_maxmsg = 1;
   q_attr.mq_msgsize = sizeof(int);
   sprintf(req.q_name,"/%d",getpid());
   int output;

   if((q_client = mq_open(req.q_name, O_CREAT | O_RDONLY,0700,&q_attr))==-1){
       perror("Can not create client queue");
        exit(-1);
   }
   if((q_server = mq_open(SERVERQ,O_WRONLY))==-1) {
       perror("Can not create open server queue");
        exit(-1);
   }

    req.operation = 2;
    strcpy(req.name,name);
   req.arg2 = i;
   req.arg3 = value; 

    mq_send(q_server, &req, sizeof(struct request),0);
    mq_receive(q_client,&output,sizeof(int),0);

    mq_close(q_server);
    mq_close(q_client);
    mq_unlink(req.q_name);
    if(output!=0 && output!=-1){
        *value = output;
        output = 0;
        return output;
    }
    *value = 0;
    return output;
}

int destroy (char * name){
     mqd_t q_server;
    mqd_t q_client;
    struct request req;
    struct mq_attr q_attr;

   q_attr.mq_maxmsg = 1;
   q_attr.mq_msgsize = sizeof(int);
   sprintf(req.q_name,"/%d",getpid());
   int output;

   if((q_client = mq_open(req.q_name, O_CREAT | O_RDONLY,0700,&q_attr))==-1){
       perror("Can not create client queue");
        exit(-1);
   }
   if((q_server = mq_open(SERVERQ,O_WRONLY))==-1) {
       perror("Can not create open server queue");
        exit(-1);
   }

   req.operation = 3;
   strcpy(req.name,name);
    mq_send(q_server, &req, sizeof(struct request),0);
    mq_receive(q_client,&output,sizeof(int),0);

    mq_close(q_server);
    mq_close(q_client);
    mq_unlink(req.q_name);
    return output;
}