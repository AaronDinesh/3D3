
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SIZE 1024

void send_file(FILE* fp, int sockfd);

int main(int argc, char* argv){
    char name[SIZE] = {"\0"};
    printf("What is your Identifier: ");
    scanf(" %1023s", name);
    char *server_ip = "127.0.0.1";
    int server_port = 10000;
    int e;

    int sockfd;
    struct sockaddr_in server_addr;
    FILE *file = NULL;
    char *filename = "send.txt";

    char buffer[SIZE] = {"\0"};

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd < 0){
        perror("Error creating the socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = server_port;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    e = connect(sockfd, (struct sockaddr*) &server_addr, (socklen_t) sizeof(server_addr));
    recv(sockfd, buffer, SIZE, 0);
    if(strcmp(buffer, "<IDEN>") == 0){
        if(send(sockfd, name, sizeof(name), 0) == -1){
            perror("Error in sending name");
            exit(1);
        }
    }

    if(e == -1){
        perror("Error in connecting to server");
        exit(1);
    }else{
        printf("Successfully connected to server\n");
    }

    file = fopen(filename, "r");
    if(!file){
        perror("Could not open file");
        exit(1);
    }

    // send_file(file, sockfd);
    // printf("Sent file sucessfully\n");
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