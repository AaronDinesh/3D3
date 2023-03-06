#include <stdio.h>
#include <stdlib.h>
#include "SLL.h"
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
typedef struct sockaddr_in sockaddr_in;
/*
    LL = Linked List
    SLL = Singly Linked List
    1) Typedef struct with name node. Node contains data of char
       and pointer to another "node"
    2) Create function to add at the FRONT of an LL
    3) Create function that prints the entire LL
        Try recursive function?
    4) Create function that removes node
    5) Create function that deletes an entire list
        Create a recursive version of this
*/

void prepend_Node(Node** start_ptr_ptr, char* identifier, int sockfd, sockaddr_in* remote_conn, time_t seconds){
    Node* newNode_ptr = (Node*)malloc(sizeof(Node));
    newNode_ptr->identifier = malloc(strlen(identifier)*sizeof(char));
    strcpy(newNode_ptr->identifier, identifier);
    newNode_ptr->sockfd = sockfd;
    newNode_ptr->remote_conn = remote_conn;
    newNode_ptr->seconds_since_conn = seconds;
    newNode_ptr->next = *start_ptr_ptr;
    *start_ptr_ptr = newNode_ptr;
}


Node* search_list_recursive_identifier(Node* start_ptr, char* identifier){
    if(!start_ptr){
        return NULL;
    }

    if(strcmp(start_ptr->identifier, identifier) == 0){
        return start_ptr;
    }
    
    return search_list_recursive_identifier(start_ptr->next, identifier);
    
}

Node* search_list_recursive_sockfd(Node* start_ptr, int sockfd){
    if(!start_ptr){
        return NULL;
    }

    if(start_ptr->sockfd == sockfd){
        return start_ptr;
    }
    
    return search_list_recursive_sockfd(start_ptr->next, sockfd);
    
}

void print_list_info(Node** start_ptr_ptr){
    Node* next_ptr = *start_ptr_ptr;
    while(next_ptr){
        printf("Identifier: %s,IP: %s \n", next_ptr->identifier,inet_ntoa(next_ptr->remote_conn->sin_addr));
        next_ptr = next_ptr->next;
    }
}

void print_list_ip(Node** start_ptr_ptr){
    Node* next_ptr = *start_ptr_ptr;
    while(next_ptr){
        printf("IP: %s \n", inet_ntoa(next_ptr->remote_conn->sin_addr));
        next_ptr = next_ptr->next;
    }
}

void print_list_identifier(Node** start_ptr_ptr){
    Node* next_ptr = *start_ptr_ptr;
    while(next_ptr){
        printf("Identifier: %s \n", next_ptr->identifier);
        next_ptr = next_ptr->next;
    }
}


void delete_list(Node** start_ptr_ptr){
    Node* next_ptr = *start_ptr_ptr;
    Node* tmp = NULL;
    while(next_ptr){
        tmp = next_ptr->next;
        free(next_ptr);
        next_ptr = tmp;
    }
}

void delete_list_recursive(Node* start_ptr){
    Node* tmp = NULL;
    if(start_ptr){
        tmp = start_ptr->next;
        free(start_ptr);
        delete_list_recursive(tmp);
    }
}

void delete_node(Node** start_ptr_ptr, char* identifier){
    Node* current_ptr = *start_ptr_ptr;
    Node* prev_ptr = NULL;
    Node* next_ptr = NULL;

    while(current_ptr){
        next_ptr = current_ptr->next;
        if(strcmp(current_ptr->identifier, identifier) == 0){
            if(current_ptr == *start_ptr_ptr){
                *start_ptr_ptr = current_ptr -> next;
                return;
            }
            prev_ptr->next = next_ptr;
            free(current_ptr);
            return;
        }else{
            prev_ptr = current_ptr;
            current_ptr = next_ptr;
        }
    }
}