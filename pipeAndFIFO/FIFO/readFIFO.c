#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define FIFOname "./testFIFO"
#define BUF_SIZE 1024

int main()
{

    char buf[BUF_SIZE];
    int fd;

    fd = open(FIFOname, O_RDONLY);
    if (fd == -1)
    {
        perror("open\n");
        exit(EXIT_FAILURE);
    }

    read(fd, &buf, BUF_SIZE);
    close(fd);

    printf("Data read in FIFO: %s\n", buf);

    return 0;
}