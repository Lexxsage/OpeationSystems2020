#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>

#include "header.h"

struct FileLine {
    off_t offset;
    int length;
};

int input = 0;

int parseFile(struct FileLine *lines, int *countLine) {
    *countLine = 0;
    char c = 0;
    ssize_t code = 0;

    while((code = read(input, &c, sizeof(char))) > 0) {
        if(c != '\n') {
            ++lines[*countLine].length;
            continue;
        }

        ++*countLine;
        if(*countLine >= COUNT_LINES) {
            fprintf(stderr, "File too big\n");
            return ERROR_FILE_SIZE;
        }

        lines[*countLine].offset = lines[*countLine - 1].offset + lines[*countLine - 1].length + 1;
    }

    if(code == ERROR_READ_FILE) {
        perror("Can't read file");
        return ERROR_READ_FILE;
    }

    //++*countLine;
    return EXIT_SUCCESS;
}

void printAllString(int sig) {
    if(lseek(input, 0, SEEK_SET) == ERROR_READ_FILE) {
        perror("Can't read file");
    }

    char c = 0;
    ssize_t code = 0;
    while((code = read(input, &c, sizeof(char))) > 0) {
        printf("%c", c);
    }

    if(code == ERROR_READ_FILE) {
        perror("Can't read file");
    }
}

int printLines(int countLines, struct FileLine *lines) {
    printf("Enter number of string: (0 for exit)\n");

    int needLine = 0;
    char c = 0;
    int code = 0;

    if(signal(SIGALRM, printAllString) == SIG_ERR) {
        perror("Signal set error");
        return ERROR_SIGNAL;
    }
    alarm(TIME_WAIT);

    while((code = scanf("%d", &needLine)) == 1) { //1 - количество считанных аргументов
        alarm(0);
        if(needLine == 0) {
            return EXIT_SUCCESS;
        }

        if(needLine < 1 || needLine > countLines) {
            printf("Bad number!\n");
            alarm(TIME_WAIT);
            continue;
        }

        if(lseek(input, lines[needLine - 1].offset, SEEK_SET) == ERROR_READ_FILE) {
            perror("Can't read file");
            return ERROR_READ_FILE;
        }

        for(int i = 0; i < lines[needLine - 1].length; ++i) {
            if(read(input, &c, sizeof(char)) <= 0) {
                perror("Can't read file");
                return ERROR_READ_FILE;
            }

            printf("%c", c);
        }
        printf("\n");

        alarm(TIME_WAIT);
    }

    if(errno == EINTR) { //если выход из цикла произошел из-за сигнала SIGALRM
        return EXIT_SUCCESS;
    }

    if(code == 0) {
        fprintf(stderr, "Bad input\n");
        return BAD_INPUT;
    }

    perror("Scanf error");
    return EXIT_FAILURE;
}

int main(int argc, char **argv) {

    if(argc < 2) {
        fprintf(stderr, "Need filename\n");
        return BAD_ARGS;
    }

    input = open(argv[1], O_RDONLY);
    if(input == ERROR_OPEN_FILE) {
        perror("Open file error");
        return ERROR_OPEN_FILE;
    }

    struct FileLine lines[COUNT_LINES] = {};
    int countLines = 0;
    if(parseFile(lines, &countLines) != EXIT_SUCCESS) {
        close(input);
        return ERROR_READ_FILE;
    }

    if(countLines == 0) {
        fprintf(stderr, "Empty file\n");
        close(input);
        return FILE_IS_EMPTY;
    }

    if(printLines(countLines, lines) != EXIT_SUCCESS) {
        close(input);
        return ERROR_READ_FILE;
    }

    close(input);
    return EXIT_SUCCESS;
}
