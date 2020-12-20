#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "header.h"

int sendEndAnswer(int queue) {
    struct message buf = {MSG_TYPE_ANSWER, QUIT_MSG};

    if(msgsnd(queue, &buf, LENGTH_QUIT_MSG, NO_FLAGS) == ERROR) {
        perror("Can't send message");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int getMessages(int queue) {
    struct message msg;
    ssize_t code;
    while((code = msgrcv(queue, &msg, SIZE_MSG, MSG_ALL_TYPES, NO_FLAGS)) > 0) {
        if(msg.mtype == MSG_TYPE_END) {
            break;
        }

        printf("Msg type: %ld, msgrcv return: %d, msg: %s\n", msg.mtype, (int)code, msg.mtext);
    }

    if(code == ERROR) {
        perror("Can't get message");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int main() {
    int queue;

    if((queue = msgget(getuid(), NO_FLAGS)) == ERROR) {
        perror("Can't get queue");
        return EXIT_FAILURE;
    }

    printf("Connected\n");

    if(getMessages(queue) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    if(sendEndAnswer(queue) != EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


