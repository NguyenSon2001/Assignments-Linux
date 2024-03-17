#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>

#define MAX_NAME_LENGTH 50
#define MAX_ADDRESS_LENGTH 100
#define MAX_PHONE_LENGTH 15
#define MAX_MSG_SIZE sizeof(struct message)

struct sinhvien {
    char hoten[MAX_NAME_LENGTH];
    int tuoi;
    char quequan[MAX_ADDRESS_LENGTH];
    char sdt[MAX_PHONE_LENGTH];
};

struct message {
    long priority;  // id của sinh viên
    struct sinhvien sv;
};

void displayMenu() {
    printf("\n**************************[STUDENT MANAGEMENT]**************************\n");
    printf("1. Add Student\n");
    printf("2. Display Student by ID\n");
    printf("3. Display All Students\n");
    printf("4. Remove Student by ID\n");
    printf("5. Exit\n");
    printf("**********************************************************************\n");
}

void addStudent(mqd_t mqid) {
    struct message msg;
    printf("Enter Student ID (greater than 0): ");
    scanf("%ld", &msg.priority);

    if (msg.priority <= 0) {
        printf("Invalid ID. ID must be greater than 0.\n");
        return;
    }

    printf("Enter Student Name: ");
    scanf("%s", msg.sv.hoten);

    printf("Enter Student Age: ");
    scanf("%d", &msg.sv.tuoi);

    printf("Enter Student Address: ");
    scanf("%s", msg.sv.quequan);

    printf("Enter Student Phone: ");
    scanf("%s", msg.sv.sdt);

    if (mq_send(mqid, (const char*)&msg, MAX_MSG_SIZE, msg.priority) == -1) {
        perror("mq_send");
        return;
    }

    printf("Student added successfully!\n");
}

void displayStudentByID(mqd_t mqid) {
    unsigned int studentID;
    printf("Enter Student ID to display: ");
    scanf("%u", &studentID);

    struct message msg;
    if (mq_receive(mqid, (char*)&msg, MAX_MSG_SIZE, NULL) == -1) {
        perror("mq_receive");
        return;
    }


    printf("Student ID: %ld\n", msg.priority);
    printf("Name: %s\n", msg.sv.hoten);
    printf("Age: %d\n", msg.sv.tuoi);
    printf("Address: %s\n", msg.sv.quequan);
    printf("Phone: %s\n", msg.sv.sdt);


    if (mq_send(mqid, (const char*)&msg, MAX_MSG_SIZE, msg.priority) == -1) {
        perror("mq_send");
        return;
    }
}

void displayAllStudents(mqd_t mqid) {
    struct message msg;
    printf("-------------------Student List---------------------\n");

    while (mq_receive(mqid, (char*)&msg, MAX_MSG_SIZE, NULL) != -1) {
        printf("Student ID: %ld\n", msg.priority);
        printf("Name: %s\n", msg.sv.hoten);
        printf("Age: %d\n", msg.sv.tuoi);
        printf("Address: %s\n", msg.sv.quequan);
        printf("Phone: %s\n", msg.sv.sdt);
        printf("-----------------------------------------\n");
    }

    printf("-------------------End---------------------\n");
}

void removeStudentByID(mqd_t mqid) {
    unsigned int studentID;
    printf("Enter Student ID to remove: ");
    scanf("%u", &studentID);

    // Send a message with priority -1 to indicate removal
    struct message msg;
    msg.priority = -1;

    if (mq_send(mqid, (const char*)&msg, MAX_MSG_SIZE, studentID) == -1) {
        perror("mq_send");
        return;
    }

    printf("Student removed successfully!\n");
}

int main() {
    mqd_t mqid;
    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;    // Maximum number of messages in the queue
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    mqid = mq_open("/student_queue", O_RDWR | O_CREAT|O_NONBLOCK, 0666, &attr);
    if (mqid == -1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    int choice;
    do {
        displayMenu();
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                addStudent(mqid);
                break;
            case 2:
                displayStudentByID(mqid);
                break;
            case 3:
                displayAllStudents(mqid);
                break;
            case 4:
                removeStudentByID(mqid);
                break;
            case 5:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice. Please enter a valid option.\n");
        }
    } while (choice != 5);

    mq_close(mqid);
    mq_unlink("/student_queue");

    return 0;
}
