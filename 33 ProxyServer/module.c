#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>

#include "header.h"

int main(int argc, char **argv) {
    if(argc < 3) {
        fprintf(stderr, "Need name of part and count of widgets\n");
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

    struct sembuf partAB[2] = {
            {PART_A, WAIT, SEM_UNDO},
            {PART_B, WAIT, SEM_UNDO}
    };
    struct sembuf module = {PART_MODULE, POST, SEM_UNDO};

    printf("Mcount: %d\n", widgetCount);

    int i;
    for(i = 0; i < widgetCount; ++i)
    {
        if(semop(sem, partAB, 2) == ERROR) {
            perror("Can't take part A and B");
            return EXIT_FAILURE;
        }

        printf("Module %d was made\n", i + 1);
        fflush(stdout);

        if(semop(sem, &module, 1) == ERROR) {
            perror("Can't post module");
            return EXIT_FAILURE;
        }
    }
}

