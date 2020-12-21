#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include "header.h"

int delSem(int sem) {
    if(semctl(sem, DUMMY, IPC_RMID, DUMMY) == ERROR) {
        perror("Can't del semaphore");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int startPart(char *executable, char *partName, char *widgetCount) {
    pid_t pid = fork();
    if(pid == ERROR) {
        perror("Can't start part");
        return ERROR;
    }

    if(pid != CHILD) {
        return pid;
    }

    execl(executable, executable, partName, widgetCount, NULL);
    perror("Can't execute part");
    return ERROR;
}

int getWidgetCount() {
    printf("Enter widget count: ");
    int res;

    while((scanf("%d", &res) != 1 && !scanf("%*s")) || res < 1) {
        printf("Enter correct value: ");
    }

    return res;
}

int factoryLoop(int sem, int widgetCount) {
    struct sembuf makeWidget[2] = {
            {PART_C, WAIT, SEM_UNDO},
            {PART_MODULE, WAIT, SEM_UNDO}
    };
    
    int i;
    for(i = 0; i < widgetCount; ++i) {
        if(semop(sem, makeWidget, 2) == ERROR) {
            perror("Can't make widget");
            return EXIT_FAILURE;
        }

        printf("Widget %d is done\n", i+1);
        fflush(stdout);
    }

    return EXIT_SUCCESS;
}

int main() {
    int widgetCount = getWidgetCount();
    char strCount[INT_LENGTH] = {};
    sprintf(strCount, "%d", widgetCount);

	key_t semKey = ftok("factory.c", PROJECT_PREFIX);
    if(semKey == ERROR) {
		perror("Can't generate key for semaphore");
		return EXIT_FAILURE;
	}

	int sem = semget(semKey, COUNT_PARTS, IPC_CREAT | ACCESS_RIGHTS);
	if(sem == ERROR) {
		perror("Can't get semaphore");
		return EXIT_FAILURE;
	}	

    if(startPart("part", "a", strCount) == ERROR) {
        delSem(sem);
        return EXIT_FAILURE;
    }
    if(startPart("part", "b", strCount) == ERROR) {
        delSem(sem);
        return EXIT_FAILURE;
    }
    if(startPart("part", "c", strCount) == ERROR) {
        delSem(sem);
        return EXIT_FAILURE;
    }
    if(startPart("module", "", strCount) == ERROR) {
        delSem(sem);
        return EXIT_FAILURE;
    }

    if(factoryLoop(sem, widgetCount) != EXIT_SUCCESS) {
        delSem(sem);
        return EXIT_FAILURE;
    }

    if(delSem(sem) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

