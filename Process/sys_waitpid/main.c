#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(void)
{
    pid_t cpid, wcpid;
    int wstatus;

    cpid = fork();
    switch (cpid)
    {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    case 0:
        printf("my PID is : %d\n", getpid());
        printf("my PPID is : %d\n", getppid());
        while (1)
            ;
        puts("Child exiting.");
        exit(EXIT_SUCCESS);
    default:

        wcpid = waitpid(cpid, &wstatus, WUNTRACED);

        if (wcpid == -1)
        {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }

        if (WIFEXITED(wstatus))
        {
            printf("exited, status = %d\n", WEXITSTATUS(wstatus));
        }
        else if (WIFSIGNALED(wstatus))
        {
            printf("stopped by signal %d\n", WTERMSIG(wstatus));
        }
        else if (WIFSTOPPED(wstatus))
        {
            printf("stopped by signal %d\n", WSTOPSIG(wstatus));
        }

        puts("Parent exiting.");
        exit(EXIT_SUCCESS);
    }

    return 0;
}
