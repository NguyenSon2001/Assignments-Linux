#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define SUM_RESULT 10000000000

pthread_t pthread_id1, pthread_id2;
int sum_multi, sum_single;

void *thread_handler1(void *arg)
{
    sum_multi++;
}
void *thread_handler2(void *arg)
{

    sum_multi++;
}

int main()
{

    if (pthread_create(&pthread_id1, NULL, &thread_handler1, NULL))
    {
        printf("can't create pthead_id1\n");
    }

    if (pthread_create(&pthread_id2, NULL, &thread_handler2, NULL))
    {
        printf("can't create pthead_id2\n");
    }
    while (1)
    {
        sum_single++;
        if (sum_single == SUM_RESULT || sum_multi == SUM_RESULT)
        {
            printf("sum single = %d | sum multi = %d\n", sum_single, sum_multi);
            return 1;
        }
    }

    printf("main exit");
    return 0;
}