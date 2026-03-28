/**
CMPSC 311: Introduction to Systems, Programming Project, Group 1
**/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> // Include for gethostbyname

#define SERV_TCP_PORT 9999

// Define a struct to hold an integer and a character
struct Data{
    int integer;
    char character[100];
};

// Implement functions for sending and receiving messages over sockets
void send_message(int sockfd, char message[]) {
    if (write(sockfd, message, strlen(message)) < 0) {
        perror("Write to Socket");
        exit(EXIT_FAILURE);
    }
}

struct Data receive_message(int sockfd, int buffer_size) {
    struct Data data;
    // allocate memory for the struct Data

    // Populate the integer field
    data.integer = read(sockfd, &(data.character), 99);
    if (data.integer < 0) {
        perror("Read from Socket");
        exit(EXIT_FAILURE);
    }
    return data;
}

int connect_server()
{
    struct sockaddr_in serv_addr;
    struct hostent *server;

    //create socket file
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Opening socket");
        exit(EXIT_FAILURE);
    }

    server = gethostbyname("localhost");
    if (server == NULL) {
        fprintf(stderr, "No host");
        exit(EXIT_FAILURE);
    }

    //Server Address
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(SERV_TCP_PORT);

    //Connecting to server
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connecting to server");
        exit(EXIT_FAILURE);
    }

    return sockfd;

}
