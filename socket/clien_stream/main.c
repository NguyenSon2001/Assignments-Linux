#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

struct sockaddr_in ser_addr;
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Invalid format\n");
        printf("./build.run <addr> <port>\n");
        exit(EXIT_FAILURE);
    }

    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1)
        fprintf(stderr, "socket()\n");

    // memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(atoi(argv[2]));
    struct hostent *hp, *gethostbyname();
    hp = gethostbyname(argv[1]);
    memcpy((char *)&ser_addr.sin_addr, (char *)hp->h_addr,
           hp->h_length);

    if (connect(sfd, (struct sockaddr *)&ser_addr, sizeof ser_addr) == -1)
    {
        perror("connect()");
        exit(1);
    }

    char *data = "haha" ;
    if (write(sfd, data, sizeof data) == -1)
        perror("write()");
    close(sfd);
    exit(0);
}