#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "read_line.h"
#include <pthread.h>
#define BUFFER_SIZE 256
/* using stream sockets */

pthread_mutex_t mutex_msg;
int msg_not_copied = 1;
pthread_cond_t cond_msg;


void process_request(int * sd){
    int socket;

    pthread_mutex_lock(&mutex_msg);
    socket = *sd;
    msg_not_copied = 0;
    pthread_cond_signal(&cond_msg);
    pthread_mutex_unlock(&mutex_msg);

    char message[BUFFER_SIZE];
    int number_received;

    while (1)
        {
            if((number_received = recv(socket,message,BUFFER_SIZE,0))==-1){
                perror("Error when receiving");
            }
            if(strncmp(message,"EXIT",number_received)==0){
                break;
            }
            
            write(1, message, number_received);
            puts("");
            if ((send_msg(socket,message,number_received))==-1)
            {
                puts("Error when sending data");
            }
        }
    close(socket);
    pthread_exit(0);
}

int main(int argc, char const *argv[])
{
    int accept_socket, data_socket;
    struct sockaddr_in server_addr,client_addr;
    int val;

    accept_socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    val = 1;

    setsockopt (accept_socket,SOL_SOCKET,SO_REUSEADDR, (char *)&val,sizeof(int));

    memset((char *)&server_addr,0,sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(4200);

    bind(accept_socket,(struct sockaddr *)&server_addr,sizeof(server_addr));

    int size = sizeof(client_addr);
    /* char message[BUFFER_SIZE]; */
    //memset((char*)message,0,BUFFER_SIZE);
    /* int number_received;
    int exit =1 ; */
    listen(accept_socket,5);

    pthread_attr_t t_attr;
    pthread_t thid;
    pthread_mutex_init(&mutex_msg,NULL);
    pthread_cond_init(&cond_msg,NULL);
    pthread_attr_init(&t_attr);
    pthread_attr_setdetachstate(&t_attr,PTHREAD_CREATE_DETACHED);
    while (1)
    {
        puts("Waiting for connection");

        if(( data_socket = accept(accept_socket,(struct sockaddr *)&client_addr, &size))==-1){
             perror("Error when acepting");
        }

        pthread_create(&thid,&t_attr,(void *)process_request,&data_socket);
        pthread_mutex_lock(&mutex_msg);
        while (msg_not_copied)
        {
            pthread_cond_wait(&cond_msg,&mutex_msg);
        }
        msg_not_copied = 1;
        pthread_mutex_unlock(&mutex_msg);
        /* while (exit)
        {
            if((number_received = recv(data_socket,message,BUFFER_SIZE,0))==-1){
                perror("Error when receiving");
            }
            if(strncmp(message,"EXIT",number_received)==0){
                printf("message %s\n",message);
                exit = 0;
            }
            if(exit){
                write(1, message, number_received);
                puts("");
            if ((send_msg(data_socket,message,number_received))==-1)
            {
                puts("Error when sending data");
            }
            }
            
            
        }
        exit = 1; */
        //close(data_socket);
        //puts("End");
        /* int number_received;
        if((number_received = recv(data_socket,message_rec,BUFFER_SIZE,0))==-1){
            perror("Error when receiving");
        }
        printf("Number receive %d\n", number_received);

        printf("Message receive %s\n", message_rec);

        strcpy(message_send,"bye");

        int number_sent=send(data_socket,message_send,BUFFER_SIZE,0);
        printf("Number sent %d\n", number_sent); */
    }
    
    close(accept_socket);
    return 0;
}
