#include "array.h"

int init(char * name, int N){
    /* Check name lenght and N value before contacting the server */
    if(strlen(name)+1>VECTORNAME || N<0){
        return -1;
    }
    /* Server and client queue parameters */
    mqd_t q_server;
    mqd_t q_client;
    Request req;
    struct mq_attr q_attr;
    /* Size to 1 ad there will only be on message in client queue */
    q_attr.mq_maxmsg = 1;
    q_attr.mq_msgsize = sizeof(Response);
    /* The name of the client queue will be its process id, so it is fully identifiable*/
    sprintf(req.q_name,"/%d",getpid());
    /* Value to use for return */
    Response server_response;
    /* Try to open client queue */
    if((q_client = mq_open(req.q_name, O_CREAT | O_RDONLY,0700,&q_attr))==-1){
       perror("Can not create client queue");
       return -1;
    }
    /* Try to open server queue */
    if((q_server = mq_open(SERVERQ,O_WRONLY))==-1) {
       perror("Can not create open server queue");
       return -1;
   }
    /* Set the request parameters */
    req.operation = 0;
    /* As vector name is declared in this process memory space in order for another 
        proces to have access for it we copy the value */
    strcpy(req.name,name);
    req.arg1 = N;
    /* Send request to server queue */
    if(( mq_send(q_server, (char *)&req, sizeof(Request),0))==-1){
        perror("Error when sending message to server");
        return -1;
    }
    /* Receive result from server queue */
    if(( mq_receive(q_client,(char *)&server_response,sizeof(Response),0))==-1){
        perror("Error when receiving message from server");
        return -1;
    }
    /* Close server queue desciptor */
    mq_close(q_server);
    
    /* Close client queue desciptor */
    mq_close(q_client);
    /* Remove client queue name */
    if(( mq_unlink(req.q_name))==-1){
        perror("Can not remove the client queue name");
        return -1;
    }
    return server_response.output;
}

int set(char * name,int i,int value){
    /* Check name lenght and i value before contacting the server */
    if(strlen(name)+1>VECTORNAME || i<0){
        return -1;
    }
     /* Server and client queue parameters */
    mqd_t q_server;
    mqd_t q_client;
    Request req;
    struct mq_attr q_attr;
    /* Size to 1 ad there will only be on message in client queue */
    q_attr.mq_maxmsg = 1;
    q_attr.mq_msgsize = sizeof(Response);
    /* The name of the client queue will be its process id, so it is fully identifiable*/
    sprintf(req.q_name,"/%d",getpid());
    /* Value to use for return */
    Response server_response;
    /* Try to open client queue */
    if((q_client = mq_open(req.q_name, O_CREAT | O_RDONLY,0700,&q_attr))==-1){
       perror("Can not create client queue");
       return -1;
    }
    /* Try to open server queue */
    if((q_server = mq_open(SERVERQ,O_WRONLY))==-1) {
       perror("Can not create open server queue");
       return -1;
   }
  
     /* Set the request parameters */
     req.operation = 1;
     /* As vector name is declared in this process memory space in order for another 
        proces to have access for it we copy the value */
     strcpy(req.name,name);
     req.arg1 = i;
     req.arg2 = value;
     
    /* Send request to server queue */
    if(( mq_send(q_server, (char *)&req, sizeof(Request),0))==-1){
         perror("Error when sending message to server");
        return -1;
    }
    /* Receive result from server queue */
    if(( mq_receive(q_client,(char *)&server_response,sizeof(Response),0))==-1){
        perror("Error when receiving message from server");
        return -1;
    }
    /* Close server queue desciptor */
    mq_close(q_server);
    /* Close client queue desciptor */
    mq_close(q_client);
    /* Remove client queue name */
    if(( mq_unlink(req.q_name))==-1){
        perror("Can not remove the client queue name");
        return -1;
    } 
    return server_response.output;
}

int get(char * name,int i, int * value){
    /* Check name lenght and N value before contacting the server */
    if(strlen(name)+1>VECTORNAME || i<0){
        return -1;
    }
    /* Server and client queue parameters */
    mqd_t q_server;
    mqd_t q_client;
    Request req;
    struct mq_attr q_attr;
    /* Size to 1 ad there will only be on message in client queue */
    q_attr.mq_maxmsg = 1;
    q_attr.mq_msgsize = sizeof(Response);
    /* The name of the client queue will be its process id, so it is fully identifiable*/
    sprintf(req.q_name,"/%d",getpid());
    /* Value to use for return */
    Response server_response;
    /* Try to open client queue */
    if((q_client = mq_open(req.q_name, O_CREAT | O_RDONLY,0700,&q_attr))==-1){
       perror("Can not create client queue");
       return -1;
    }
    /* Try to open server queue */
    if((q_server = mq_open(SERVERQ,O_WRONLY))==-1) {
       perror("Can not create open server queue");
       return -1;
   }
    /* Set the request parameters */
    req.operation = 2;
    /* As vector name is declared in this process memory space in order for another 
        proces to have access for it we copy the value */
    strcpy(req.name,name);
    req.arg1 = i; 

     /* Send request to server queue */
    if(( mq_send(q_server, (char *)&req, sizeof(Request),0))==-1){
         perror("Error when sending message to server");
        return -1;
    }
    /* Receive result from server queue */
    if(( mq_receive(q_client,(char *)&server_response,sizeof(Response),0))==-1){
         perror("Error when receiving message from server");
        return -1;
    }
    /* Close server queue desciptor */
    mq_close(q_server);
    /* Close client queue desciptor */
    mq_close(q_client);
    /* Remove client queue name */
    if(( mq_unlink(req.q_name))==-1){
        perror("Can not remove the client queue name");
        return -1;
    }
    if(server_response.output!=-1){
         *value = server_response.data;
    }
    return server_response.output;
}

int destroy (char * name){
    /* Check name lenght before contacting the server */
    if(strlen(name)+1>VECTORNAME){
        return -1;
    }
    /* Server and client queue parameters */
    mqd_t q_server;
    mqd_t q_client;
    Request req;
    struct mq_attr q_attr;
    /* Size to 1 ad there will only be on message in client queue */
    q_attr.mq_maxmsg = 1;
    q_attr.mq_msgsize = sizeof(Response);
    /* The name of the client queue will be its process id, so it is fully identifiable*/
    sprintf(req.q_name,"/%d",getpid());
    /* Value to use for return */
    Response server_response;
    /* Try to open client queue */
    if((q_client = mq_open(req.q_name, O_CREAT | O_RDONLY,0700,&q_attr))==-1){
       perror("Can not create client queue");
       return -1;
    }
    /* Try to open server queue */
    if((q_server = mq_open(SERVERQ,O_WRONLY))==-1) {
       perror("Can not create open server queue");
       return -1;
   }
    /* Set the request parameters */
    req.operation = 3;
    strcpy(req.name,name);

    /* Send request to server queue */
    if(( mq_send(q_server, (char *)&req, sizeof(Request),0))==-1){
         perror("Error when sending message to server");
        return -1;
    }
    /* Receive result from server queue */
    if(( mq_receive(q_client,(char *)&server_response,sizeof(Response),0))==-1){
         perror("Error when receiving message from server");
        return -1;
    }
    /* Close server queue desciptor */
    mq_close(q_server);
    /* Close client queue desciptor */
    mq_close(q_client);
    /* Remove client queue name */
    if(( mq_unlink(req.q_name))==-1){
        perror("Can not remove the client queue name");
        return -1;
    }
    return server_response.output;
}