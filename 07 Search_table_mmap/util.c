#include "util.h"

#include <stdio.h>
#include <string.h>

#include "header.h"

void * checkMalloc(size_t size) {
    void *ptr = malloc(size);

    if(ptr == NULL) {
        perror("Malloc error");
    }

    return ptr;
}

void * checkRealloc(void *ptr, size_t size) {
    ptr = realloc(ptr, size);

    if(ptr == NULL) {
        perror("Realloc error");
    }

    return ptr;
}

int extendStr(struct String *string) {
    size_t needLength = string->length == 0 ? DEFAULT_STRING_LENGTH : sizeof(char) * string->length * BUFFER_RATIO;

    char *newStr = (char *)checkRealloc(string->str, needLength);
    if(!newStr) {
        fprintf(stderr, "Error: Can't save string\n");
        return MALLOC_ERROR;
    }

    string->length = needLength;
    string->str = newStr;

    return EXIT_SUCCESS;
}

int addChar(struct String *string, char c) {
    if(string->currentPos < string->length) {
        string->str[string->currentPos++] = c;
        return EXIT_SUCCESS;
    }

    if(extendStr(string) != 0) {
        return SAVE_STR_ERROR;
    }
    string->str[string->currentPos++] = c;

    return EXIT_SUCCESS;
}
