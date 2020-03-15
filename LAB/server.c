#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "lines.h"
#include "LinkedList.h"
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sys/stat.h>
#include <dirent.h>
#include <linux/limits.h>
#include <fcntl.h>
#define MESSAGE_SIZE 256
#define PORT_SIZE 8
#define MAX_REQUESTS 10
#define USERS_DIRECTORY "/Users"
#define USERS_FILE "files.txt"
#define REGISTER "REGISTER"
#define UNREGISTER "UNREGISTER"
#define CONNECT "CONNECT"
#define DISCONNECT "DISCONNECT"
#define PUBLISH "PUBLISH"


pthread_mutex_t mutex_msg;
int msg_not_copied = 1;
pthread_cond_t cond_msg;

List * connected_users;

int remove_directory(const char *path)
{
   DIR *d = opendir(path);
   size_t path_len = strlen(path);
   int r = -1;

   if (d)
   {
      struct dirent *p;
      r = 0;
      while (!r && (p=readdir(d)))
      {
          int r2 = -1;
          char *buf;
          size_t len;

          /* Skip the names "." and ".." as we don't want to recurse on them. */
          if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
          {
             continue;
          }

          len = path_len + strlen(p->d_name) + 2; 
          buf = malloc(len);

          if (buf)
          {
             struct stat statbuf;

             snprintf(buf, len, "%s/%s", path, p->d_name);

             if (!stat(buf, &statbuf))
             {
                if (S_ISDIR(statbuf.st_mode))
                {
                   r2 = remove_directory(buf);
                }
                else
                {
                   r2 = unlink(buf);
                }
             }
             free(buf);
          }

          r = r2;
      }
      closedir(d);
   }
   if (!r)
   {
      r = rmdir(path);
   }
   return r;
}

int isusr_registered(char * user_name){
	DIR * directory;
	struct dirent * d_entry;
	char dir_path[PATH_MAX];

	getcwd(dir_path,PATH_MAX);
	strcat(dir_path,USERS_DIRECTORY);

	if((directory= opendir(dir_path))==NULL){
		perror("Error when opening users directory");
		closedir(directory);
		return -1;
	}
	while ((d_entry = readdir(directory)))
	{
		if(strcmp(d_entry->d_name,user_name)==0){
			/* USER IS ALREADY REGISTERED */
			closedir(directory);
			return 1;
		}
	}
	/* not registered */
	closedir(directory);
	return 0;
}

int register_user (int socket) {
	char user_name[MESSAGE_SIZE];
    int number_received;
	if((number_received=readLine(socket,user_name,MESSAGE_SIZE))==-1){
		   perror("Error when recieving data");
		   return 2;
	}
	int user_registered = isusr_registered(user_name);
	char dir_path[PATH_MAX];
	switch (user_registered)
	{
	case 0:
		getcwd(dir_path,PATH_MAX);
		strcat(dir_path,USERS_DIRECTORY);
		char user_path[260];
		sprintf(user_path,"/%s",user_name);
		strcat(dir_path,user_path);

		if(mkdir(dir_path,0777)==-1){
			perror("Cannot create user directory");
			return 2;
		}
		return 0;
	
	case 1:
		return 1;
	default:
		return 2;
	}
	/*
	char file_path[11];
	sprintf(file_path,"/%s",USERS_FILE);
	strcat(path,file_path);
	printf("Path users directory name file %s\n",path);

	if ((open(path, O_CREAT | O_RDWR,0664))==-1){
		perror("Cannot create user file");
		return 2;
	}
	*/
}
 
int unregister_user(int socket){
	char user_name[MESSAGE_SIZE];
    int number_received;
	if((number_received=readLine(socket,user_name,MESSAGE_SIZE))==-1){
		   perror("Error when recieving data");
		   return 2;
	}
	int user_registered = isusr_registered(user_name);
	char dir_path[PATH_MAX];
	switch (user_registered)
	{
	case 1:
		getcwd(dir_path,PATH_MAX);
		strcat(dir_path,USERS_DIRECTORY);

		char user_path[260];
		sprintf(user_path,"/%s",user_name);
		strcat(dir_path,user_path);
		/* Remove directory entry */
		if((remove_directory(dir_path))==-1) {
			perror("Error when deleting users data");
			return 2;
		}
		/* remove from connected */
		delete_element(user_name,connected_users);
		return 0;
	case 0:
		return 1;
	default:
		return 2;
	}
} 
int connect_user(int socket){
	char user_name[MESSAGE_SIZE];
    int number_received;
	/* receive user name */
	if((number_received=readLine(socket,user_name,MESSAGE_SIZE))==-1){
		   perror("Error when recieving data");
		   return 3;
	}
	/* receive user port */
	char user_port [PORT_SIZE];
	if((number_received=readLine(socket,user_port,PORT_SIZE))==-1){
		   perror("Error when recieving data");
		   return 3;
	}

	int user_registered = isusr_registered(user_name);
	int user_connected;
	
	switch (user_registered)
	{
	case 1:
		user_connected = find_element(user_name,connected_users);
		if(user_connected){
			return 2;
		}
		UserServer tmp;
		strcpy(tmp.name,user_name);
		strcpy(tmp.port,user_port);
		add_element(&tmp,connected_users);
		return 0;
	case 0:
		return 1;
	default:
		return 3;
	}
}

int disconnect_user(int socket){
	char user_name[MESSAGE_SIZE];
    int number_received;
	if((number_received=readLine(socket,user_name,MESSAGE_SIZE))==-1){
		   perror("Error when recieving data");
		   return 3;
	}
	int user_registered = isusr_registered(user_name);
	int user_connected;
	switch (user_registered)
	{
	case 1:
		user_connected = find_element(user_name,connected_users);
		if(user_connected){
			delete_element(user_name,connected_users);
			return 0;
		}
		return 2;
	case 0:
		return 1;
	default:
		return 3;
	}
}

int command_selector (int socket,char * buffer, int size){
	int output;
	puts("Processing request step 2");
	if(strncmp(buffer,REGISTER,size)==0) {
		output = register_user(socket);
	}
	else if(strncmp(buffer,UNREGISTER,size)==0) {
		output = unregister_user(socket);
	}
	else if(strncmp(buffer,CONNECT,size)==0){
		output = connect_user(socket);
	}
	else if (strncmp(buffer,DISCONNECT,size)==0){
		output = disconnect_user(socket);
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
	puts("Processing request step 1");
    if((number_received=readLine(socket,message,MESSAGE_SIZE))==-1){
		   perror("Error when recieving data");
	}
	else {
		output = command_selector(socket,message,number_received);
		puts("Processing request step 6");
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

	if(!(connected_users = create_list())){
		perror("Error when creating connected users queue");
		return -1;
	}

	int accept_socket, data_socket;
	struct sockaddr_in server_addr, client_addr;
	accept_socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	int val;

	setsockopt(accept_socket,SOL_SOCKET,SO_REUSEADDR, (char *)&val,sizeof(int));
	memset((char *)&server_addr,0,sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	/* change port o given port atoi */
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
		puts("Wait for request");
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
	
