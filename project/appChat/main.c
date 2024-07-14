#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <mqueue.h>
#include <ifaddrs.h>

#define LISTEN_SERVICE 10
pthread_t pthread_id;
typedef struct listData
{
    char IPaddr[20];
    int port_no;
    int socket_fd;
} listData;

struct message
{
    long priority; // id của sinh viên
    struct listData lData;
};

char mqName[50] = "/mqueue";

struct sockaddr_in ser_addr;
int sfd;
long prio = 1;
mqd_t mqid;
pthread_mutex_t mutexCon_read = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexAccept = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condCon_read = PTHREAD_COND_INITIALIZER;
int flagCon_read = 0;

void getMyIP()
{
    struct ifaddrs *ifap, *ifa;
    struct sockaddr_in *sa;

    if (getifaddrs(&ifap) == -1)
    {
        perror("Không thể lấy địa chỉ IP");
        exit(EXIT_FAILURE);
    }

    for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr != NULL && ifa->ifa_addr->sa_family == AF_INET)
        {
            if (strcmp(ifa->ifa_name, "ens33") == 0)
            {
                sa = (struct sockaddr_in *)ifa->ifa_addr;
                char *ipAddr = inet_ntoa(sa->sin_addr);
                printf("My IP is : %s\n", ipAddr);
                break;
            }
        }
    }

    freeifaddrs(ifap);
}

int revExit(int socket_fd)
{
    struct message msg[10];
    int i = 0;
    while (1)
    {
        if (mq_receive(mqid, (char *)&msg[i], sizeof(struct message), NULL) == -1)
        {
            printf("Can't find socketfd");
            return -1;
        }
        // In thông tin từ struct
        if (socket_fd == msg[i].lData.socket_fd)
        {
            printf("\n**************************************************\n");
            printf("Ending connection with id %ld, ID %s, port %d\n", msg[i].priority, msg[i].lData.IPaddr, msg[i].lData.port_no);
            printf("**************************************************\n");
            i--;
            return 0;
        }
        i++;
    }

    while (i > 0)
    {

        if (mq_send(mqid, (const char *)&msg[i], sizeof(struct message), msg[i].priority) == -1)
        {
            perror("mq_send()");
            return -1;
        }
        i--;
    }
}

void readSend(int socket_fd, char IP[20])
{
    int mess;
    char buf[1024];

    do
    {
        mess = read(socket_fd, buf, sizeof(buf));
        if (mess == -1)
        {
            perror("reading stream message");
            break;
        }
        else if (mess == 0)
        {
            if (revExit(socket_fd) == -1)
            {
                perror("prevExit");
                break;
            }
        }
        else if (mess > 0)
        {

            printf("\n**************************************************\n");
            printf("send from %s --> %s\n", IP, buf);
            printf("**************************************************\n");
        }

    } while (mess > 0);

    close(socket_fd);
}

void *thread_accept(void *arg)
{
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(struct sockaddr_in));
    int len = sizeof(client_addr);

    int client_socket = accept(sfd, (struct sockaddr *)&client_addr, (socklen_t *)&len);
    if (client_socket == -1)
        fprintf(stderr, "accept()\n");
    pthread_mutex_lock(&mutexAccept);

    struct message msg;
    msg.priority = prio++;
    strcpy(msg.lData.IPaddr, inet_ntoa(client_addr.sin_addr));
    msg.lData.port_no = ntohs(client_addr.sin_port);
    msg.lData.socket_fd = client_socket;

    pthread_mutex_unlock(&mutexAccept);
    sleep(1);

    if (mq_send(mqid, (const char *)&msg, sizeof(struct message), msg.priority) == -1)
    {
        perror("mq_send()");
        pthread_exit(NULL);
    }
    printf("\n**************************************************\n");
    printf("Accepted a new connection from address: %s, setup at port: %d\n", msg.lData.IPaddr, msg.lData.port_no);
    printf("**************************************************\n");

    pthread_t pthread_id;
    if (pthread_create(&pthread_id, NULL, &thread_accept, NULL) == -1)
    {
        printf("can't create pthead_id\n");
    }

    readSend(client_socket, msg.lData.IPaddr);
    return 0;
}

