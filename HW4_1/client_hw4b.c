// run as root, ./client_hw4b <IP address>

#include <arpa/inet.h>
#include <fcntl.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

// Define the max buffer size
#define MAX 80

// Define the port the server will run on
#define PORT 80

// Define a struct for the socket
#define SA struct sockaddr

/* This function performs the "chat" betweent he client
 * and the server.
 */
void chat(int sockfd)
{
    char buff[MAX];
    int n;

    // Run the chat in an infinite loop
    for (;;)
    {
        // clear the buffer
        bzero(buff, sizeof(buff));

        // Client prompted for first message
        printf("Enter the string : "); 
        n = 0;

        // Copy client message to server
        while ((buff[n++] = getchar()) != '\n')
        {
            ;
        }

        // Send buffer to server
        write(sockfd, buff, sizeof(buff));
        bzero(buff, sizeof(buff));

        // Read buffer from server and print
        read(sockfd, buff, sizeof(buff));
        printf("From Server : %s", buff);

        // If message contains "exit" the chat will end
        if ((strncmp(buff, "exit", 4)) == 0)
        {
            printf("Client Exit...\n");
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    // Must be passed an IP address as argument
    if(argc!=2)
    {
        printf("\nFormat: ./client_hw4b < ipaddress>\n");
        return 0;
    }

    // Create socket and verify, if fails set and print error
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
    {
        printf("Socket successfully created..\n");
    }

    // Clear the socket
    bzero(&servaddr, sizeof(servaddr));

    // Assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(PORT);

    // Connect the client socket to the server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0)
    { 
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
    {
        printf("connected to the server..\n");
    }

    // Function for chatting between client and server
    chat(sockfd);

    // After chatting close the socket
    close(sockfd);
}


/*
RESOURCES
https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/
https://www.geeksforgeeks.org/ping-in-c/
http://www.strudel.org.uk/itoa/
https://www.binarytides.com/socket-programming-c-linux-tutorial/
*/