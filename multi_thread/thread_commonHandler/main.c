#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_t pthread_id1, pthread_id2;

struct human
{
    char *hoTen;
    char *namSinh;
    int sdt;
    char *queQuan;
};

void *thread_handler(void *arg)
{
    struct human *data = (struct human *)arg;
    pthread_t tid = pthread_self();
    if (pthread_equal(tid, pthread_id2))
    {
        printf("i'm thread_id2\n ");
        printf("%s |%s |%s |%d\n", data->hoTen, data->namSinh, data->queQuan, data->sdt);

        data->sdt = 20;
    }
    else if (pthread_equal(tid, pthread_id1))
    {
        printf("i'm thread_id1 \n ");
    }
}

int main()
{

    struct human myHuman;

    myHuman.hoTen = "ho ten";
    myHuman.namSinh = "nam sinh";
    myHuman.queQuan = "Que quan";
    myHuman.sdt = 10000;

    if (pthread_create(&pthread_id1, NULL, &thread_handler, NULL) == 0)
    {
        printf("created pthead_id1\n");
    }
    else
    {
        printf("can't create pthead_id1\n");
    }

    if (pthread_create(&pthread_id2, NULL, &thread_handler, &myHuman) == 0)
    {
        printf("created pthead_id2\n");
    }
    else
    {
        printf("can't create pthead_id2\n");
    }

    sleep(5);
    printf("%d", myHuman.sdt);
    printf("main exit");
    return 0;
}