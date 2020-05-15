#include "PeerToPeer.h"

/*****************************************************************************
 *                                                                           *
 *  Server functions to implement the peer to peer functionalities           *
 *  specified. These functions either implement an expected functionality    *
 *  of the system directly (CONNECT, DISCONNECT, etc), or act as             *
 *  auxiliary in other client related function's implementation.             *
 *  Client connection is made throught a manager function, which also        *
 *  processes the return values of the following functions, expect for       *
 *  list_contents and list_users.                                            *
 *                                                                           *
 *  Authors:     Pablo Alejandre Blanco                                      *
 *               Mario Pérez Enríquez                                        *
 *                                                                           *
 *  Last Revision Date:        29/04/2020                                    *
 *                                                                           *
 ****************************************************************************/

pthread_mutex_t data_mutex;
pthread_mutex_t list_mutex;

List * connected_users;

/* Initialize auxiliary structures for p2p connection */
int init_p2p_service () {
	printf("INITIALIZING P2P SOCKET\n");
    connected_users = create_list();

	if(connected_users == NULL){
		return -1;
	}

    pthread_mutex_init(&data_mutex,NULL);
    pthread_mutex_init(&list_mutex,NULL);

	/* Create users folder where persistent data will be stored */
	if(mkdir(USERS_DIRECTORY,0777)==-1){
		if(errno == EEXIST){
			return 0;
		}
		perror("Cannot create user directory");
		return -1;
	}
	return 0;
}

/*******************************************************************************/

int register_user (int socket) {
	char user_name[MAX_SIZE];
	int user_name_length;
	/* Reads data from the client */
	if((user_name_length=readLine(socket,user_name,MAX_SIZE))==-1){
		   perror("Error when recieving data");
		   return 2;
	}
	printf("OPERATION FROM %s\n",user_name);
	int user_registered = isusr_registered(user_name);

	size_t directory_size = user_name_length + 2 + strlen(USERS_DIRECTORY) + 1;
	char * dir_path = (char *)malloc(directory_size * sizeof(char));

	/* Create directory for user if not registered*/
	switch (user_registered)
	{
	case 0:
		sprintf(dir_path,"%s/%s",USERS_DIRECTORY,user_name);

		if(mkdir(dir_path,0777)==-1){
			perror("Cannot create user directory");
			free(dir_path);
			return OTHER_ERROR_2;
		}
		free(dir_path);
		return SUCCESS;
	
	case 1:
		free(dir_path);
		return USER_REGISTERED;
	default:
		free(dir_path);
		return OTHER_ERROR_2;
	}
}

/*******************************************************************************/

int unregister_user(int socket){
	char user_name[MAX_SIZE];
    int user_name_length;
	/* Reads data from client */
	if((user_name_length=readLine(socket,user_name,MAX_SIZE))==-1){
		   perror("Error when recieving data");
		   return OTHER_ERROR_2;
	}
	printf("OPERATION FROM %s\n", user_name);
	int user_registered = isusr_registered(user_name);

	size_t directory_size = user_name_length + 2 + strlen(USERS_DIRECTORY) + 1;
	char * dir_path = (char	*)malloc(directory_size * sizeof(char));

	/* Removes directory and user connection if user registered */
	switch (user_registered)
	{
	case 1:
		sprintf(dir_path,"%s/%s",USERS_DIRECTORY,user_name);
		/* Remove directory entry */
		if((remove_directory(dir_path))==-1) {
			perror("Error when deleting users data");
			free(dir_path);
			return OTHER_ERROR_2;
		}
		/* Remove from connected */
		delete_element(user_name,connected_users);
		free(dir_path);
		return SUCCESS;
	case 0:
		free(dir_path);
		return USER_NOT_REGISTERED;
	default:
		free(dir_path);
		return OTHER_ERROR_2;
	}
} 

/*******************************************************************************/

int connect_user(int socket){
	char user_name[MAX_SIZE];
    int user_name_length;
	/* Recieve user name from client */
	if((user_name_length=readLine(socket,user_name,MAX_SIZE))==-1){
		   perror("Error when recieving data");
		   return OTHER_ERROR_3;
	}
	printf("OPERATION FROM %s\n", user_name);
	/* Recieve port from client */
	char user_port [PORT_SIZE];
	int user_port_length;
	
	if((user_port_length=readLine(socket,user_port,PORT_SIZE))==-1){
		   perror("Error when recieving data");
		   return OTHER_ERROR_3;
	}

	/* Check user is registered */
	int user_registered = isusr_registered(user_name);
	int user_connected;
	
	/* Stablish connection if user registered */
	switch (user_registered)
	{
	case 1:
		user_connected = find_element_byname(connected_users,user_name);
		if(user_connected){
			return USER_CONNECTED;
		}
		char * client_address;

		if ((client_address=get_socket_address(socket))==NULL){
			return OTHER_ERROR_3;
		}

		UserServerData tmp;
		strcpy(tmp.name,user_name);
		strcpy(tmp.port,user_port);
		strcpy(tmp.ip_address,client_address);

		add_element(&tmp,connected_users);
		return SUCCESS;
	case 0:
		return USER_NOT_REGISTERED;
	default:
		return OTHER_ERROR_3;
	}
}

