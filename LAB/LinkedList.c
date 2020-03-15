#include "LinkedList.h"


typedef struct node {
    UserServer data;
    Node * next;
} Node;

typedef struct list{
    Node * head;
} List;


List * create_list() {
    List * list = malloc(sizeof(List));
    if(!list){
        return NULL;
    }
    list->head =NULL;
    return list;
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
  }
  else {
    current = list->head; 
    while (current->next!=NULL){
      current = current->next;
    }
    current->next = create_node(data);
  }
}
void delete_element(char * data, List * list){
  Node * current = list->head;            
  Node * previous = current;           
  while(current != NULL){           
    if(strcmp(current->data.name,data)==0){      
      previous->next = current->next;
      if(current == list->head)
        list->head = current->next;
      free(current);
      return;
    }                               
    previous = current;             
    current = current->next;        
  }                                 
}  

int find_element (char * data, List * list){
    Node * current = list->head;     
    while(current!=NULL){
        if(strcmp(current->data.name,data)==0){
            return 1;
        }
        current = current->next;
    }    
    return 0;   
    
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