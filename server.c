#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "SLL.h"

#define SIZE 1024
typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct Node Node;

int list_length = 0;

void write_file(int sockfd);
int add_connection(int new_sockfd, sockaddr_in* remote_addr, Node** client_list);
int server_init(void);

int main(){
    Node* client_list = NULL;

    int sockfd = server_init();

    int new_sock;
    sockaddr_in new_addr;
    socklen_t new_addr_size = sizeof(new_addr);
    new_sock = accept(sockfd, (sockaddr*) &new_addr, &new_addr_size);
    add_connection(new_sock, &new_addr, &client_list);
    
    while(list_length > 0){
        new_sock = accept(sockfd, (sockaddr*) &new_addr, &new_addr_size);
        add_connection(new_sock, &new_addr, &client_list);
        print_list(&client_list);
    }

    // write_file(new_sock);
    return 0;
}

int add_connection(int new_sockfd, sockaddr_in* remote_addr, Node** client_list){
    char* msg = "<IDEN>";
    char buffer[SIZE] = {"\0"};
    send(new_sockfd, msg, sizeof(msg), 0);

    int bytes_recv = recv(new_sockfd, buffer, SIZE, 0);
    while(1){
        bytes_recv = recv(new_sockfd, buffer, SIZE, 0);
        if(bytes_recv <= 0){
            break;
        }
    }

    prepend_Node(client_list, buffer, new_sockfd, remote_addr);
    list_length++;
    memset(buffer, 0, SIZE);
    return 0;
}

void write_file(int sockfd){
    int n;
    FILE* file  = NULL;
    char* filename = "receievd_file.txt";
    char buffer[SIZE];

    file = fopen(filename, "w");

    if(!file){
        perror("Error creating file");
        exit(1);
    }

    while(1){
        n = recv(sockfd, buffer, SIZE, 0);
        if(n <= 0){
            fclose(file);
            return;
        }

        fprintf(file, "%s", buffer);
        memset(buffer, 0, SIZE);
    }   
}

int server_init(void){
    char *ip = "127.0.0.1";
    int port = 10000;
    int e;

    int sockfd;
    sockaddr_in server_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd < 0){
        perror("Error in Socket");
        exit(1);
    }

    printf("Server socket has been created\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    e = bind(sockfd, (struct sockaddr*) &server_addr, (socklen_t) sizeof(server_addr));
    if(e < 0){
        perror("Error in binding");
        exit(1);
    } 

    printf("Binding Sucessful\n");

    e = listen(sockfd, 10);

    if(e == 0){
        printf("Listening...\n");
    }else{
        perror("Max connections reached");
        exit(1);
    }

    return sockfd;
}