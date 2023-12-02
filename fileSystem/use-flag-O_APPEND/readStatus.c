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
    int openFile = open(pathname, O_CREAT | O_APPEND | O_RDWR, S_IRUSR | S_IWUSR);
    if (openFile < 0)
    {
        printf("ERROR:can't open \n");
        return 0;
    }

    off_t seekFile = lseek(openFile, 0, SEEK_SET);
    if (seekFile < 0)
    {
        printf("ERROR:can't set offset \n");
        return 0;
    }

    char *textFile = "thu hien ghi vao\n";
    ssize_t t = write(openFile, textFile, strlen(textFile));
    if (t < 0)
    {
        printf("ERROR: can't write \n");
        return 0;
    }

    close(openFile);
    return 0;
}