// run as root, ./server_hw4a

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
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
        bzero(buff, MAX);

        // Read client message and copy to buffer
        read(sockfd, buff, sizeof(buff));

        // Print buffer and clear it out
        printf("From client: %s\t To client : ", buff);
        bzero(buff, MAX);
        n = 0;

        // Copy server message to buffer
        while ((buff[n++] = getchar()) != '\n')
        {
            ;
        }

        //Send buffer to client
        write(sockfd, buff, sizeof(buff));

        // If message contains "exit" the chat will end
        if (strncmp("exit", buff, 4) == 0)
        {
            printf("Server Exit...\n");
            break;
        }
    }
}

int main()
{
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

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
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0)
    {
        printf("socket bind failed...\n");
        exit(0);
    } 
    else
    {
        printf("Socket successfully binded..\n");
    }

    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0)
    {
        printf("Listen failed...\n");
        exit(0);
    }
    else
    {
        printf("Server listening..\n");
    }
    len = sizeof(cli); 

    // Accept the data packet from client and verification
    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd < 0)
    {
        printf("server acccept failed...\n");
        exit(0);
    }
    else
    {
        printf("server acccept the client...\n");
    }

    // Function for chatting between client and server
    chat(connfd);

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