void *thread_receiveSocket(void *arg)
{
    pthread_mutex_lock(&mutexCon_read);
    struct listData *lData = (struct listData *)arg;
    int socket = lData->socket_fd;
    char IP[20];
    strcpy(IP, lData->IPaddr);
    flagCon_read = 1;
    pthread_cond_signal(&condCon_read);
    pthread_mutex_unlock(&mutexCon_read);
    sleep(1);
    // block đến đây
    readSend(socket, IP);
    return 0;
}

int connectSocket(struct message msg, mqd_t mqid)
{
    struct sockaddr_in serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(msg.lData.port_no);
    if (inet_pton(AF_INET, msg.lData.IPaddr, &serv_addr.sin_addr) == -1)
    {
        perror("inet_pton()");
        return -1;
    }

    /* Tạo socket */
    msg.lData.socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (msg.lData.socket_fd == -1)
    {
        perror("socket()");
        return -1;
    }

    /* Kết nối tới server*/
    if (connect(msg.lData.socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        perror("connect()");
        return -1;
    }

    if (pthread_create(&pthread_id, NULL, &thread_receiveSocket, &msg.lData) == -1)
    {
        printf("can't create thread_receiveSocket\n");
    }

    pthread_mutex_lock(&mutexCon_read);
    if (flagCon_read == 0)
    {
        pthread_cond_wait(&condCon_read, &mutexCon_read);
    }

    if (mq_send(mqid, (const char *)&msg, sizeof(struct message), msg.priority) == -1)
    {
        perror("mq_send()");
        return -1;
    }
    flagCon_read = 0;
    pthread_mutex_unlock(&mutexCon_read);

    return 0;
}

void menu()
{
    printf("\n**************************[CHAT APPLICATION]**************************\n");
    printf("Use the commands below:\n");
    printf("1. help                            :display user interface options\n");
    printf("2. myip                            :\n");
    printf("3. myport                          :\n");
    printf("4. connect <destination> <port no> :\n");
    printf("5. list                            :\n");
    printf("6. terminate <conection id>        :\n");
    printf("7. send <conection id> <message>   :\n");
    printf("8. exit                            :\n");
    printf("\n**********************************************************************\n");
}
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Invalid format\n");
        printf("%s <port no>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    menu();

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1)
        fprintf(stderr, "socket()\n");

    // memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(atoi(argv[1]));
    ser_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sfd, (struct sockaddr *)&ser_addr, sizeof(ser_addr)) == -1)
    {
        fprintf(stderr, "bind()\n");
        exit(EXIT_FAILURE);
    }

    if (listen(sfd, LISTEN_SERVICE) == -1)
    {
        fprintf(stderr, "listen()\n");
        exit(EXIT_FAILURE);
    }

    struct mq_attr attr;
    strcat(mqName, argv[1]);
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10; // Maximum number of messages in the queue
    attr.mq_msgsize = sizeof(struct message);
    attr.mq_curmsgs = 0;
    mqid = mq_open(mqName, O_RDWR | O_CREAT | O_NONBLOCK, 0666, &attr);
    if (mqid == -1)
    {
        printf("mq_open() error ");
        exit(EXIT_FAILURE);
    }

    pthread_t pthread_id;
    if (pthread_create(&pthread_id, NULL, &thread_accept, NULL) == -1)
    {
        printf("can't create pthead_id\n");
        exit(EXIT_FAILURE);
    }

    char command[2048];
    char *com[3];
    while (1)
    {
        printf("Enter command: ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = '\0';

        com[0] = strtok(command, " ");

        if (strcmp(com[0], "help") == 0)
        {
            printf("\n help \n");
        }
        else if (strcmp(com[0], "myip") == 0)
        {
            getMyIP();
        }
        else if (strcmp(com[0], "myport") == 0)
        {
            printf("My port is: %d\n", atoi(argv[1]));
        }
        else if (strcmp(com[0], "connect") == 0)
        {
            int i = 0;
            while (com[i] != NULL)
            {
                i++;
                com[i] = strtok(NULL, " ");
            }

            // setup data
            pthread_mutex_lock(&mutexAccept);
            struct message msg;
            msg.priority = prio++;
            msg.lData.port_no = atoi(com[2]);
            strncpy(msg.lData.IPaddr, com[1], sizeof(msg.lData.IPaddr));
            pthread_mutex_unlock(&mutexAccept);
            // connect
            if (connectSocket(msg, mqid) == 0)
            {
                printf("\nDone connect IP: %s    port:%d\n", msg.lData.IPaddr, msg.lData.port_no);
            }
        }
        else if (strcmp(com[0], "list") == 0)
        {
            printf("-------------------List---------------------\n");
            struct message msg[10];
            int i = 0;
            while (1)
            {
                if (mq_receive(mqid, (char *)&msg[i], sizeof(struct message), NULL) == -1)
                {
                    break;
                }
                // In thông tin từ struct
                printf("id=%ld   IP=%s    port=%d  \n", msg[i].priority, msg[i].lData.IPaddr, msg[i].lData.port_no);
                i++;
            }

            while (i > 0)
            {

                if (mq_send(mqid, (const char *)&msg[i - 1], sizeof(struct message), msg[i - 1].priority) == -1)
                {
                    perror("mq_send()");
                    return -1;
                }
                i--;
            }

            // In thông tin từ struct

            printf("-------------------End---------------------\n");
        }
        else if (strcmp(com[0], "terminate") == 0)
        {
            int con_id = atoi(strtok(NULL, " "));

            struct message msg[10];
            int i = 0;
            int sfd = 0;
            while (1)
            {
                if (mq_receive(mqid, (char *)&msg[i], sizeof(struct message), NULL) == -1)
                {
                    break;
                }

                i++;
                if (msg[i - 1].priority == con_id)
                {
                    sfd = msg[i - 1].lData.socket_fd;
                    printf("sfd  =  %d", sfd);
                    i--;
                    break;
                }
            }

            while (i > 0)
            {

                if (mq_send(mqid, (const char *)&msg[i - 1], sizeof(struct message), msg[i - 1].priority) == -1)
                {
                    perror("mq_send()");
                    return -1;
                }
                i--;
            }
            if (shutdown(sfd, SHUT_WR) == -1)
            {
                perror("shutdown");
                return -1;
            }

            printf("Done terminate connection id %d\n", con_id);

            // if (close(sfd) != -1)
            // {
            //     printf("Done terminate connection id %d\n", con_id);
            // }
        }
        else if (strcmp(com[0], "send") == 0)
        {
            com[1] = strtok(NULL, " ");
            com[2] = strtok(NULL, "\n");

            struct message msg[10];
            int i = 0;
            int sfd = 0;
            while (1)
            {
                if (mq_receive(mqid, (char *)&msg[i], sizeof(struct message), NULL) == -1)
                {
                    break;
                }

                i++;
                if (msg[i - 1].priority == atoi(com[1]))
                {
                    sfd = msg[i - 1].lData.socket_fd;
                    break;
                }
            }
            if (sfd > 0)
            {
                // printf("msg id=%ld\n", msg[i - 1].priority);
                if (write(sfd, com[2], strlen(com[2])) == -1)
                    perror("write()");
            }
            else
            {
                printf("Can't found id %d\n", atoi(com[1]));
            }

            while (i > 0)
            {

                if (mq_send(mqid, (const char *)&msg[i - 1], sizeof(struct message), msg[i - 1].priority) == -1)
                {
                    perror("mq_send()");
                    return -1;
                }
                i--;
            }
        }

        else if (strcmp(com[0], "exit") == 0)
        {
            printf("\nExit app\n");
            if (mq_unlink(mqName) == -1)
            {
                perror("mq_unlink");
            }
            close(sfd);
            return 0;
        }
    }
    exit(0);
}
