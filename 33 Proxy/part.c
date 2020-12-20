
#include <stdio.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#include "header.h"

int main(int argc, char **argv) {
    if(argc < 3) {
        fprintf(stderr, "Need name of part and count of widgets\n");
        return EXIT_FAILURE;
    }

    char partName = tolower(argv[1][0]);
    if(partName < 'a' || partName > 'c') {
        fprintf(stderr, "Available names of parts: a,b or c\n");
        return EXIT_FAILURE;
    }

    int widgetCount;
    if(sscanf(argv[2], "%d", &widgetCount) != 1) {
        fprintf(stderr, "Bad format of count widgets\n");
        return EXIT_FAILURE;
    }

    key_t semKey = ftok("factory.c", PROJECT_PREFIX);
    if(semKey == ERROR) {
        perror("Can't generate key for semaphore");
        return EXIT_FAILURE;
    }

    int sem = semget(semKey, COUNT_PARTS, NO_FLAGS);
    if(sem == ERROR) {
        perror("Can't get semaphore");
        return EXIT_FAILURE;
    }

    int times[] = {TIME_A, TIME_B, TIME_C};
    int index = partName - 'a';
    struct sembuf part = {index, POST, SEM_UNDO};

    printf("%ccount: %d\n", partName,  widgetCount);

    int i;
    for(i = 0; i < widgetCount; ++i) {
        sleep(times[index]);

        printf("Part %c number %d was made\n", partName, i+1);
        fflush(stdout);

        if(semop(sem, &part, 1) == ERROR) {
            perror("Can't make part");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

