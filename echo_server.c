#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>

int sockfd, connfd;

void exit_handler(int signal)
{
    close(connfd);
    close(sockfd);
    exit(0);
}

int main(int argc, char *argv[])
{
    int port;
    socklen_t client_length;
    char buffer[256];
    struct sockaddr_in server_addr, client_addr;

    if (argc != 3)
    {
        perror("Usage: server <address> <port>\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error: Opening socket.\n");
    }

    bzero((char*) &server_addr, sizeof(server_addr));
    port = atoi(argv[2]);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0)
    {
        perror("Error: Binding failed.\n");
        exit(1);
    }

    signal(SIGINT, exit_handler);

    listen(sockfd, 1);
    
    while (1)
    {
        client_length = sizeof(client_addr);
        connfd = accept(sockfd, (struct sockaddr*) &client_addr, &client_length);
        if (connfd < 0)
        {
            perror("Error: Accepting failed.\n");
            exit(1);
        }
        bzero(buffer, 256);
        while (1)
        {
            if ((read(connfd, buffer, 255)) < 0)
            {
                perror("Error: Reading failed.\n");
                exit(1);
            }
            if ((write(connfd, buffer, strlen(buffer))) < 0)
            {
                perror("Error: Writing failed.\n");
                exit(1);
            }
            bzero(buffer, 256);
        }
    }
}