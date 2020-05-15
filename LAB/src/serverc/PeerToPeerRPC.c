#include "PeerToPeerRPC.h"

/*****************************************************************************
 *                                                                           *
 *  Server functions to implement the peer to peer functionalities           *
 *  specified. All the set of functions defined here serve as implementation *
 *  for the system using RPCs.Client connection is made throught a manager   *
 *  function, which also                                                     *
 *  processes the return values of the following functions, expect for       *
 *  list_contents and list_users.                                            *
 *                                                                           *
 *  Authors:     Pablo Alejandre Blanco                                      *
 *               Mario Pérez Enríquez                                        *
 *                                                                           *
 *  Last Revision Date:        17/05/2020                                    *
 *                                                                           *
 ****************************************************************************/

CLIENT *client;

/* Initialize auxiliary client structure for RPC */
void init_p2p_service_rpc () {
    client = clnt_create ("localhost", STORAGE, STORAGEVER, "tcp");
	if (client == NULL) {
		clnt_pcreateerror ("localhost");
		exit (1);
	}
}

/**************************************************************************/

int register_user_rpc (int socket){
	char user_name[MAX_SIZE];
	int user_name_length;
	/* Recieves data from client */
	if((user_name_length=readLine(socket,user_name,MAX_SIZE))==-1){
		   perror("Error when recieving data");
		   return 2;
	}
	int user_registered;
	/* Call the procedure for register after getting data*/
	int ret = register_user_rpc_1(user_name,&user_registered,client);
	if (ret != RPC_SUCCESS) {
		clnt_perror (client, "call failed");
		return 2;
	}
    /* 1 if already registered, 0 if success */
	return user_registered;
}

/**************************************************************************/

int unregister_user_rpc (int socket){
	char user_name[MAX_SIZE];
    int user_name_length;
	/* Recieves data from client */
	if((user_name_length=readLine(socket,user_name,MAX_SIZE))==-1){
		   perror("Error when recieving data");
		   return 2;
	}
	int user_unregistered;
	/* Call the procedure for unregister after getting data*/
	int ret = unregister__user_rpc_1(user_name,&user_unregistered,client);
	if (ret != RPC_SUCCESS) {
		clnt_perror (client, "call failed");
		return 2;
	}
	return user_unregistered;
}

/**************************************************************************/

int connect_user_rpc (int socket){
	char user_name[MAX_SIZE];
    int user_name_length;
	/* Recieves data from client */
	if((user_name_length=readLine(socket,user_name,MAX_SIZE))==-1){
		   perror("Error when recieving data");
		   return 3;
	}

	/* Receive port */
	char user_port [PORT_SIZE];
	int user_port_length;
	
	if((user_port_length=readLine(socket,user_port,PORT_SIZE))==-1){
		   perror("Error when recieving data");
		   return 3;
	}

	/* Get client network data */
	char * client_address;

	if ((client_address=get_socket_address(socket))==NULL){
		return 3;
	}

	/* Get data for RPC as struct (See XDR) */
	connect_arguments arguments;
	arguments.name = malloc(MAX_SIZE);
	strcpy(arguments.name,user_name);
	arguments.port = malloc(PORT_SIZE);
	strcpy(arguments.port,user_port);;
	arguments.ip = malloc(IP_SIZE);
	strcpy(arguments.ip,client_address);

	int user_connected;

	/* Call the procedure for connecting after getting data*/
	int ret = connect_user_rpc_1(arguments,&user_connected,client);
	if (ret != RPC_SUCCESS) {
		clnt_perror (client, "call failed");
		/* Free memory to avoid leaks */
		free(arguments.name);
		free(arguments.port);
		free(arguments.ip);
		return 3;
	}
	/* Free memory to avoid leaks */
	free(arguments.name);
	free(arguments.port);
	free(arguments.ip);
	return user_connected;
}

/****************************************************************************/

