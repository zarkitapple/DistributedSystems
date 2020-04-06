#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <netinet/in.h>
#include "lines.h"
#include "LinkedList.h"
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#define MAX_REQUESTS 10
#define USERS_DIRECTORY "./Users"
#define REGISTER "REGISTER"
#define UNREGISTER "UNREGISTER"
#define CONNECT "CONNECT"
#define DISCONNECT "DISCONNECT"
#define PUBLISH "PUBLISH"
#define DELETE "DELETE"
#define LIST_USERS "LIST_USERS"
#define LIST_CONTENT "LIST_CONTENT"

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

char * get_socket_address(int socket){
	/* Socket structure to store the socket address */
		struct sockaddr_in client_addr;
		memset((char *)&client_addr,0,sizeof(client_addr));
		socklen_t size = sizeof(client_addr);
		if((getsockname(socket,(struct sockaddr *)&client_addr,&size ))==-1){
			perror("Cannot get socket address");
			return NULL;
		}
		//char * client_address = (char *)malloc(IP_SIZE);
		char * client_address = inet_ntoa(client_addr.sin_addr);
		return client_address;
}

int file_exists (char * file_name, char * directory_path){
	DIR * directory;
	struct dirent * d_entry;

	if((directory= opendir(directory_path))==NULL){
		perror("Error when opening users directory");
		closedir(directory);
		return -1;
	}
	while ((d_entry = readdir(directory)))
	{
		if(strcmp(d_entry->d_name,file_name)==0){
			/* file exists */
			closedir(directory);
			return 1;
		}
	}
	/* not exits */
	closedir(directory);
	return 0;
}


int isusr_registered(char * user_name){
	DIR * directory;
	struct dirent * d_entry;
	char * dir_path = USERS_DIRECTORY;

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
	char user_name[MAX_SIZE];
	int user_name_length;
	if((user_name_length=readLine(socket,user_name,MAX_SIZE))==-1){
		   perror("Error when recieving data");
		   return 2;
	}
	int user_registered = isusr_registered(user_name);

	size_t directory_size = user_name_length + 2 + strlen(USERS_DIRECTORY) + 1;
	char * dir_path = (char *)malloc(directory_size * sizeof(char));

	switch (user_registered)
	{
	case 0:
		sprintf(dir_path,"%s/%s",USERS_DIRECTORY,user_name);

		if(mkdir(dir_path,0777)==-1){
			perror("Cannot create user directory");
			free(dir_path);
			return 2;
		}
		free(dir_path);
		return 0;
	
	case 1:
		free(dir_path);
		return 1;
	default:
		free(dir_path);
		return 2;
	}
}
 
int unregister_user(int socket){
	char user_name[MAX_SIZE];
    int user_name_length;
	if((user_name_length=readLine(socket,user_name,MAX_SIZE))==-1){
		   perror("Error when recieving data");
		   return 2;
	}
	int user_registered = isusr_registered(user_name);

	size_t directory_size = user_name_length + 2 + strlen(USERS_DIRECTORY) + 1;
	char * dir_path = (char	*)malloc(directory_size * sizeof(char));

	switch (user_registered)
	{
	case 1:
		sprintf(dir_path,"%s/%s",USERS_DIRECTORY,user_name);
		/* Remove directory entry */
		if((remove_directory(dir_path))==-1) {
			perror("Error when deleting users data");
			free(dir_path);
			return 2;
		}
		/* remove from connected */
		delete_element(user_name,connected_users);
		free(dir_path);
		return 0;
	case 0:
		free(dir_path);
		return 1;
	default:
		free(dir_path);
		return 2;
	}
} 
int connect_user(int socket){
	char user_name[MAX_SIZE];
    int user_name_length;
	/* receive user name */
	if((user_name_length=readLine(socket,user_name,MAX_SIZE))==-1){
		   perror("Error when recieving data");
		   return 3;
	}
	/* receive user port */
	char user_port [PORT_SIZE];
	int user_port_length;
	
	if((user_port_length=readLine(socket,user_port,PORT_SIZE))==-1){
		   perror("Error when recieving data");
		   return 3;
	}

	int user_registered = isusr_registered(user_name);
	int user_connected;
	switch (user_registered)
	{
	case 1:
		user_connected = find_element_byname(connected_users,user_name);
		if(user_connected){
			return 2;
		}
		char * client_address;

		if ((client_address=get_socket_address(socket))==NULL){
			return 3;
		}

		UserServer tmp;
		strcpy(tmp.name,user_name);
		strcpy(tmp.port,user_port);
		strcpy(tmp.ip_address,client_address);

		add_element(&tmp,connected_users);
		return 0;
	case 0:
		return 1;
	default:
		return 3;
	}
}

