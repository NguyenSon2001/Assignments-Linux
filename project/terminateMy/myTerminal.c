#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
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
int runshell(char **parsed, int lenParsed);
int ownCmdHandler(char **parsed);




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
        if (ret == -1)
        {
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
    else if (piped == 0)
    {
        int len = parseSpace(str_input, parsed);
        if (ownCmdHandler(parsed))
            return 0;
        int ret = runshell(parsed, len);
        return 0;
    }
    else
    {
    }
}

int redirect_output_to_file(char *pathname)
{
    int fd = open(pathname, O_CREAT | O_APPEND | O_WRONLY, 0644);
    if (fd < 0)
    {
        printf("ERROR:can't open %s\n", pathname);
        return -1;
    }
    dup2(fd, STDOUT_FILENO);
    close(fd);

    return 0;
}

int redirect_input_to_file(char *pathname)
{
    int fd = open(pathname, O_RDONLY, 0664);
    if (fd < 0)
    {
        printf("ERROR:can't open %s\n", pathname);
        return -1;
    }
    dup2(fd, STDIN_FILENO);
    close(fd);

    return 0;
}

int runshell(char **parsed, int lenParsed)
{
    char token[2];
    // Forking a child
    pid_t pid = fork();

    if (pid == -1)
    {
        printf("\nFailed forking child..");
        return -1;
    }
    else if (pid == 0)
    {
        for (int i = 0; i < lenParsed; i++)
        {
            if (strcmp(parsed[i], ">") == 0 || strcmp(parsed[i], "<") == 0)
            {
                strcpy(token, parsed[i]);
                if (i + 1 < lenParsed)
                {
                    if (strcmp(parsed[i], ">") == 0)
                        redirect_output_to_file(parsed[i + 1]);
                    else if (strcmp(parsed[i], "<") == 0)
                        redirect_input_to_file(parsed[i + 1]);

                    for (int j = i; j < lenParsed - 2; j++)
                    {
                        parsed[j] = parsed[j + 2];
                    }
                    parsed[lenParsed - 2] = NULL; // Kết thúc mảng

                    break;
                }
                else
                {
                    printf("No file after token '%s'\n", token);
                    return -3;
                }
            }
        }
        if (execvp(parsed[0], parsed) < 0)
        {
            printf("\nCould not execute command..");
        }
        exit(0);
    }
    else
    {
        // waiting for child to terminate
        wait(NULL);
        return 0;
    }
}

int ownCmdHandler(char **parsed)
{
    int NoOfOwnCmds = 3, i, switchOwnArg = 0;
    char *ListOfOwnCmds[NoOfOwnCmds];
    char *username;

    ListOfOwnCmds[0] = "exit";
    ListOfOwnCmds[1] = "cd";
    ListOfOwnCmds[2] = "hello";

    for (i = 0; i < NoOfOwnCmds; i++)
    {
        if (strcmp(parsed[0], ListOfOwnCmds[i]) == 0)
        {
            switchOwnArg = i + 1;
            break;
        }
    }

    switch (switchOwnArg)
    {
    case 1:
        printf("\nGoodbye\n");
        exit(0);
    case 2:
        chdir(parsed[1]);
        return 1;
    case 3:
        username = getenv("USER");
        printf("\nHello %s.\nMind that this is "
               "not a place to play around."
               "\nUse help to know more..\n",
               username);
        return 1;
    default:
        break;
    }

    return 0;
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
    if (i <= 0)
        return -1;
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