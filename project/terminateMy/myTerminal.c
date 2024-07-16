#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAXCOM 1000
#define MAXLIST 100
#define clear() printf("\033[H\033[J")

void init_shell();
void printDir();
int takeInput(char *str);
int parsePipe(char *str, char **strpiped);
int process_ex(char *str_input);
int parseSpace(char *str, char **parsed);
int main()
{
    char inputStr[MAXCOM];
    init_shell();
    while (1)
    {
        printDir();
        if (takeInput(inputStr))
            continue;
        int ret = process_ex(inputStr);
        if(ret == -1){
            printf("\nRemove the trailing '|' to avoid parsing errors.\n");
        }
    }

    return 0;
}

int process_ex(char *str_input)
{
    char *strpiped[2];
    char *parsed[MAXLIST];
    int piped = 0;

    piped = parsePipe(str_input, strpiped);
    if (piped < 0)
        return -1;
    else if (piped == 0){
        int byte =  parseSpace(str_input, parsed);
        return 0;
    }
    else{

    }
}
int parsePipe(char *str, char **strpiped)
{
    strpiped[0] = strsep(&str, "|");
    strpiped[1] = str;

    if (strpiped[1] == NULL)
        return 0; // returns zero if no pipe is found.
    else
    {
        if (strlen(strpiped[1]) == 0)
            return -1;
        else
            return 1;
    }
}
int parseSpace(char *str, char **parsed)
{
    int i;

    for (i = 0; i < MAXLIST; i++)
    {
        parsed[i] = strsep(&str, " ");

        if (parsed[i] == NULL)
            break;
        if (strlen(parsed[i]) == 0)
            i--;
    }
    if(i <= 0 ) return -1;
    return i;
}

void init_shell()
{
    clear();
    printf("\n\n\n\n******************"
           "************************");
    printf("\n\n\n\t****WEOCOME TO MY SHELL****");
    printf("\n\n\n\n*******************"
           "***********************");
    char *username = getenv("USER");
    printf("\n\n\nUSER is: @%s", username);
    printf("\n");
    sleep(1);
    clear();
}

void printDir()
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("\nDir: %s", cwd);
}

int takeInput(char *str)
{
    char *buf;

    buf = readline("\n>>!> ");
    if (strlen(buf) != 0)
    {
        add_history(buf);
        strcpy(str, buf);

        free(buf);
        return 0;
    }
    else
    {
        return 1;
    }
}