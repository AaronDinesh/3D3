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
void get_client_ip(char* buff);
bool isEmpty(char buff[]);


int main(){
    //Initalise things for the client
    Node* client_list = NULL;
    int input;
    int sockfd;
    char str;
    char name[SIZE] = {'\0'};
    char server_ip[SIZE] = {'\0'};
    char client_ip[SIZE] = {'\0'};
    char search_buffer[SIZE] = {'\0'};
    bool done = false;

    //Get the IP address of the client
    get_client_ip(client_ip);

    //Ask the Client the IP Address of the server and what it wants to be known by.
    printf("What is the server IP (IPv4): ");
    scanf(" %4096s", server_ip);
    printf("What is your Identifier: ");
    scanf(" %4096s", name);
    
    //Connect to the server
    sockfd = connect_sever(name, server_ip);    
    update_client_list(&client_list);
    close(sockfd);

    //The main event loop for the client
    printf("What do you want to do\n");
    printf("\t[1] Update the Client List\n");
    printf("\t[2] Connect to another client\n");
    printf("\t[3] Print client list\n");
    printf("\t[4] Get IP of client\n");
    printf("\t[5] Quit\n");
    scanf(" %d", &input);
    while(!done){
        if(input == 1){
            printf("Updating client list\n");
            sockfd = connect_sever(name, server_ip);    
            update_client_list(&client_list);
            printf("Printing out updated client list...\n");
            print_list_info(&client_list);
            close(sockfd);
        }else if(input == 2){
            printf("Connecting to a Client\n");
            printf("As stated in the report this part was not programed\n");
            printf("We focused on the tracking server - client interaction\n");
            printf("This was done because the server - client code could\n");
            printf("Easily be changed to work with client - client \n");
            printf("We instead wanted to showcase reliable data transfer\n");
            printf("And in the second demo show that messages can be encrypted\n");
        }else if(input == 3){
            sockfd = connect_sever(name, server_ip);    
            update_client_list(&client_list);
            close(sockfd);
            printf("Printing Client list...\n");
            print_list_info(&client_list);
        }else if (input == 4){
            sockfd = connect_sever(name, server_ip);    
            update_client_list(&client_list);
            close(sockfd);
            Node* search_return = NULL;
            printf("Who do you want to find?\n");
            scanf(" %4059s", search_buffer);
            search_return = search_list_recursive_identifier(client_list, search_buffer);

            if(!search_return){
                printf("Could not find %s in list\n", search_buffer);
                printf("Make sure the identifier is correct\n");
            }else{
                char search_return_ip[SIZE] = {'\0'};
                inet_ntop(AF_INET, &(search_return->remote_conn->sin_addr.s_addr), search_return_ip, INET_ADDRSTRLEN);
                printf("Result: %s with IP %s", search_return->identifier, search_return_ip);
            }

        }else if(input == 5){
            printf("Quitting...\n");
            return 0;
        }else if(input != 0){
            printf("Invalid Option: %d\n", input);
        }
        printf("What do you want to do\n");
        printf("\t[1] Update the Client List\n");
        printf("\t[2] Connect to another client\n");
        printf("\t[3] Print client list\n");
        printf("\t[4] Get IP of client\n");
        printf("\t[5] Quit\n");
        scanf(" %d", &input);
    }

    return 0;
}

void send_file(FILE* fp, int sockfd){
    char data[SIZE] = {0};
    //Get the data from the file until fgets returns EOF or Null
    while(fgets(data, SIZE, fp)){
        if(send(sockfd, data, sizeof(data), 0) == -1){
            perror("Error in sending data");
            exit(1);
        }

        //Reset buffer after each read
        memset(data, 0, SIZE);
    } 

    fclose(fp);

}

int connect_sever(char* name, char* ip){
    //Define some connection variables for the server. Comms is done on port 10000
    int server_port = 10000;
    char buffer[SIZE] = {"\0"};
    int sockfd;
    sockaddr_in server_addr;

    //Set the socket to be TCP
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    //Error checking for socket creation
    if(sockfd < 0){
        perror("Error creating the socket");
        exit(1);
    }

    //Fill the server sockaddr struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = server_port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    //Connect and check for return err
    if(connect(sockfd, (sockaddr*) &server_addr, (socklen_t) sizeof(server_addr)) == -1){
        perror("Couldn't connect to the server");
        exit(1);
    }else{
        printf("Successfully connected to server\n");
    }

    //The server will send an <IDEN> tag so we can send the client identifier
    recv(sockfd, buffer, SIZE, 0);

    if(strcmp(buffer, "<IDEN>") == 0){
        printf("Going to send name\n");
        if(send(sockfd, name, sizeof(name), 0) == -1){
            perror("Error in sending name");
            exit(1);
        }
    }
    write_connection_file(sockfd);
    return sockfd;
}

void write_connection_file(int sockfd){
    //Declare some variables for the the new socket
    struct pollfd fd;
    int poll_ret;
    fd.fd = sockfd;
    fd.events = POLLIN;
    int bytes_recv;
    int done = false;

    //Write the recvieved client lists
    int n;
    FILE* file  = NULL;
    char* filename = "connection_list_received.txt";
    char buffer[SIZE];

    file = fopen(filename, "w");

    if(!file){
        perror("Error creating file");
        exit(1);
    }

    //While there is still something to read, read it. Timeout will occur in 1000 ms
    while(!done){
    poll_ret = poll(&fd, 1, 4000);
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
            if(!isEmpty(buffer)){
                printf("Received %s\n", buffer);
                fprintf(file, "%s", buffer);
            }
            memset(buffer, 0, SIZE);
            break;
    }
    } 

    fclose(file);
}

void update_client_list(Node** client_list){
    //Open the connection list from the server
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

    //Delete the existing client list
    if((*client_list)){
        delete_list(client_list);
    }

    *client_list = NULL;

    char* token = NULL;

    //Read the file and update the client list
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

void get_client_ip(char* buff){
    //Runs the command "hostname -I >> ip.txt" command as if it was in a shell
    system("hostname -I >> ip.txt");
    
    //Open the ip.txt file and get the IP address.
    FILE* fp = fopen("ip.txt", "r");
    fgets(buff, SIZE, fp);
    buff[strcspn(buff, "\n")] = '\0';
    printf("IP is: %s\n", buff);
    fclose(fp);
    remove("ip.txt");
}

bool isEmpty(char buff[]){
    for(int i = 0; i < SIZE; i++){
        if(buff[i] != '\0'){
            return false;
        }
    }
    return true;
}