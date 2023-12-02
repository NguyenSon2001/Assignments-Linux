#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(void)
{
    pid_t pid = fork();

    switch (pid) {
           case -1:
               perror("fork");
               exit(EXIT_FAILURE);
           case 0:
                printf("my PID is : %d\n", getpid());
                printf("my PPID is : %d\n", getppid());
               puts("Child exiting.");
               exit(EXIT_SUCCESS);
           default:
               puts("Parent exiting.");
               exit(EXIT_SUCCESS);
           }

return 0;
}

