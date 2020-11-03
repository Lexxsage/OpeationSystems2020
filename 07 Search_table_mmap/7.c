#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>

#include <sys/mman.h>

#include "header.h"

struct FileLine {
    off_t offset;
    int length;
};

char *mappingFile =  NULL;
size_t fileLength = 0;

int parseFile(struct FileLine *lines, int *countLine) {
    *countLine = 0;

    for(size_t i = 0; i < fileLength; ++i) {
        if(mappingFile[i] != '\n') {
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

    //++*countLine;
    return EXIT_SUCCESS;
}

void printAllString(int sig) {
    printf("%s", mappingFile);
//    for(int i = 0; i < fileLength; ++i) {
//        printf("%c", mappingFile[i]);
//    }
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

        for(off_t i = lines[needLine - 1].offset, count = i + lines[needLine - 1].length; i < count; ++i) {
            printf("%c", mappingFile[i]);
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

void * mapping(char *file) {
    int input = open(file, O_RDONLY);
    if(input == ERROR_OPEN_FILE) {
        perror("Open file error");
        return NULL;
    }

    off_t offset = lseek(input, 0, SEEK_END);
    if(offset == ERROR_READ_FILE) {
        perror("Can't read file");
        return NULL;
    }

    fileLength = (size_t)offset;
    char *ptr = mmap(NULL, fileLength, PROT_READ, MAP_PRIVATE, input, 0);
    if(ptr == MAP_FAILED) {
        perror("Error mapping");
        return NULL;
    }

    close(input);

    return ptr;
}

int main(int argc, char **argv) {
    if(argc < 2) {
        fprintf(stderr, "Need filename\n");
        return BAD_ARGS;
    }

    mappingFile = mapping(argv[1]);
    if(mappingFile == NULL) {
        return ERROR_OPEN_FILE;
    }

    struct FileLine lines[COUNT_LINES] = {};
    int countLines = 0;
    if(parseFile(lines, &countLines) != EXIT_SUCCESS) {
        return ERROR_READ_FILE;
    }

    if(countLines == 0) {
        fprintf(stderr, "Empty file\n");
        return FILE_IS_EMPTY;
    }

    if(printLines(countLines, lines) != EXIT_SUCCESS) {
        return ERROR_READ_FILE;
    }

    return EXIT_SUCCESS;
}
