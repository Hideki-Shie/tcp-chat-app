/**
CMPSC 311: Introduction to Systems, Programming Project, Group 1
**/


#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define SERV_TCP_PORT 23 /* server's port number */
#define MSG_LEN     2048
#define MAX_CLIENT  512


//initialize global variables
int clientCount = 0; //counter for the connected clients
int clientSocks[MAX_CLIENT]; //array for the connected clients
pthread_mutex_t mutex; // declare mutex variable


//show message all connected clients
void msgAll(char *msg, int len)
{
    int i;

    pthread_mutex_lock(&mutex); //enter the critical section

    //send message all connected clients
    for (i = 0; i < clientCount; i++)
        write(clientSocks[i], msg, len);

    pthread_mutex_unlock(&mutex); //leave the critical section
}

//receive client's message and display it to all connected clients
void* serverClient(void *arg)
{
    int clientSock = *((int*)arg);
    int str_len = 0;
    int i;
    char msg[MSG_LEN];

    //loop until client terminates
    while ((str_len = read(clientSock, msg, sizeof(msg))) != 0)
        msgAll(msg, str_len);

    pthread_mutex_lock(&mutex); //enter the critical section

    //removes client from list before terminating thread
    for (i = 0; i < clientCount; i++)
    {
        if (clientSock == clientSocks[i])
        {
            while (i < clientCount - 1)
            {
                clientSocks[i] = clientSocks[i + 1];
                i++;
            }

            break;
        }
    }

    clientCount--;
    pthread_mutex_unlock(&mutex); //leave the critical section

    //close connection
    close(clientSock);

    return NULL;
}

int main(int argc, char *argv[])
{
    int serveSock, clientSock;  //server and client sockets
    struct sockaddr_in serv_addr, cli_addr; //socket addresses
    int cli_addr_sz; //size of the client cli_addr
    int port;
    pthread_t thread; //initialize thread

    /* command line: server [port_number] */
    if (argc == 2)
    {
        sscanf(argv[1], "%d", &port); /* read the port number if provided */
    }
    else
    {
        port = SERV_TCP_PORT;
    }

    pthread_mutex_init(&mutex, NULL);

    //open a TCP socket
     if((serveSock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {

         perror("can't open stream socket");

         exit(1);

      }

    //bind the local address, so that the client can send to server
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    // bind server's address information to the server socket
    if (bind(serveSock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("can't bind local address");
        exit(1);
    }

     //listen to the socket
    if (listen(serveSock, 5) == -1)
        perror("listen failed");


    //welcome message
    printf("Server running \n");

    while (1)
    {
        //wait for a connection from a client
        cli_addr_sz = sizeof(cli_addr);

        clientSock = accept(serveSock, (struct sockaddr*)&cli_addr, &cli_addr_sz);

        if (clientSock < 0)
            {
                perror("can't accept local connection");
            }

        //add new clients to the list
        pthread_mutex_lock(&mutex); //enter critical section
        clientSocks[clientCount++] = clientSock;
        pthread_mutex_unlock(&mutex); //leave critical section

        //make a thread to serve client
        pthread_create(&thread, NULL, serverClient, (void*)&clientSock);

        //detach from main thread
        pthread_detach(thread);

        //print connected client's IP address
        printf("User joined \n");
    }

    close(serveSock);

    return 0;
}



