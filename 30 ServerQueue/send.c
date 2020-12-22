#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "header.h"

int closeQueue(int queue) {
    if(msgctl(queue,IPC_RMID,NULL) == ERROR) {
        perror("Can't close queue");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

size_t msgLength(char *msg) {
    size_t length = strlen(msg);
    if(msg[length - 1] == '\n') {
        msg[length - 1] = '\0';
    } else {
        ++length;
    }

    return length;
}

int sendMessages(int queue) {
    struct message buf;
    buf.mtype = MSG_TYPE_DATA;
    int isWork = TRUE;
    do {
        if(fgets(buf.mtext, SIZE_MSG, stdin) == NULL) {
            fprintf(stderr, "Can't read message\n");
            return EXIT_FAILURE;
        }

        size_t length = msgLength(buf.mtext);

        if(length == LENGTH_QUIT_MSG && strncmp(buf.mtext, QUIT_MSG, LENGTH_QUIT_MSG) == EQUALS_STR) {
            buf.mtype = MSG_TYPE_END;
            isWork = FALSE;
        }

        if(msgsnd(queue, &buf, length, NO_FLAGS) == ERROR) {
            perror("Can't send message");
            return EXIT_FAILURE;
        }

    } while(isWork);

    return EXIT_SUCCESS;
}

int waitAnswer(int queue) {
    struct message buf;
    if(msgrcv(queue, &buf, SIZE_MSG, MSG_TYPE_ANSWER, NO_FLAGS) == ERROR) {
        perror("Can't get answer");
        return EXIT_FAILURE;
    }

    printf("Get answer from recv: %s\n", buf.mtext);

    return EXIT_SUCCESS;
}

int main() {
    int queue;

    queue = msgget(getuid(), IPC_CREAT | QUEUE_RIGHTS);
    if(queue == ERROR)     {
        perror("Can't make queue");
        return EXIT_FAILURE;
    }
    printf("Queue was created\nEnter messages (max length - %d symbols, quit - for exit):\n", SIZE_MSG - 1); //because we need to have a place for '\0' in the end of message

    if(sendMessages(queue) != EXIT_SUCCESS) {
        closeQueue(queue);
        return EXIT_FAILURE;
    }

    if(waitAnswer(queue) != EXIT_SUCCESS) {
        closeQueue(queue);
        return EXIT_FAILURE;
    }

    if(closeQueue(queue) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    printf("Queue was closed\n");
    return EXIT_SUCCESS;
}

