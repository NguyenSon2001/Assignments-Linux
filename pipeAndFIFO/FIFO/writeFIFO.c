#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FIFOname "./testFIFO"
#define BUF_SIZE 1024

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <string>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char buf[BUF_SIZE];
    int fd;
    mkfifo(FIFOname, 0666);

    fd = open(FIFOname, O_WRONLY);
    if (fd == -1)
    {
        perror("open\n");
        exit(EXIT_FAILURE);
    }

    write(fd, argv[1], strlen(argv[1]) + 1);
    close(fd);

    printf("Done wirte %s in FIFO", argv[1]);

    return 0;
}