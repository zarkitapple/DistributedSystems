#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "read_line.h"
#define BUFFER_SIZE 256
/* TCP using stream sockets */
int main(int argc, char const *argv[])
{
    int server_socket;
    struct sockaddr_in server_addr;
    struct hostent * host;

    if(argc<2){
        return -1;
    }

    if((server_socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==-1){
        perror("Error when creating socket");
        return -1;
    }

    memset((char *)&server_addr,0,sizeof(server_addr));
    host = gethostbyname(argv[1]);

    memcpy(&(server_addr.sin_addr), *(host->h_addr_list),host->h_length);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4200);

    connect(server_socket,(struct sockaddr *)&server_addr,sizeof(server_addr));

    char message[BUFFER_SIZE];
    char message_rec[BUFFER_SIZE];
    int err=0;
    int n;

    while (err != -1) {
		n = readLine(0, message, BUFFER_SIZE);
        
		if (n!=-1){
            if ((send_msg(server_socket,message,n))==-1){
                puts("Error when sending data");
            }
            if(strncmp(message,"EXIT",5)==0){
               break;
            }
            if ((recv_msg(server_socket,message_rec,n))==-1){
                puts("Error when receiving data");
            }
            write(1, message_rec, n);
            puts("");
        }
			
	}
    /*
    strcpy(message_send,"Hello");
    int number_sent = send(server_socket, message_send, BUFFER_SIZE,0);
    printf("Number sent %d\n", number_sent);

    int number_received = recv(server_socket, message_receive,BUFFER_SIZE,0);
    printf("Number received %d\n", number_received);
    printf("Message %s\n", message_receive);
    */
    close(server_socket);

    return 0;
}