int disconnect_user_rpc(int socket){
	char user_name[MAX_SIZE];
    int user_name_lenth;
	/* Recieves data from client */
	if((user_name_lenth=readLine(socket,user_name,MAX_SIZE))==-1){
		   perror("Error when recieving data");
		   return 3;
	}
	int user_disconnected;
	/* Call the procedure for disconnecting after getting data*/
	int ret = disconnect_user_rpc_1(user_name,&user_disconnected,client);
	if (ret != RPC_SUCCESS) {
		clnt_perror (client, "call failed");
		return 3;
	}
	return user_disconnected;
}

/*************************************************************************************/

int publish_file_rpc (int socket){
	char user_name[MAX_SIZE];
    int user_name_length;
	
	/* Recieves data from client, spread in three packets, packet 1, user name */
	if((user_name_length=readLine(socket,user_name,MAX_SIZE))==-1){
		perror("Error when recieving data");
		return 4;
	}
	char file_name[MAX_SIZE];
    int file_name_length;
	
	/* Recieves data from client, spread in three packets, packet 2, file name */
	if((file_name_length=readLine(socket,file_name,MAX_SIZE))==-1){
		perror("Error when recieving data");
		return 4;
	}
	char file_description[MAX_SIZE];
	int file_description_length;

	/* Recieves data from client, spread in three packets, packet 3, file description */
	if((file_description_length=readLine(socket,file_description,MAX_SIZE))==-1){
		perror("Error when recieving data");
		return 4;
	}

	/* Get data for RPC as struct (See XDR) */
	publish_arguments arguments;
	arguments.name = malloc(MAX_SIZE);
	strcpy(arguments.name,user_name);
	arguments.file_name = malloc(MAX_SIZE);
	strcpy(arguments.file_name,file_name);
	arguments.file_description = malloc(MAX_SIZE);
	strcpy(arguments.file_description,file_description);

	int file_published;
	/* Call the procedure for publishing after getting data*/
	int ret = publish_file_rpc_1(arguments,&file_published,client);
	if (ret != RPC_SUCCESS) {
		clnt_perror (client, "call failed");
		/* Free memory to avoid leaks */
		free(arguments.name);
		free(arguments.file_name);
		free(arguments.file_description);
		return 4;
	}
	/* Free memory to avoid leaks */
	free(arguments.name);
	free(arguments.file_name);
	free(arguments.file_description);
	return file_published;
}

/***************************************************************************************/

int delete_file_rpc( int socket) {
	char user_name[MAX_SIZE];
    int user_name_length;
	/* Recieves data from client, spread in two packets, packet 1, user name */
	if((user_name_length=readLine(socket,user_name,MAX_SIZE))==-1){
		   perror("Error when recieving data");
		   return 4;
	}
	char file_name[MAX_SIZE];
    int file_name_length;
	/* Recieves data from client, spread in two packets, packet 2, file name */
	if((file_name_length=readLine(socket,file_name,MAX_SIZE))==-1){
		   perror("Error when recieving data");
		   return 4;
	}
	
	/* Get data for RPC as struct (See XDR) */
	delete_arguments arguments;
	arguments.name = malloc(MAX_SIZE);
	strcpy(arguments.name,user_name);
	arguments.file_name = malloc(MAX_SIZE);
	strcpy(arguments.file_name,file_name);
	

	int file_deleted;
	/* Call the procedure for deleting after getting data*/
	int ret = delete_file_rpc_1(arguments,&file_deleted,client);
	if (ret != RPC_SUCCESS) {
		clnt_perror (client, "call failed");
		/* Free memory to avoid leaks */
		free(arguments.name);
		free(arguments.file_name);
		return 4;
	}
	/* Free memory to avoid leaks */
	free(arguments.name);
	free(arguments.file_name);
	return file_deleted;	
}

/********************************************************************************/

