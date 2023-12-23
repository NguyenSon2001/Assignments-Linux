#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>

void signal_handler(int num_signal)
{
    printf("I'm SIGINT, my signal is %d\n", num_signal);
}

int main()
{
    sigset_t newset, oldset;

    if (signal(SIGINT, signal_handler) == SIG_ERR)
    {
        printf("Can't handler SIGINT\n");
        exit(EXIT_FAILURE);
    }
    sigemptyset(&newset);
    sigemptyset(&oldset);

    if (sigprocmask(SIG_SETMASK, NULL, &oldset) == -1)
    {
        printf("Can't get oldset\n");
        exit(EXIT_FAILURE);
    }

    int ret = sigismember(&oldset, SIGINT);
    sigaddset(&newset, SIGINT);
    printf("ret = %d\n", ret);
    if (ret == -1)
    {
        printf("Can't call sigismember\n");
    }
    else if (ret) // SIGINT là member của oldset
    {
        
        ret = sigprocmask(SIG_UNBLOCK, &newset, &oldset);
        if (ret == 1)
        {
            printf("I'm unblock, Can't set newset\n");
            exit(EXIT_FAILURE);
        }
        printf("I'm unblock, unblock SIGINT\n");
    }
    else //SIGINT không là member của oldset
    {
        ret = sigprocmask(SIG_BLOCK, &newset, &oldset);
        if (ret == 1)
        {
            printf("I'm block, Can't set newset\n");
            exit(EXIT_FAILURE);
        }
        printf("I'm block, block SIGINT\n");
        
    }

    while (1)
    {
        sleep(2);
    }
}