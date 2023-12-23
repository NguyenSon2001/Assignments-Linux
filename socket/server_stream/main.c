#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define LISTEN_SERVICE 10

struct sockaddr_in ser_addr;
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Invalid format\n");
         printf("./build.run <port>\n");
        exit(EXIT_FAILURE);
    }

    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1)
        fprintf(stderr, "socket()\n");

    // memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET; 
    ser_addr.sin_port = htons(atoi(argv[1]));
    ser_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sfd, (struct sockaddr *)&ser_addr, sizeof(ser_addr)) == -1)
        fprintf(stderr, "bind()\n");

    if (listen(sfd, LISTEN_SERVICE) == -1)
        fprintf(stderr, "listen()\n");

    while (1)
    {
        printf("listening to the client's connection\n ");
        int msgsock = accept(sfd, (struct sockaddr *)0, (int *)0);
        if (msgsock == -1)
            fprintf(stderr, "accept()\n");

        int mess;
        char buf[1024];

        do
        {
            mess = read(msgsock, buf, sizeof(buf));
            if (mess == -1)
                perror("reading stream message");
            else if (mess == 0)
                printf("Ending connection\n");
            else
                printf("--> %s\n", buf);
        } while (mess > 0);

        close(msgsock);
    }
    exit(0);
}