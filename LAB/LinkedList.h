#ifndef _LINKEDLIST_
#define _LINKEDLIST_
#include <stdlib.h>
#include <string.h>
#define MAX_SIZE 256
#define PORT_SIZE 7
#define IP_SIZE 16

typedef struct userver {
  char name [MAX_SIZE];
  char port [PORT_SIZE];
  char ip_address[IP_SIZE];
} UserServer;

typedef struct node Node;

typedef struct list List;

List * create_list();
int list_get_size(List * list);
Node * create_node(UserServer * data);
void add_element(UserServer * data, List * list);
void delete_element(char * data, List * list);
void destroy_list(List * list);
UserServer * get_elements(List * list);
int find_element_byname (List * list,char * data);
UserServer * get_element_byname (List * list,char * data);
char * get_name_byaddress (List * list, char * ip);

#endif