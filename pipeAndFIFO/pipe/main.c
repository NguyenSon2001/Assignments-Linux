#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MS_SIZE 1024

int main(int argc, char *argv[])
{
    pid_t pid;
    int pipefd[2];
    char pipeBuf[MS_SIZE];

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <string>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (signal(SIGCHLD, SIG_IGN) == SIG_ERR)
    {
        perror("signal\n");
        exit(EXIT_FAILURE);
    }

    if (pipe(pipefd) == -1)
    {
        perror("pipe\n");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    switch (pid)
    {
    case -1:
        perror("fork error\n");
        exit(EXIT_FAILURE);
    case 0:
        close(pipefd[1]);

        read(pipefd[0], &pipeBuf, MS_SIZE);
        printf("Child read in pipe: %s\n", pipeBuf);

        close(pipefd[0]);
        exit(EXIT_SUCCESS);
    default:

        close(pipefd[0]);
        printf("Parent write in pipe: %s\n", argv[1]);
        write(pipefd[1], argv[1], strlen(argv[1]));
        close(pipefd[1]);

        exit(EXIT_SUCCESS);
    }

    return 0;
}
