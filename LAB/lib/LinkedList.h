#ifndef _LINKEDLIST_
#define _LINKEDLIST_
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define MAX_SIZE 256
#define PORT_SIZE 7
#define IP_SIZE 16

typedef struct userver {
  char name [MAX_SIZE];
  char port [PORT_SIZE];
  char ip_address[IP_SIZE];
} UserServerData;

typedef struct node Node;

typedef struct node {
    UserServerData data;
    Node * next;
} Node;


typedef struct list{
    Node * head;
    int size;
} List;

List * create_list();
int list_get_size(List * list);
Node * create_node(UserServerData * data);
void add_element(UserServerData * data, List * list);
void delete_element(char * data, List * list);
void destroy_list(List * list);
UserServerData * get_elements(List * list);
int find_element_byname (List * list,char * data);
UserServerData * get_element_byname (List * list,char * data);
char * get_name_byaddress (List * list, char * ip);
void print_list(List * list);


#endif