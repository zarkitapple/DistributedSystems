#include <unistd.h>

int send_msg(int socket, char *mensaje, int longitud);
int recieve_msg(int socket, char *mensaje, int longitud);
ssize_t readLine(int fd, void *buffer, size_t n);
ssize_t writeLine(int fd, void *buffer, size_t n);