#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <errno.h>
#include "SLL.h"
#include <poll.h>
#include <time.h>


/*
This code file is meant to impliment a "Tracking server" similar to how BitTorrent works.
When a client connects to the server, the server logs the timestamp of connection and
stores it in a  linked list with the other client details. If the client doesn't reconnect
within 40 minutes (2400 seconds) then the tracking server assumes that the client has fully
disconnected from the network and will remove its entry.

Also when a client connects to a server it will receive the full connection list, so that
the client can then talk to any of the clients that are on the network.

What allows this to be decentralised is that this connection list can be dumped into a file
and this file can then be sent to a random node in the network that will then assume the role
of the tracking server. This way the network doesn't rely on an always on server.

*/

//Define MAX buffer size
#define SIZE 4096

//Define some typedefs for the server
typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct Node Node;


//Define the functions and some global variables
int list_length = 0;
int add_connection(Node** client_list, int new_sockfd, sockaddr_in* remote_addr);
int export_connection_list(Node** client_list);
int update_connection_list(char* identifier, sockaddr_in* remote_addr, time_t seconds);
void send_connection_list(int sockfd);
int server_init(void);
void receive(int new_sockfd, char* buffer);
void check_client_list(Node** client_list);

int main(){
    //Print out server IP
    system("hostname -I");

    //Start a Linked List to hold all the client connections
    Node* client_list = NULL;

    //Initalise the server
    int sockfd = server_init();
    int i = 0;

    //These are some structs for when a client connects
    int new_sock;
    sockaddr_in new_addr;
    socklen_t new_addr_size = sizeof(new_addr);

    //Main While loop for the server
    while(list_length >= 0){
        new_sock = accept(sockfd, (sockaddr*) &new_addr, &new_addr_size);
        if(errno == EWOULDBLOCK && new_sock == -1){
            sleep(1);
        }else{
            add_connection(&client_list, new_sock, &new_addr);
            list_length++;
        }
    }
    return 0;
}


int add_connection(Node** client_list, int new_sockfd, sockaddr_in* remote_addr){
    //This will log the UTC second that the client joins
    time_t seconds;
    time(&seconds);

    //Send the identifier message to the Client
    char* msg = "<IDEN>";
    char buffer[SIZE] = {"\0"};
    send(new_sockfd, msg, sizeof(msg), 0);

    //Receive the identifier
    receive(new_sockfd, buffer);

    printf("Got Identifier: %s\n", buffer);

    printf("Going to search list\n");

    //Check the client list to see if the client has already connected
    if(!search_list_recursive_identifier(*(client_list), buffer)){
        //If it is a new client then add it to the list
        printf("Client is not in list. Adding Client\n");
        prepend_Node(client_list, buffer, new_sockfd, remote_addr, seconds);
        list_length++;
        update_connection_list(buffer, remote_addr, seconds);
    }else{
        //If it is already in the list then update the time since last 
        time(&seconds);
        search_list_recursive_identifier(*client_list, buffer)->seconds_since_conn = seconds;
    }

    //Send the connetion list to the client
    printf("Exporting and sending connection list\n");
    check_client_list(client_list);
    //export_connection_list(client_list);
    send_connection_list(new_sockfd);
    memset(buffer, 0, SIZE);
    return 0;
}

int export_connection_list(Node** client_list){
    //Create a new export file
    char* file_name = "connection_list_export.txt";
    char long_str[10+sizeof(char)];
    FILE* fp = fopen(file_name, "w");
    if(!fp){
        perror("Error opening connection_list_export.txt");
        exit(1);
    }

    //Dump the entire contents of the client_list
    Node* next_ptr = *client_list;
    while(next_ptr){
        sprintf(long_str, "%ld", next_ptr->seconds_since_conn);
        fprintf(fp, "%s,%s,%s\n", next_ptr->identifier,inet_ntoa(next_ptr->remote_conn->sin_addr),long_str);
        next_ptr = next_ptr->next;
    }

    //Close the file
    return fclose(fp);
}

int update_connection_list(char* identifier, sockaddr_in* remote_addr, time_t seconds){
    char long_str[10+sizeof(char)];
    sprintf(long_str, "%ld", seconds);
    //Open the connection_list.txt in append mode
    char* file_name = "connection_list.txt";
    FILE* fp = fopen(file_name, "a+");
    if(!fp){
        perror("Error opening connection_list.txt");
        exit(1);
    }
    //Append the new entry
    fprintf(fp, "%s,%s,%s\n", identifier,inet_ntoa(remote_addr->sin_addr), long_str);
    return fclose(fp);
}

int server_init(void){
    //Set up some variables to start the server
    char *ip = "0.0.0.0";
    int port = 10000;
    int e;
    int sockfd;
    sockaddr_in server_addr;

    //Create the socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        perror("Error in Socket");
        exit(1);
    }

    printf("Server socket has been created\n");

    //Load the server_addr struct with the ip and port
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    //Bind to the socket
    e = bind(sockfd, (struct sockaddr*) &server_addr, (socklen_t) sizeof(server_addr));
    if(e < 0){
        perror("Error in binding");
        exit(1);
    } 

    printf("Binding Sucessful\n");
    
    //Listen on this socket and accept a max of 50 connections
    e = listen(sockfd, 50);
    if(e == 0){
        printf("Listening...\n");
    }else{
        perror("Max connections reached");
        exit(1);
    }

    return sockfd;
}

void send_connection_list(int sockfd){
    //Create a data buffer
    char data[SIZE] = {0};

    
    //Open the connection_list_export.txt
    char* file_name = "connection_list.txt";
    FILE* fp = fopen(file_name, "r");

    //Parse the file and send the data throgh sockfd
    while(fgets(data, SIZE, fp)){
        if(send(sockfd, data, sizeof(data), 0) == -1){
            perror("Error in sending data");
            exit(1);
        }
        memset(data, 0, SIZE);
    }

    fclose(fp);
}

void receive(int new_sockfd, char* buffer){
    //Create a pollfd struct to poll the socket
    struct pollfd fd;
    int poll_ret;
    fd.fd = new_sockfd;
    fd.events = POLLIN;
    int bytes_recv;
    int done = false;
    while(!done){
        //Set a timeout for for 3 seconds
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
                //Else receive all the bytes
                bytes_recv = recv(new_sockfd, buffer, SIZE, 0);
                if(bytes_recv == 0){
                    done = true;
                }
                break;
        }

        printf("Received: %s. Bytes Received: %d\n", buffer, bytes_recv);
    }
}

void check_client_list(Node** client_list){
    //Set uo the seconds counter
    time_t seconds;
    Node* next_ptr = *client_list;
    while(next_ptr){
        time(&seconds);
        Node* tmp = next_ptr->next;
        if(seconds - (next_ptr->seconds_since_conn) >= 2400){
            //If the current time is more than 40 minutes after the the last time recorded in
            //seconds_since_conn then delete the entry.
            delete_node(client_list, next_ptr->identifier);
            list_length--;
        }
        next_ptr = tmp;
    }

}

