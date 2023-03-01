
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <stdbool.h>

#define SIZE 1024

void send_file(FILE* fp, int sockfd);
int connect_sever(char* name);
void write_connection_file(int sockfd);


int main(int argc, char* argv){
    char name[SIZE] = {"\0"};
    printf("What is your Identifier: ");
    scanf(" %1023s", name);
    int sockfd = connect_sever(name);    
    
    close(sockfd);
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

int connect_sever(char* name){
    char *server_ip = "10.6.1.87";
    int server_port = 10000;
    int e;
    char buffer[SIZE] = {"\0"};


    int sockfd;
    struct sockaddr_in server_addr;


    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd < 0){
        perror("Error creating the socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = server_port;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    e = connect(sockfd, (struct sockaddr*) &server_addr, (socklen_t) sizeof(server_addr));

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