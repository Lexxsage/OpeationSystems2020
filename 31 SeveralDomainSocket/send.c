#include <sys/types.h>
#include <signal.h>
#include <termios.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>

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

struct array * initArray() {
    struct array *res = calloc(1, sizeof(struct array));
    if(res == NULL) {
        perror("Can't init array");
        return NULL;
    }

    return res;
}

int pushBack(struct array *arr, pid_t item) {
    if(arr->indexForAdd < arr->size) {
        arr->arr[arr->indexForAdd] = item;
        ++arr->indexForAdd;
        return EXIT_SUCCESS;
    }

    pid_t *tmp = (pid_t *)realloc(arr->arr, arr->size + ARRAY_RATIO_SIZE);
    if(tmp == NULL) {
        perror("Can't add new element");
        return EXIT_FAILURE;
    }

    arr->size += ARRAY_RATIO_SIZE;
    arr->arr = tmp;
    arr->arr[arr->indexForAdd] = item;
    ++arr->indexForAdd;
    return EXIT_SUCCESS;
}

int find(struct array *arr, pid_t item) {
    int i;
    for(i = 0; i < arr->indexForAdd; ++i) {
        if(arr->arr[i] == item) {
            return i;
        }
    }

    return ERROR;
}

int removeItem(struct array *arr, int index) {
    if(index < 0 || index >= arr->indexForAdd) {
        return EXIT_FAILURE;
    }

    int i;
    for(i = index + 1; i < arr->indexForAdd; ++i) {
        arr->arr[i - 1] = arr->arr[i];
    }
    --arr->indexForAdd;

    return EXIT_SUCCESS;
}

void destroyArray(struct array *arr) {
    free(arr->arr);
    free(arr);
}

int isEmpty(struct array *arr) {
    return arr->indexForAdd <= 0;
}

int addListeners(int queue, struct array *pids) {
    struct message buf;

    while(msgrcv(queue, &buf, sizeof(pid_t), MSG_TYPE_REG, IPC_NOWAIT) != ERROR) {
        if(pushBack(pids, buf.pid) != EXIT_SUCCESS) {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

int delListeners(int queue, struct array *pids) {
    struct message buf;

    while(msgrcv(queue, &buf, sizeof(pid_t), MSG_TYPE_END, IPC_NOWAIT) != ERROR) {
        int index = find(pids, buf.pid);
        if(index == ERROR) {
            fprintf(stderr, "Del listener, which doesn't exits\n");
            return EXIT_FAILURE;
        }

        if(removeItem(pids, index) != EXIT_SUCCESS) {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

int updListeners(int queue, struct array *pids) {
    if(addListeners(queue, pids) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    if(delListeners(queue, pids) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int sendMessage(int queue, struct array *pids, struct message *buf, size_t length) {
    int i;
    for(i = 0; i < pids->indexForAdd; ++i) {
        buf->mtype = pids->arr[i];
        if(msgsnd(queue, buf, length + sizeof(pid_t), NO_FLAGS) == ERROR) {
            perror("Can't send message");
            return EXIT_FAILURE;
        }
    }

    printf("Message was sent to %d listener(s)\n", i);

    return EXIT_SUCCESS;
}

int sendMessages(int queue, struct array *pids) {
    printf("Enter messages (max length - %d symbols, Ctrl+D - for exit):\n", SIZE_MSG - 1);
    //because we need place for '\0' in the end of message

    struct message buf;
    int isWork = TRUE;
    do {
        size_t length;
        if(fgets(buf.mtext, SIZE_MSG, stdin) == NULL) {
            length = 0;
        } else {
            length = msgLength(buf.mtext);
        }

        if(updListeners(queue, pids) != EXIT_SUCCESS) {
            return EXIT_FAILURE;
        }

        if(!length) {
            isWork = FALSE;
        }

        if(sendMessage(queue, pids, &buf, length) != EXIT_SUCCESS) {
            return EXIT_FAILURE;
        }

    } while(isWork);

    return EXIT_SUCCESS;
}

int waitAnswers(int queue, struct array *pids) {
    struct message buf;

    while(!isEmpty(pids)) {
        if(msgrcv(queue, &buf, sizeof(pid_t), MSG_TYPE_END_ANSWER, NO_FLAGS) == ERROR) {
            perror("Can't get answer from listeners");
            return EXIT_FAILURE;
        }

        int index = find(pids, buf.pid);
        if(index == ERROR) {
            fprintf(stderr, "Del listener, which doesn't exits\n");
            return EXIT_FAILURE;
        }

        if(removeItem(pids, index) != EXIT_SUCCESS) {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

int main() {
    key_t queueKey = ftok("send.c", PROJECT_PREFIX);
    if(queueKey == ERROR) {
        perror("Can't generate key for queue");
        return EXIT_FAILURE;
    }

    int queue;
    queue = msgget(queueKey, IPC_CREAT | QUEUE_RIGHTS);
    if(queue == ERROR)     {
        perror("Can't make queue");
        return EXIT_FAILURE;
    }

    printf("Queue was created\n");

    struct array *pids = initArray();

    if(sendMessages(queue, pids) != EXIT_SUCCESS) {
        closeQueue(queue);
        destroyArray(pids);
        return EXIT_FAILURE;
    }

    if(waitAnswers(queue, pids) != EXIT_SUCCESS) {
        closeQueue(queue);
        destroyArray(pids);
        return EXIT_FAILURE;
    }

    if(closeQueue(queue) != EXIT_SUCCESS) {
        destroyArray(pids);
        return EXIT_FAILURE;
    }

    destroyArray(pids);
    return EXIT_SUCCESS;
}

