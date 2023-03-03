#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <stdbool.h>
#include <netdb.h>
#include <time.h>
#include "SLL.h"

#define SIZE 4096
typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct Node Node;

void send_file(FILE* fp, int sockfd);
int connect_sever(char* name, char* ip);
void write_connection_file(int sockfd);
void update_client_list(Node** client_list);
void flush();


int main(){
    Node* client_list = NULL;
    int input;
    char str;
    char name[SIZE] = {'\0'};
    char ip[SIZE] = {'\0'};
    bool done = false;
    printf("What is the server IP (IPv4): ");
    scanf(" %4096s", ip);
    printf("What is your Identifier: ");
    scanf(" %4096s", name);
    int sockfd = connect_sever(name, ip);    
    update_client_list(&client_list);
    close(sockfd);
    printf("What do you want to do\n");
    printf("[1] Update the Client List\n");
    printf("[2] Connect to another client\n");
    printf("[3] Quit\n");
    scanf(" %d", &input);
    while(!done){
        if(input == 1){
            printf("Got here\n");
            sockfd = connect_sever(name, ip);    
            update_client_list(&client_list);
            close(sockfd);
        }else if(input == 2){
            printf("Connecting to a Client\n");
        }else if(input == 3){
            printf("Quitting...\n");
            return 0;
        }else if(input != 0){
            printf("Invalid Option: %d\n", input);
        }
        printf("What do you want to do\n");
        printf("\t[1] Update the Client List\n");
        printf("\t[2] Connect to another client\n");
        printf("\t[3] Quit\n");
        scanf(" %d", &input);
    }

    return 0;
}

void send_file(FILE* fp, int sockfd){
    char data[SIZE] = {0};
    while(fgets(data, SIZE, fp)){
        if(send(sockfd, data, sizeof(data), 0) == -1){
            perror("Error in sending data");
            exit(1);
        }

        memset(data, 0, SIZE);
    } 

    fclose(fp);

}

int connect_sever(char* name, char* ip){
    int server_port = 10000;
    int e;
    char buffer[SIZE] = {"\0"};


    int sockfd;
    sockaddr_in server_addr;


    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd < 0){
        perror("Error creating the socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = server_port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    e = connect(sockfd, (sockaddr*) &server_addr, (socklen_t) sizeof(server_addr));

    if(e == -1){
        perror("Error in connecting to server");
        exit(1);
    }else{
        printf("Successfully connected to server\n");
    }

    recv(sockfd, buffer, SIZE, 0);

    if(strcmp(buffer, "<IDEN>") == 0){
        printf("Going to send name\n");
        if(send(sockfd, name, sizeof(name), 0) == -1){
            perror("Error in sending name");
            exit(1);
        }
    }
    printf("Sent name\n");

    write_connection_file(sockfd);
    return sockfd;
}

void write_connection_file(int sockfd){
    struct pollfd fd;
    int poll_ret;
    fd.fd = sockfd;
    fd.events = POLLIN;
    int bytes_recv;
    int done = false;

    int n;
    FILE* file  = NULL;
    char* filename = "connection_list_received.txt";
    char buffer[SIZE];

    file = fopen(filename, "w");

    if(!file){
        perror("Error creating file");
        exit(1);
    }

     while(!done){
        poll_ret = poll(&fd, 1, 5000);
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
                bytes_recv = recv(sockfd, buffer, SIZE, 0);
                fprintf(file, "%s", buffer);
                memset(buffer, 0, SIZE);
                break;
        }
     } 
}

void update_client_list(Node** client_list){
    FILE* fp = fopen("connection_list_received.txt", "r");
    char buffer[SIZE] = {"\0"};
    char identifier[SIZE] = {"\0"};
    char ip[SIZE] = {"\0"};
    time_t seconds;
    int i = 0;

    if(!fp){
        perror("Could not open file");
        exit(1);
    }

    if(!(*client_list)){
        delete_list(client_list);
    }

    char* token = NULL;

    while(!feof(fp)){
        fscanf(fp, "%s\n", buffer);
        token = strtok(buffer, ",");
        while(token){
            switch(i){
                case 0:
                    strcpy(identifier, token);
                    printf("%s\n", identifier);
                    i++;
                    break;
                case 1:

                    strcpy(ip, token);
                    printf("%s\n", ip);
                    i++;
                    break;
                case 2:
                    seconds = atoll(token);
                    printf("%ld\n", seconds);
                    i = 0;
                    break;
                default:
                    break;
            }
            token = strtok(NULL, ",");
        }

        sockaddr_in* remote_conn = (sockaddr_in*) malloc(sizeof(sockaddr_in));
        remote_conn->sin_port = 10000;
        remote_conn->sin_family = AF_INET;
        remote_conn->sin_addr.s_addr = inet_addr(ip);

        prepend_Node(client_list, identifier, 0, remote_conn, seconds);
        memset(identifier, 0, sizeof(buffer));
        memset(ip, 0, sizeof(buffer));
        seconds = 0;
    }

}

void flush(){
    char ch;
    while ((ch = getchar()) != '\n' && ch != EOF);  
}