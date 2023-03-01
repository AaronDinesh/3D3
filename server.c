#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include "SLL.h"
#include <poll.h>

#define SIZE 1024
typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct Node Node;

int list_length = 0;
int add_connection(int new_sockfd, sockaddr_in* remote_addr, Node** client_list);
int export_connection_list(Node** client_list);
int update_connection_list(char* identifier, sockaddr_in* remote_addr);
void send_connection_list(int sockfd);
int server_init(void);
int guard(int n, char * err) { if (n == -1) { perror(err); exit(1); } return n;}
void receive(int new_sockfd, char* buffer);

int main(){
    Node* client_list = NULL;
    int sockfd = server_init();
    int i = 0;

    int new_sock;
    sockaddr_in new_addr;
    socklen_t new_addr_size = sizeof(new_addr);
    // new_sock = accept(sockfd, (sockaddr*) &new_addr, &new_addr_size);
    // add_connection(new_sock, &new_addr, &client_list);

    while(list_length >= 0){
        new_sock = accept(sockfd, (sockaddr*) &new_addr, &new_addr_size);
        if(errno == EWOULDBLOCK && new_sock == -1){
            sleep(1);
        }else{
            printf("Adding new connection\n");
            add_connection(new_sock, &new_addr, &client_list);
            //print_list_ip(&client_list);
            list_length++;
        }
    }
    return 0;
}

int add_connection(int new_sockfd, sockaddr_in* remote_addr, Node** client_list){
    char* msg = "<IDEN>";
    char buffer[SIZE] = {"\0"};
    send(new_sockfd, msg, sizeof(msg), 0);

    receive(new_sockfd, buffer);

    printf("Got Identifier: %s\n", buffer);

    printf("Going to search list\n");
    if(!search_list_recursive_identifier(*(client_list), buffer)){
        printf("Client is not in list. Adding Client\n");
        prepend_Node(client_list, buffer, new_sockfd, remote_addr);
        list_length++;
        update_connection_list(buffer, remote_addr);
    }

    printf("Exporting and sending connection list\n");
    export_connection_list(client_list);

    send_connection_list(new_sockfd);
    memset(buffer, 0, SIZE);
    return 0;
}

int export_connection_list(Node** client_list){
    char* file_name = "connection_list_export.txt";
    FILE* fp = fopen(file_name, "w");
    if(!fp){
        perror("Error opening connection_list.txt");
        exit(1);
    }

    Node* next_ptr = *client_list;
    while(next_ptr){
        fprintf(fp, "%s,%s\n", next_ptr->identifier,inet_ntoa(next_ptr->remote_conn->sin_addr));
        next_ptr = next_ptr->next;
    }

    return fclose(fp);
}

int update_connection_list(char* identifier, sockaddr_in* remote_addr){
    char* file_name = "connection_list.txt";
    FILE* fp = fopen(file_name, "a+");
    if(!fp){
        perror("Error opening connection_list.txt");
        exit(1);
    }
    
    fprintf(fp, "%s,%s\n", identifier,inet_ntoa(remote_addr->sin_addr));
    return fclose(fp);
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

void send_connection_list(int sockfd){
    char data[SIZE] = {0};
    char* file_name = "connection_list_export.txt";

    
    FILE* fp = fopen(file_name, "r");

    while(fgets(data, SIZE, fp)){
        if(send(sockfd, data, sizeof(data), 0) == -1){
            perror("Error in sending data");
            exit(1);
        }
        memset(data, 0, SIZE);
    }

    fclose(fp);
    remove(file_name);
}

void receive(int new_sockfd, char* buffer){
    struct pollfd fd;
    int poll_ret;
    fd.fd = new_sockfd;
    fd.events = POLLIN;
    int bytes_recv;
    int done = false;
    while(!done){
        poll_ret = poll(&fd, 1, 3000);
        switch(poll_ret){
            case -1:
                //There was an error
                done = true;
                break;
            case 0:
                //Time out happened
                done = true;
                break;
            default:
                bytes_recv = recv(new_sockfd, buffer, SIZE, 0);
                break;
        }

        printf("Received: %s. Bytes Received: %d\n", buffer, bytes_recv);
    }
}