#include <stdio.h>
#include <libgen.h>
#include <stdlib.h>
#include <time.h>

#define MAX 100
#define NUM_INT 100
#define INT_PER_LINE 10
#define ERROR -1
#define READ 1
#define WRITE 0

int main() {
    FILE *fd[2];
    if (p2open("sort -bn", fd) == ERROR) {
        perror("Can't make pipe");
        return EXIT_FAILURE;
    }

    time_t currentTime = time(NULL);
    if(currentTime == ERROR) {
        perror("Can't get time");
       (void)p2close(fd);
        return EXIT_FAILURE;
    }

    srand(currentTime);
    int i;
    for (i = 0; i < NUM_INT ; ++i) {
        fprintf(fd[WRITE], "%d\n", rand() % MAX);
    }
    fclose(fd[WRITE]);

    int j;
    for (i = 0; i < NUM_INT; i+=INT_PER_LINE) {
        for (j = 0; j < INT_PER_LINE; ++j) {
            int val;
            fscanf(fd[READ], "%d", &val);
            printf("%2d ", val);
        }
        printf("\n");
    }
    fclose(fd[READ]);

    return EXIT_SUCCESS;
}

