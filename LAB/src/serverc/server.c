
#include <getopt.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#define MAX_REQUESTS 10
#define SELF_REPLY 10

#ifdef RPC
#include "PeerToPeerRPC.h"
#else
#include "PeerToPeer.h"				
#endif

#define REGISTER "REGISTER"
#define UNREGISTER "UNREGISTER"
#define CONNECT "CONNECT"
#define DISCONNECT "DISCONNECT"
#define PUBLISH "PUBLISH"
#define DELETE "DELETE"
#define LIST_USERS_LIST "LIST_USERS"
#define LIST_CONTENT_LIST "LIST_CONTENT"


pthread_mutex_t server_mutex;
int msg_not_copied = 1;
pthread_cond_t cond_msg;




int command_selector (int socket,char * buffer){
	int output;
	if(strcmp(buffer,REGISTER)==0) {
		#ifdef RPC
		output = register_user_rpc(socket);
		#else
		output = register_user(socket);
		#endif
	}
	else if(strcmp(buffer,UNREGISTER)==0) {
		#ifdef RPC
		output = unregister_user_rpc(socket);
		#else
		output = unregister_user(socket);
		#endif
	}
	else if(strcmp(buffer,CONNECT)==0){
		#ifdef RPC
		output = connect_user_rpc(socket);
		#else
		output = connect_user(socket);
		#endif
	}
	else if (strcmp(buffer,DISCONNECT)==0){
		#ifdef RPC
		output = disconnect_user_rpc(socket);
		#else
		output = disconnect_user(socket);
		#endif
	}
	else if (strcmp(buffer,PUBLISH)==0) {
		#ifdef RPC
		output = publish_file_rpc(socket);
		#else
		output = publish_file(socket);
		#endif
	}
	else if (strcmp(buffer,DELETE)==0){
		#ifdef RPC
		output = delete_file_rpc(socket);
		#else
		output = delete_file(socket);
		#endif
	}
	else if(strcmp(buffer,LIST_USERS_LIST)==0){
		#ifdef RPC
		list_users_rpc(socket);
		#else
		list_users(socket);
		#endif
		output = SELF_REPLY;
	}
	else if (strcmp(buffer,LIST_CONTENT_LIST)==0){
		#ifdef RPC
		list_content_rpc(socket);
		#else
		list_content(socket);
		#endif
		output = SELF_REPLY;
	}
	return output;
}

void process_request(int * sd){
    int socket;

    pthread_mutex_lock(&server_mutex);
    socket = *sd;
    msg_not_copied = 0;
    pthread_cond_signal(&cond_msg);
    pthread_mutex_unlock(&server_mutex);

    char message[MAX_SIZE];
	int output;
    if((readLine(socket,message,MAX_SIZE))==-1){
		   perror("Error when recieving data");
	}
	else {
		output = command_selector(socket,message);
		if (output != SELF_REPLY){
			int response = htonl(output);
			if(send(socket,(int *)&response,sizeof(int),0)==-1){
				perror("Error when sending data");
			}
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

	#ifdef RPC
	init_p2p_service_rpc();
	#else
	if(init_p2p_service()!=0){
		 perror("Error when creating data structures");
	}
	#endif

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
    pthread_mutex_init(&server_mutex,NULL);
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
        pthread_mutex_lock(&server_mutex);
        while (msg_not_copied)
        {
            pthread_cond_wait(&cond_msg,&server_mutex);
        }
        msg_not_copied = 1;
        pthread_mutex_unlock(&server_mutex);
	}
	
	
	return 0;
}
	