/*******************************************************************************/

int disconnect_user(int socket){
	char user_name[MAX_SIZE];
    int user_name_lenth;
	/* Reads data from client */
	if((user_name_lenth=readLine(socket,user_name,MAX_SIZE))==-1){
		   perror("Error when recieving data");
		   return OTHER_ERROR_3;
	}
	printf("OPERATION FROM %s\n",user_name);
	int user_registered = isusr_registered(user_name);
	int user_connected;
	switch (user_registered)
	{
	case 1:
		user_connected = find_element_byname(connected_users,user_name);
		if(user_connected){
			delete_element(user_name,connected_users);
			return SUCCESS;
		}
		return USER_NOT_CONNECTED;
	case 0:
		return USER_NOT_REGISTERED;
	default:
		return OTHER_ERROR_3;
	}
}

/*******************************************************************************/

int publish_file(int socket){ 
	char user_name[MAX_SIZE];
    int user_name_length;
	
	/* Read data from client in different packets, packet 1, user name */
	if((user_name_length=readLine(socket,user_name,MAX_SIZE))==-1){
		perror("Error when recieving data");
		return OTHER_ERROR_4;
	}
	printf("OPERATION FROM %s\n", user_name);
	char file_name[MAX_SIZE];
    int file_name_length;
	
	/* Read data from client in different packets, packet 2, file name */
	if((file_name_length=readLine(socket,file_name,MAX_SIZE))==-1){
		perror("Error when recieving data");
		return OTHER_ERROR_4;
	}
	char file_description[MAX_SIZE];
	int file_description_length;
	
	/* Read data from client in different packets, packet 3, file description */
	if((file_description_length=readLine(socket,file_description,MAX_SIZE))==-1){
		perror("Error when recieving data");
		return OTHER_ERROR_4;
	}

	/* Check if the user is registered */
	int user_registered = isusr_registered(user_name);
	int user_connected;

	/* Create directory if user is connected and no errors */
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
					strncat(dir_path,file_name,file_name_length);
					if((file=open(dir_path, O_CREAT | O_RDWR,0664))==-1){
						perror("Error when creating file");
						free(dir_path);
						return OTHER_ERROR_4;
					}
					if((write(file,file_description,file_description_length))==-1){
						perror("Error when writing description to file");
						close(file);
						free(dir_path);
						return OTHER_ERROR_4;
					}
					close(file);
					free(dir_path);
					return SUCCESS;
				case 1:
					free(dir_path);
					return FILE_PUBLISHED;
				default:
					free(dir_path);
					return OTHER_ERROR_4;
			}
		}
		return USER_NOT_CONNECTED;
	
	default:
		/* User not registered */ 
		return USER_NOT_REGISTERED;
	}
}

/*******************************************************************************/

int delete_file(int socket){
	char user_name[MAX_SIZE];
    int user_name_length;
	
	/* Read data from client in different packets, packet 1, user name */
	if((user_name_length=readLine(socket,user_name,MAX_SIZE))==-1){
		   perror("Error when recieving data");
		   return OTHER_ERROR_4;
	}
	printf("OPERATION FROM %s\n", user_name);
	char file_name[MAX_SIZE];
    int file_name_length;
	/* Read data from client in different packets, packet 2, file name */
	if((file_name_length=readLine(socket,file_name,MAX_SIZE))==-1){
		   perror("Error when recieving data");
		   return OTHER_ERROR_4;
	}

	size_t dir_path_size = strlen(USERS_DIRECTORY) +1 + user_name_length + 2;
	char * dir_path = (char *)malloc(dir_path_size * sizeof(char));
	sprintf(dir_path,"%s/%s/",USERS_DIRECTORY,user_name);

	int file=file_exists(file_name,dir_path);

	/* Delete file given the file exists, or error otherwise */
	switch (file) {
	case 1:
		strncat(dir_path,file_name,file_name_length);
		if((remove(dir_path))==-1){
			perror("Error when removing file");
			free(dir_path);
			return OTHER_ERROR_4;
		}
		free(dir_path);
		return SUCCESS;
	case 0:
		free(dir_path);
		return FILE_NOT_PUBLISHED;
	default:
		free(dir_path);
		return OTHER_ERROR_4;
	}
}

/*******************************************************************************/

