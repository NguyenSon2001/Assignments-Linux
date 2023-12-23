#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

pthread_t pthread_id1, pthread_id2;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond3 = PTHREAD_COND_INITIALIZER;

typedef struct sinhVien
{
    char hoTen[20];
    char ngaySinh[10];
    char queQuan[20];
} sinhVien;

sinhVien sv;
int retScan = 0;

void *thread_handler1(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&mutex1);
        printf("thread 1 run ...\n");
        while (retScan == 1)
            pthread_cond_wait(&cond3, &mutex1);

        printf("\nNhap thong tin sinh vien:");

        printf(" \nHo ten: ");
        fgets(sv.hoTen, sizeof(sv.hoTen), stdin);

        printf(" \nNgay sinh: ");
        fgets(sv.ngaySinh, sizeof(sv.ngaySinh), stdin);

        printf(" \nQue quan: ");
        fgets(sv.queQuan, sizeof(sv.queQuan), stdin);

        retScan = 1;
        pthread_cond_signal(&cond1);

        pthread_mutex_unlock(&mutex1);
        sleep(1);
    }
}

void *thread_handler2(void *arg)
{

    while (1)
    {
        pthread_mutex_lock(&mutex1);
        printf("thread 2 run ...\n");
        if (retScan == 0)
            pthread_cond_wait(&cond1, &mutex1);
        printf("\n Dang nhap thong tin sinh vien ... ");

        sv.hoTen[strcspn(sv.hoTen, "\n")] = '\0';
        sv.ngaySinh[strcspn(sv.ngaySinh, "\n")] = '\0';
        sv.queQuan[strcspn(sv.queQuan, "\n")] = '\0';

        char textString[60];

        snprintf(textString, sizeof(textString), "%s   %s   %s\n", sv.hoTen, sv.ngaySinh, sv.queQuan);

        int df = open("thongtinsinhvien.txt", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
        if (df < 0)
        {
            printf("\n Can't open file");
        }

        off_t seekFile = lseek(df, 0, SEEK_END);
        if (seekFile < 0)
        {
            printf("ERROR:can't set offset \n");
            return 0;
        }

        ssize_t t = write(df, textString, strlen(textString));

        close(df);
        pthread_cond_signal(&cond2);
        pthread_mutex_unlock(&mutex1);
        sleep(1);
    }
}

int main()
{
    if (pthread_mutex_init(&mutex1, NULL))
    {
        printf("can't create mutex1\n");
    }

    if (pthread_mutex_init(&mutex2, NULL))
    {
        printf("can't create mutex2\n");
    }

    if (pthread_cond_init(&cond1, NULL))
    {
        printf("can't create cond1 \n");
    }
    if (pthread_cond_init(&cond2, NULL))
    {
        printf("can't create cond2 \n");
    }
    if (pthread_cond_init(&cond3, NULL))
    {
        printf("can't create cond3 \n");
    }

    if (pthread_create(&pthread_id1, NULL, &thread_handler2, NULL))
    {
        printf("can't create pthead_id1\n");
    }

    if (pthread_create(&pthread_id2, NULL, &thread_handler1, NULL))
    {
        printf("can't create pthead_id2\n");
    }

    while (1)
    {
        pthread_mutex_lock(&mutex1);
        printf("thread 3 run ...\n");
        pthread_cond_wait(&cond2, &mutex1);
        printf("\nDa nhap xong thong tin sinh vien: %s  %s  %s", sv.hoTen, sv.ngaySinh, sv.queQuan);
        retScan = 0;
        pthread_cond_signal(&cond3);

        pthread_mutex_unlock(&mutex1);
        sleep(1);
    }

    if (pthread_mutex_destroy(&mutex1))
    {
        printf("can't destroy mutex1");
    }

    if (pthread_mutex_destroy(&mutex2))
    {
        printf("can't destroy mutex2");
    }

    if (pthread_cond_destroy(&cond1))
    {
        printf("can't destroy cond");
    }

    printf("main exit");
    return 0;
}