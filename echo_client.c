//  Created by Michael Tan on 2019/11/8.
//  Copyright © 2019 MichaelTan9999. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <signal.h>

int sockfd;

void exit_handler(int signal)
{
    close(sockfd);
    exit(0);
}

int main(int argc, char *argv[])
{
    int port, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];
    
    if (argc < 3 || argc > 3)
    {
        perror("Usage: client <hostname> <port>\n");
        exit(1);
    }
    port = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("ERROR opening socket");
        exit(1);
    }
    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        perror("ERROR, no such host\n");
        exit(1);
    }
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(port);
    
    if (connect(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
        perror("ERROR connecting");
        exit(1);
    }

    signal(SIGINT, exit_handler);

    bzero(buffer,256);
    while (1)
    {
//        bzero(buffer,256);
        fgets(buffer, 256, stdin);
        n = write(sockfd, buffer, strlen(buffer));
        if (!(strncmp(buffer, "EOF", 3)))
        {
            break;
        }
        if (n < 0)
        {
            perror("ERROR writing to socket");
            exit(1);
        }
        bzero(buffer, 256);
        n = read(sockfd, buffer, 256);
        if (n < 0)
        {
            perror("ERROR reading from socket");
            exit(1);
        }
        printf("%s", buffer);
    }
    close(sockfd);
    return 0;
}