int disconnect_user(int socket){
	char user_name[MAX_SIZE];
    int user_name_lenth;
	if((user_name_lenth=readLine(socket,user_name,MAX_SIZE))==-1){
		   perror("Error when recieving data");
		   return 3;
	}
	int user_registered = isusr_registered(user_name);
	int user_connected;
	switch (user_registered)
	{
	case 1:
		user_connected = find_element_byname(connected_users,user_name);
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

int publish_file(int socket){ 
	char user_name[MAX_SIZE];
    int user_name_length;
	
	if((user_name_length=readLine(socket,user_name,MAX_SIZE))==-1){
		perror("Error when recieving data");
		return 4;
	}
	char file_name[MAX_SIZE];
    int file_name_length;
	
	if((file_name_length=readLine(socket,file_name,MAX_SIZE))==-1){
		perror("Error when recieving data");
		return 4;
	}
	char file_description[MAX_SIZE];
	int file_description_length;
	if((file_description_length=readLine(socket,file_description,MAX_SIZE))==-1){
		perror("Error when recieving data");
		return 4;
	}


	int user_registered = isusr_registered(user_name);
	int user_connected;


	switch (user_registered)
	{
	case 1:
		user_connected = find_element_byname(connected_users,user_name);
		if(user_connected){
			/*./Userddirectory/UserName/*/
			size_t dir_path_size = strlen(USERS_DIRECTORY) +1 + user_name_length + 2;
			char * dir_path = (char *)malloc(dir_path_size * sizeof(char));
			sprintf(dir_path,"%s/%s/",USERS_DIRECTORY,user_name);

			int file=file_exists(file_name,dir_path);
			switch(file) {
				case 0:
					/*./Userddirectory/UserName/*/
					strncat(dir_path,file_name,file_name_length+1);
					if((file=open(dir_path, O_CREAT | O_RDWR,0664))==-1){
						perror("Error when creating file");
						free(dir_path);
						return 4;
					}
					if((write(file,file_description,file_description_length))==-1){
						perror("Error when writing description to file");
						close(file);
						free(dir_path);
						return 4;
					}
					close(file);
					free(dir_path);
					return 0;
				case 1:
					free(dir_path);
					return 3;
				default:
					free(dir_path);
					return 4;
			}
		}
		return 2;
	
	default:
		/* User not registered */ 
		return 1;
	}
}

int delete_file(int socket){
	char user_name[MAX_SIZE];
    int user_name_length;
	if((user_name_length=readLine(socket,user_name,MAX_SIZE))==-1){
		   perror("Error when recieving data");
		   return 4;
	}
	char file_name[MAX_SIZE];
    int file_name_length;
	if((file_name_length=readLine(socket,file_name,MAX_SIZE))==-1){
		   perror("Error when recieving data");
		   return 4;
	}
	int user_registered = isusr_registered(user_name);
	int user_connected;
	/* Check that we are connected */
	switch (user_registered){
		case 1:
			user_connected = find_element_byname(connected_users,user_name);
			if(user_connected){
				size_t dir_path_size = strlen(USERS_DIRECTORY) +1 + user_name_length + 2;
				char * dir_path = (char *)malloc(dir_path_size * sizeof(char));
				sprintf(dir_path,"%s/%s/",USERS_DIRECTORY,user_name);

				int file=file_exists(file_name,dir_path);
				switch (file) {
				case 1:
					/* File exists */
					strncat(dir_path,file_name,file_name_length+1);
					if((remove(dir_path))==-1){
						perror("Error when removing file");
						free(dir_path);
						return 4;
					}
					free(dir_path);
					return 0;
				case 0:
					free(dir_path);
					return 3;
				default:
					free(dir_path);
					return 4;
				}
			}
			return 2;
		default:
			return 1;
	}
}
int isusr_connected(int socket,char * user_name){
	char * client_address;
	if ((client_address=get_socket_address(socket))==NULL){
			return 0;
	}
	char * user_connected = get_name_byaddress(connected_users,client_address);
	if(user_connected!=NULL){
		strcpy(user_name,user_connected);
		free(user_connected);
		return 1;
	}
	free(user_connected);
	return 0;
}
void list_users (int socket){
	int list_size = list_get_size(connected_users);
	char * user_name = (char *) malloc(MAX_SIZE * sizeof(char));
	int user_connected = isusr_connected(socket,user_name);
	int user_registered;
	UserServer * users = NULL;
	char ** users_data = NULL;
	size_t * users_data_length = NULL;
	int output;
	switch (user_connected)
	{
	case 1:
		user_registered = isusr_registered(user_name);
		if(user_registered){
			users = get_elements(connected_users);
			if(users == NULL){
				output = 3;
				break;
			}
			users_data = malloc(list_size * sizeof(char *));
			if(users_data == NULL){
				output = 3;
				break;
			}
			users_data_length = malloc( list_size * sizeof(size_t));
			if(users_data_length == NULL){
				output = 3;
				break;
			}
			for(int ii = 0; ii<list_size ; ii++){
				size_t data_length = strlen(users[ii].name) + strlen(users[ii].port) + strlen(users[ii].ip_address) + 3;
				users_data_length[ii] = data_length;
				users_data[ii] = (char *)malloc(data_length * sizeof(char));
				if(users_data[ii] == NULL){
					output = 3;
					break;
				}
				sprintf(users_data[ii],"%s %s %s",users[ii].name,users[ii].ip_address,users[ii].port);
			}
			output = 0;
			break;
		}
		output = 1;
		break;
	default:
		output = 2;
	}
	int	response = htonl(output);
	if(send(socket,(int *)&response,sizeof(int),0)==-1){
		perror("Error when sending data");
	}
	else if(output == 0){
		response = htonl(list_size);
		if(send(socket,(int *)&response,sizeof(int),0)==-1){
			perror("Error when sending data");
		}
		else {
			for(int ii = 0; ii<list_size ; ii++){
				if(writeLine(socket,users_data[ii],users_data_length[ii])==-1){
					perror("Error when sending data");
				}
			}
		}
	}
	free(user_name);
	free(users);
	free(users_data_length);
	for(int ii = 0; ii<list_size ; ii++){
		free(users_data[ii]);
	}
	free(users_data);	
	close(socket);
	pthread_exit(0);
}

void list_content (int socket){
	char user_name[MAX_SIZE];
    int user_name_length;
	/* receive user name */
	if((user_name_length=readLine(socket,user_name,MAX_SIZE))==-1){
		perror("Error when recieving data");
		//return 4;
	}
	char * userop_name = (char *) malloc(MAX_SIZE * sizeof(char));
	int user_connected = isusr_connected(socket,userop_name);
	int user_registered;
	int remote_registered;
	int user_remote_files = 0;
	int output;
	size_t * users_data_length = NULL;
	char ** users_data = NULL;
	switch (user_connected)
	{
	case 1:
		user_registered = isusr_registered(userop_name);
		if (user_registered){
			remote_registered = isusr_registered(user_name);
			if(remote_registered){
				DIR * directory;
				struct dirent * d_entry;
				size_t dir_path_size = user_name_length + 2 + strlen(USERS_DIRECTORY) + 1;
				char * dir_path = (char *)malloc(dir_path_size * sizeof(char));
				if(dir_path == NULL){
					output = 4;
					break;
				}
				/* ./UsersDirectory/username/ */
				sprintf(dir_path,"%s/%s/",USERS_DIRECTORY,user_name);

				if((directory= opendir(dir_path))==NULL){
					perror("Error when opening users directory");
					free(dir_path);
					closedir(directory);
					output = 4;
					break;
				}
				while ((d_entry = readdir(directory))){
						if(!strcmp(d_entry->d_name, ".") || !strcmp(d_entry->d_name, "..")) {
							continue;
          				}
						user_remote_files++;
								
				}
				closedir(directory);
				if((directory= opendir(dir_path))==NULL){
					perror("Error when opening users directory");
					free(dir_path);
					closedir(directory);
					output = 4;
					break;
				}
				users_data_length = malloc( user_remote_files* sizeof(size_t));
				if(users_data_length == NULL){
					output = 4;
					break;
				}

				users_data = malloc(user_remote_files * sizeof(char *));
				if(users_data == NULL){
					output = 4;
					break;
				}

				int ii = 0;
				while ((d_entry = readdir(directory))){
						if(!strcmp(d_entry->d_name, ".") || !strcmp(d_entry->d_name, "..")) {
							continue;
          				}
						size_t data_length = strlen(d_entry->d_name)+1;
						users_data_length[ii] = data_length;
						users_data[ii] = (char *)malloc(data_length * sizeof(char));
						if(users_data[ii] == NULL){
							output = 4;
							break;
						}
						sprintf(users_data[ii],"%s",d_entry->d_name);
						ii++;
								
				}
				free(dir_path);
				closedir(directory);
				output = 0;
				break;
			}
			output = 3;
			break;
		}
		output = 1;
		break;
	
	default:
		output = 2;
		break;
	}
	int response = htonl(output);
	if(send(socket,(int *)&response,sizeof(int),0)==-1){
		perror("Error when sending data");
	}
	else if(output == 0){
		response = htonl(user_remote_files);
		if(send(socket,(int *)&response,sizeof(int),0)==-1){
			perror("Error when sending data");
		}
		else {
			for (int ii = 0; ii < user_remote_files; ii++){
				if(writeLine(socket,users_data[ii],users_data_length[ii])==-1){
					perror("Error when sending data");
				}
			}
		}

		
	}
	free(userop_name);
	free(users_data_length);
	for(int ii = 0; ii<user_remote_files; ii++){
		free(users_data[ii]);
	}
	free(users_data);
	close(socket);
	pthread_exit(0);
}


int command_selector (int socket,char * buffer){
	int output;
	if(strcmp(buffer,REGISTER)==0) {
		output = register_user(socket);
	}
	else if(strcmp(buffer,UNREGISTER)==0) {
		output = unregister_user(socket);
	}
	else if(strcmp(buffer,CONNECT)==0){
		output = connect_user(socket);
	}
	else if (strcmp(buffer,DISCONNECT)==0){
		output = disconnect_user(socket);
	}
	else if (strcmp(buffer,PUBLISH)==0) {
		output = publish_file(socket);
	}
	else if (strcmp(buffer,DELETE)==0){
		output = delete_file(socket);
	}
	else if(strcmp(buffer,LIST_USERS)==0){
		list_users(socket);
	}
	else if (strcmp(buffer,LIST_CONTENT)==0){
		list_content(socket);
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

    char message[MAX_SIZE];
	int output;
    if((readLine(socket,message,MAX_SIZE))==-1){
		   perror("Error when recieving data");
	}
	else {
		output = command_selector(socket,message);
		int response = htonl(output);
		if(send(socket,(int *)&response,sizeof(int),0)==-1){
			perror("Error when sending data");
		}
	}
    close(socket);
    pthread_exit(0);
}


void print_usage() {
	    printf("Usage: server -p puerto \n");
}

int main(int argc, char ** argv) {
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
	
	struct hostent * host;
	struct in_addr in;
	host = gethostbyname("localhost");
	memcpy(&in.s_addr,*(host->h_addr_list),sizeof(in.s_addr));

	printf("Init server %s:%s\n",inet_ntoa(in) ,port);

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
	server_addr.sin_port = htons(atoi(port));

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
	
