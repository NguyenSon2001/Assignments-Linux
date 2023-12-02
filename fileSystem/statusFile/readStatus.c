#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
int main()
{
    char *pathname = "test.txt";
    int openFile = open(pathname, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (openFile < 0)
    {
        printf("ERROR:can't open \n");
        return 0;
    }

    // char *textFile = "1 cai gi do\n";
    // ssize_t t = write(openFile, textFile, strlen(textFile));
    // if (t < 0)
    // {
    //     printf("ERROR: can't write \n");
    //     return 0;
    // }

    struct stat file_stat;
    int st = stat(pathname, &file_stat);

    if (st < 0)
    {
        printf("ERROR: can't connect stat \n");
        return 0;
    }
    printf("loai file :");
    mode_t modeFile = file_stat.st_mode;
    if (S_ISREG(modeFile))
        printf("Regular File %o\n",modeFile & S_IFMT);
    else if (S_ISDIR(modeFile))
        printf("Directory\n");
    else if (S_ISCHR(modeFile))
        printf("character device\n");
    else if (S_ISBLK(modeFile))
        printf("block device\n");
    else if (S_ISFIFO(modeFile))
        printf("FIFO \n");
    else if (S_ISLNK(modeFile))
        printf("symbolic link\n");
    else if (S_ISSOCK(modeFile))
        printf("socket\n");
    

    printf("ten flie : %s \n", pathname);
    printf("Thoi gian chinh sua lan cuoi : %s \n", ctime(&file_stat.st_mtim.tv_sec));
    printf("Kich thuoc : %ld \n", file_stat.st_size);
    

    close(openFile);
    return 0;
}