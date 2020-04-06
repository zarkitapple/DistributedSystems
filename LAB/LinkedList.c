#include "LinkedList.h"
#include <stdio.h>

typedef struct node {
    UserServer data;
    Node * next;
} Node;

typedef struct list{
    Node * head;
    int size;
} List;


List * create_list() {
    List * list = malloc(sizeof(List));
    if(!list){
        return NULL;
    }
    list->head = NULL;
    list->size = 0;
    return list;
}
int list_get_size(List * list) {
  return list->size;
}
Node * create_node(UserServer * data){
  Node * newNode = malloc(sizeof(Node));
  if (!newNode) {
    return NULL;
  }
  memcpy((UserServer *)&newNode->data,(UserServer *)data,sizeof(UserServer));
  newNode->next = NULL;
  return newNode;
}

void add_element(UserServer * data, List * list){
  Node * current = NULL;
  if(list->head == NULL){
    list->head = create_node(data);
    list->size++;
  }
  else {
    current = list->head; 
    while (current->next!=NULL){
      current = current->next;
    }
    current->next = create_node(data);
    list->size++;
  }
}
void delete_element(char * data, List * list){
  Node * current = list->head;            
  Node * previous = current;           
  while(current != NULL){           
    if(strcmp(current->data.name,data)==0){      
      previous->next = current->next;
      list->size--;
      if(current == list->head)
        list->head = current->next;
      free(current);
      return;
    }                               
    previous = current;             
    current = current->next;      
  }                                
}  


UserServer * get_elements(List * list){
  Node * current = list->head;
  UserServer * users = malloc(sizeof(UserServer)*list->size);
  int ii = 0;
  while(current!=NULL){
      users[ii] = current->data;
      current = current->next;
      ii++;
  }    
    return users; 
}
int find_element_byname (List * list,char * data){
    Node * current = list->head;     
    while(current!=NULL){
        if(strcmp(current->data.name,data)==0){
            return 1;
        }
        current = current->next;
    }    
    return 0;   
    
}
UserServer * get_element_byname (List * list,char * name){
    Node * current = list->head;     
    while(current!=NULL){
        if(strcmp(current->data.name,name)==0){
            puts("Tre");
            return &current->data;
        }
        current = current->next;
    }    
    return NULL;   
    
}

char * get_name_byaddress (List * list, char * ip){
    Node * current = list->head;
    char * name = (char *)malloc(MAX_SIZE * sizeof(char));
    while(current!=NULL){
        if(strcmp(current->data.ip_address,ip)==0){
            strcpy(name,current->data.name);
            return name;
        }
        current = current->next;
    }    
    return NULL;  
}


void destroy_list(List * list){
  Node * current = list->head;
  Node * next = current;
  while(current != NULL){
    next = current->next;
    free(current);
    current = next;
  }
  free(list);
}