void list_users (int socket){
	char user_name[MAX_SIZE];
    int user_name_length;
	int output;
	int response;

	/* Recieve data from client */
	if((user_name_length=readLine(socket,user_name,MAX_SIZE))==-1){
		perror("Error when recieving data");
		output = OTHER_ERROR_3;
		response = htonl(output);
		/* Server response on error */
		if(send(socket,(int *)&response,sizeof(int),0)==-1){
			perror("Error when sending data");
		}
		pthread_exit(0);
	}
	printf("OPERATION FROM %s\n", user_name);

	/* Get the users data */
	UserServerData * users = get_elements(connected_users);
	if(users == NULL){
		output = OTHER_ERROR_3;
	}
	else{
		output = SUCCESS;
	}
	
	/* Server response on error */
	response = htonl(output);
	if(send(socket,(int *)&response,sizeof(int),0)==-1){
		perror("Error when sending data");
	}

	/* Send back to the client the data of the connected users */
	else if (output == SUCCESS){

		int number_users = list_get_size(connected_users);
		response = htonl(number_users);

		if(send(socket,(int *)&response,sizeof(int),0)==-1){
			perror("Error when sending data");
		}
		else {
			for (int ii = 0; ii < number_users; ii++){
				if(writeLine(socket,users[ii].name,strlen(users[ii].name)+1)==-1){
					perror("Error when sending data");
				}
				if(writeLine(socket,users[ii].ip_address,strlen(users[ii].ip_address)+1)==-1){
					perror("Error when sending data");
				}
				if(writeLine(socket,users[ii].port,strlen(users[ii].port)+1)==-1){
					perror("Error when sending data");
				}
			}
		}
	}
	/* Free memory to avoid leaks */
	free(users);	
	close(socket);

	pthread_exit(0);
}

/*******************************************************************************/

/*List the contents of a user directory */
void list_content (int socket){
	char userop_name [MAX_SIZE];
    int userop_name_length;
	int output;
	int response;

	/* Recieve data from client in separated packets, packet 1, calling user name */
	if((userop_name_length=readLine(socket,userop_name,MAX_SIZE))==-1){
		perror("Error when recieving data");
		output = OTHER_ERROR_4;
		response = htonl(output);
		if(send(socket,(int *)&response,sizeof(int),0)==-1){
			perror("Error when sending data");
		}
		pthread_exit(0);
	}
	printf("OPERATION FROM %s\n", userop_name);

	char user_name [MAX_SIZE];
    int user_name_length;

	/* Recieve data from client in separated packets, packet 2, listed user name */
	if((user_name_length=readLine(socket,user_name,MAX_SIZE))==-1){
		perror("Error when recieving data");
		output = OTHER_ERROR_4;
		response = htonl(output);
		/* Server response on error */
		if(send(socket,(int *)&response,sizeof(int),0)==-1){
			perror("Error when sending data");
		}
		pthread_exit(0);
	}
	DIR * directory;
	struct dirent * d_entry;
	size_t dir_path_size = user_name_length + 2 + strlen(USERS_DIRECTORY) + 1;
	char dir_path [dir_path_size];
	sprintf(dir_path,"%s/%s/",USERS_DIRECTORY,user_name);

	int number_files = 0;

	int user_registered = isusr_registered(user_name);
	/* Check files in directory specified */
	if(user_registered){
		if((directory= opendir(dir_path))==NULL){
			perror("Error when opening users directory");
			closedir(directory);
			output = OTHER_ERROR_4;
		}
		else {
			while ((d_entry = readdir(directory))){
				if(!strcmp(d_entry->d_name, ".") || !strcmp(d_entry->d_name, "..")) {
					continue;
				}
				number_files++;		
			}	
		rewinddir(directory);
		output = SUCCESS;
		}
	}
	else {
		output = NO_CONTENT_TO_LIST;
	}
	
	/* Server response on error */
	response = htonl(output);
	if(send(socket,(int *)&response,sizeof(int),0)==-1){
		perror("Error when sending data");
	}
	/* Server response if succesfully listed */
	else if (output == SUCCESS){

		response = htonl(number_files);

		if(send(socket,(int *)&response,sizeof(int),0)==-1){
			perror("Error when sending data");
		}
		else {
			while ((d_entry = readdir(directory))){
				if(!strcmp(d_entry->d_name, ".") || !strcmp(d_entry->d_name, "..")) {
					continue;
          		}
				size_t data_length = strlen(d_entry->d_name)+1;
				char user_file_name [data_length];
				sprintf(user_file_name,"%s",d_entry->d_name);

				/* Server response on error */
				if(writeLine(socket,user_file_name,data_length)==-1){
					perror("Error when sending data");
				}
			}
		}
	}
	/* Free memory to avoid memory leaks */
	closedir(directory);
	close(socket);

	pthread_exit(0);
}