#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void signal_hanlder(int signum)
{
    printf("print by signal_hanlder");
    wait(NULL);
}

int main(void)
{
    pid_t cpid = fork();

    switch (cpid)
    {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    case 0:
        printf("my PID is : %d\n", getpid());
        printf("my PPID is : %d\n", getppid());
        puts("Child exiting.");
        exit(EXIT_SUCCESS);
    default:
        signal(SIGCHLD, signal_hanlder);
        while (1);
        puts("Parent exiting.");
        exit(EXIT_SUCCESS);
    }

    return 0;
}
