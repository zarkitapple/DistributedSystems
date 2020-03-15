#ifndef _LINKEDLIST_
#define _LINKEDLIST_
#include <stdlib.h>
#include <string.h>
#define MAX_SIZE 256
#define PORT_SIZE 8

typedef struct userver {
  char name [MAX_SIZE];
  char port [PORT_SIZE];
} UserServer;

typedef struct node Node;

typedef struct list List;

List * create_list();
Node * create_node(UserServer * data);
void add_element(UserServer * data, List * list);
void delete_element(char * data, List * list);
void destroy_list(List * list);
int find_element (char * data, List * list);

#endif