void list_users_rpc (int socket){
	char user_name[MAX_SIZE];
    int user_name_length;
	int output;
	int response;

	/* Recieve data from client */
	if((user_name_length=readLine(socket,user_name,MAX_SIZE))==-1){
		perror("Error when recieving data");
		output = 3;
		response = htonl(output);
		/* Server response on error */
		if(send(socket,(int *)&response,sizeof(int),0)==-1){
			perror("Error when sending data");
		}
		pthread_exit(0);
	}

	UserList * users_list = malloc(sizeof(UserList));

	/* Call the procedure for listing users after getting data*/ 
	int ret = list_users_1(user_name,users_list,client);
	if (ret != RPC_SUCCESS) {
		clnt_perror (client, "call failed");
		output = 3;
	}
	if(atoi(users_list->UserList_val[0].name)== -1){
		output = 3;
	}
	else {
		output = 0;
	}
	response = htonl(output);
	/* Server response on error */
	if(send(socket,(int *)&response,sizeof(int),0)==-1){
		perror("Error when sending data");
	}

	/*If success, send data to client */
	else if (output == 0){

		int number_users = users_list->UserList_len;
		response = htonl(number_users);
		/* Server response on error */
		if(send(socket,(int *)&response,sizeof(int),0)==-1){
			perror("Error when sending data");
		}
		else {

            for (size_t ii = 0; ii < number_users; ii++)
            {
				/* Server data response per user and user data */
               if(writeLine(socket,users_list->UserList_val[ii].name,strlen(users_list->UserList_val[ii].name)+1)==-1){
					perror("Error when sending data");
				}
				if(writeLine(socket,users_list->UserList_val[ii].ip,strlen(users_list->UserList_val[ii].ip)+1)==-1){
					perror("Error when sending data");
				}
				if(writeLine(socket,users_list->UserList_val[ii].port,strlen(users_list->UserList_val[ii].port)+1)==-1){
					perror("Error when sending data");
				}
            }
		}
	}
	pthread_exit(0);
}

/*****************************************************************************************************************************/

void list_content_rpc (int socket){
	char userop_name [MAX_SIZE];
    int userop_name_length;
	int output;
	int response;

	/* Recieve data from client in separated packets, packet 1, calling user name */
	if((userop_name_length=readLine(socket,userop_name,MAX_SIZE))==-1){
		perror("Error when recieving data");
		output = 4;
		response = htonl(output);
		if(send(socket,(int *)&response,sizeof(int),0)==-1){
			perror("Error when sending data");
		}
		pthread_exit(0);
	}
	char user_name [MAX_SIZE];
    int user_name_length;

	/* Recieve data from client in separated packets, packet 2, listed user name */
	if((user_name_length=readLine(socket,user_name,MAX_SIZE))==-1){
		perror("Error when recieving data");
		output = 4;
		response = htonl(output);
		if(send(socket,(int *)&response,sizeof(int),0)==-1){
			perror("Error when sending data");
		}
		pthread_exit(0);
	}

	TitleList * title_list = malloc(sizeof(TitleList));
	
	/* Call the procedure for listing content after getting data*/ 
	int ret = list_content_1(userop_name,user_name,title_list,client);
	if (ret != RPC_SUCCESS) {
		clnt_perror (client, "call failed");
		output = 4;
	}
	if (title_list->TitleList_len == 0){
		output = 0;
	}
	else if(atoi(title_list->TitleList_val[0].file_name)== -1){
		output = 4;
	}
	else if (atoi(title_list->TitleList_val[0].file_name) == -2)
	{
		output = 3;
	}
	else {
		output = 0;
	}
	response = htonl(output);

	/* Server response on error */
	if(send(socket,(int *)&response,sizeof(int),0)==-1){
		perror("Error when sending data");
	}
	/*If success, send data to client */
	else if (output == 0){
		int number_files = title_list->TitleList_len;
		response = htonl(number_files);

		/* Server response on error */
		if(send(socket,(int *)&response,sizeof(int),0)==-1){
			perror("Error when sending data");
		}
		else {

            for (size_t ii = 0; ii < number_files; ii++)
            {
				/* Server data response per file */
                if(writeLine(socket,title_list->TitleList_val[ii].file_name,strlen(title_list->TitleList_val[ii].file_name)+1)==-1){
					perror("Error when sending data");
				}
            }
		}
	}
	pthread_exit(0);
}