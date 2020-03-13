#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "lines.h"
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sys/stat.h>
#include <dirent.h>
#include <linux/limits.h>
#define MESSAGE_SIZE 256
#define MAX_REQUESTS 10
#define USERS_DIRECTOY "/Users"
#define REGISTER "REGISTER"
#define UNREGISTER "UNREGISTER"
#define CONNECT "CONNECT"
pthread_mutex_t mutex_msg;
int msg_not_copied = 1;
pthread_cond_t cond_msg;


int register_user (int socket) {
	char user_name[MESSAGE_SIZE];
    int number_received;
	if((number_received=readLine(socket,user_name,MESSAGE_SIZE))==-1){
		   perror("Error when recieving data");
		   return 2;
	}
	DIR * directory;
	struct dirent * d_entry;
	char path[PATH_MAX];

	getcwd(path,PATH_MAX);
	strcat(path,USERS_DIRECTOY);

	if((directory= opendir(path))==NULL){
		perror("Error when opening users directory");
		return 2;
	}
	while ((d_entry = readdir(directory)))
	{
		if(strcmp(d_entry->d_name,user_name)==0){
			return 1;
		}
	}
	if(mkdir(user_name,0777)==-1){
		perror("Cannot create user directory");
	}
	return 0;
}
/* 
int unregister_user(int sd){
	char user_name[MESSAGE_SIZE];
    int number_received;
	if((number_received=readLine(socket,user_name,MESSAGE_SIZE))==-1){
		   perror("Error when recieving data");
		   return 2;
	}
	DIR * directory;
	struct dirent * d_entry;
	char path[PATH_MAX];

	getcwd(path,PATH_MAX);
	strcat(path,USERS_DIRECTOY);

	if((directory= opendir(path))==NULL){
		perror("Error when opening users directory");
		return 2;
	}
	while (d_entry = readdir(directory))
	{
		if(strcmp(d_entry->d_name,user_name)==0){
		}
	}
	
	return 0;
} */





int command_selector (int socket,char * buffer, int size){
	int output;
	if(strncmp(buffer,REGISTER,size)==0) {
		output  = register_user(socket);
	}
	
	return output;
}



void process_request(int * sd){
    int socket;

    pthread_mutex_lock(&mutex_msg);
    socket = *sd;
    msg_not_copied = 0;
    pthread_cond_signal(&cond_msg);
    pthread_mutex_unlock(&mutex_msg);

    char message[MESSAGE_SIZE];
    int number_received;
	int output;

    if((number_received=readLine(socket,message,MESSAGE_SIZE))==-1){
		   perror("Error when recieving data");
	}
	else {
		output = command_selector(socket,message,number_received);
		char response [4];
		sprintf(response,"%d",output);
		if(write(socket,response,1)==-1){
			perror("Error when sending data");
		}
	}
    close(socket);
    pthread_exit(0);
}


void print_usage() {
	    printf("Usage: server -p puerto \n");
}

int main(int argc, char *argv[]) {
	int  option = 0;
	char port[256]= "";

	while ((option = getopt(argc, argv,"p:")) != -1) {
		switch (option) {
		    	case 'p' : 
				strcpy(port, optarg);
		    		break;
		    	default: 
				print_usage(); 
		    		exit(-1);
		    }
	}
	if (strcmp(port,"")==0){
		print_usage(); 
		exit(-1);
	}


	printf("Port: %s\n", port);

	int accept_socket, data_socket;
	struct sockaddr_in server_addr, client_addr;
	accept_socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	int val;

	setsockopt(accept_socket,SOL_SOCKET,SO_REUSEADDR, (char *)&val,sizeof(int));
	memset((char *)&server_addr,0,sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(4200);

	bind(accept_socket,(struct sockaddr *)&server_addr,sizeof(server_addr));

	socklen_t size = sizeof(client_addr);

	listen(accept_socket,MAX_REQUESTS);

	pthread_attr_t t_attr;
    pthread_t thid;
    pthread_mutex_init(&mutex_msg,NULL);
    pthread_cond_init(&cond_msg,NULL);
    pthread_attr_init(&t_attr);
    pthread_attr_setdetachstate(&t_attr,PTHREAD_CREATE_DETACHED);

	while (1)
	{
		
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
	}
	
	
	return 0;
}
	
