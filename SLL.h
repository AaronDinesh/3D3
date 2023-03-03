#ifndef SLL
#define SLL

typedef struct Node Node;
typedef struct sockaddr_in sockaddr_in;

struct Node{
    char* identifier;
    int sockfd;
    sockaddr_in* remote_conn;
    time_t seconds_since_conn;
    Node* next;
};

void prepend_Node(Node** start_ptr_ptr, char* identifier, int sockfd, sockaddr_in* remote_conn, time_t seconds);
void print_list_ip(Node** start_ptr_ptr);
void print_list_identifier(Node** start_ptr_ptr);
void delete_list(Node** start_ptr_ptr);
void delete_list_recursive(Node* start_ptr);
void delete_node(Node** start_ptr_ptr, char* identifier);
Node* search_list_recursive_identifier(Node* start_ptr, char* identifier);
Node* search_list_recursive_sockfd(Node* start_ptr, int sockfd);

#endif