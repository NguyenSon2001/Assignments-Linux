#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

int count_user1, count_user2;

void signal_handler(int num_signal)
{
    printf("I m signal handler, my signal is %d\n", num_signal);
}

void signal_handler1(int num_signal)
{
    printf("I m signal handler, my signal is %d, my counter is %d\n", num_signal, count_user1++);
}

void signal_handler2(int num_signal)
{
    printf("I m signal handler, my signal is %d, my counter is %d\n", num_signal, count_user2++);
}

int main()
{

    if (signal(SIGINT, signal_handler) == SIG_ERR)
    {
        printf("Can't handler SIGINT");
        exit(EXIT_FAILURE);
    }

      if (signal(SIGUSR1, signal_handler1) == SIG_ERR)
    {
        printf("Can't handler SIGUSR1");
        exit(EXIT_FAILURE);
    }

      if (signal(SIGUSR2, signal_handler2) == SIG_ERR)
    {
        printf("Can't handler SIGUSR2");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        sleep(2);
    }
}