#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "header.h"

int regInQueue(int queue) {
    pid_t pid = getpid();
    struct message buf = {MSG_TYPE_REG, pid, ""};

    if(msgsnd(queue, &buf, sizeof(pid_t), NO_FLAGS) == ERROR) {
        perror("Can't register in queue");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int getMessages(int queue, char *argv0) {
    struct message msg;
    ssize_t code;
    pid_t pid = getpid();
    while((code = msgrcv(queue, &msg, SIZE_MSG + sizeof(pid_t), pid, NO_FLAGS)) >= 0) {
        if(code == sizeof(pid_t)) { //if message is empty
            break;
        }

        printf("%s, msg: %s\n", argv0, msg.mtext);
    }

    if(code == ERROR) {
        perror("Can't get message");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int leaveQueue(int queue) {
    pid_t pid = getpid();
    struct message buf = {MSG_TYPE_END_ANSWER, pid, ""};

    if(msgsnd(queue, &buf, sizeof(pid_t), NO_FLAGS) == ERROR) {
        perror("Can't send message");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    if(argc < 1) {
        fprintf(stderr, "Bad args\n");
        return EXIT_FAILURE;
    }

    key_t queueKey = ftok("send.c", PROJECT_PREFIX);
    if(queueKey == ERROR) {
        perror("Can't generate key for queue");
        return EXIT_FAILURE;
    }

    int queue;
    queue = msgget(queueKey, NO_FLAGS);
    if(queue == ERROR) {
        perror("Can't get queue");
        return EXIT_FAILURE;
    }

    if(regInQueue(queue) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    if(getMessages(queue, argv[0]) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    if(leaveQueue(queue) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


