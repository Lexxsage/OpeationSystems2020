#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
//#include <sys/ttydefaults.h> //for linux

#include "header.h"

int setNewAttr(int terminal, struct termios *newOption, struct termios *oldOption) {
    if (tcgetattr(terminal, oldOption) == ERROR) {
        perror("Error tcgetattr");
        return EXIT_FAILURE;
    }

    *newOption = *oldOption;
    newOption->c_lflag &= ~(ICANON | ECHO | ISIG);
    newOption->c_cc[VMIN] = COUNT_SYMBS;
    newOption->c_cc[VTIME] = 0;

    if (tcsetattr(terminal, TCSANOW, newOption) == ERROR) {
        perror("Error tcsetattr");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int setOldAttr(int terminal, struct termios *oldOption) {
    if (tcsetattr(terminal, TCSANOW, oldOption) == ERROR) {
        perror("Error tcsetattr");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int erase(int terminal, char *string, size_t *lengthString, char c) {
    if (*lengthString <= 0) {
        return EXIT_SUCCESS;
    }

    if(write(terminal, "\b \b", 3) == ERROR) {
        perror("Error write");
        return EXIT_FAILURE;
    }
    --*lengthString;


    return EXIT_SUCCESS;
}

int eraseWord(int terminal, char *string, size_t *lengthString, char c) {
    while (isspace(string[(*lengthString) - 1]) && *lengthString > 0) {
        if(erase(terminal, string, lengthString, c) != EXIT_SUCCESS) {
            perror("Error write");
            return EXIT_FAILURE;
        }
    }

    while (!isspace(string[(*lengthString) - 1]) && *lengthString > 0) {
        if(erase(terminal, string, lengthString, c) != EXIT_SUCCESS) {
            perror("Error write");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

int eraseString(int terminal, char *string, size_t *lengthString, char c) {
    while (*lengthString > 0) {
        if(write(terminal, "\b \b", 3) == ERROR) {
            perror("Error write");
            return EXIT_FAILURE;
        }
        --*lengthString;
    }

    return EXIT_SUCCESS;
}

int newLine(int terminal, char *string, size_t *lengthString, char c) {
    *lengthString = 0;
    if (write(terminal, NEW_LINE, strlen(NEW_LINE)) == ERROR) {
        perror("Error write");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int tune(int terminal, char *string, size_t *lengthString, char c) {
    if (write(terminal, TUNE, strlen(TUNE)) == ERROR) {
        perror("Error write");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int writeWithNewLine(int terminal, size_t *lengthString, char *currentString) {
    size_t valueIndex = *lengthString;
    while (' ' != currentString[valueIndex] && 0 != valueIndex) {
        --valueIndex;
    }

    if(0 == valueIndex && newLine(terminal, currentString, lengthString, '\n') == EXIT_SUCCESS) {
        return EXIT_SUCCESS;
    } else if(0 == valueIndex) {
        return EXIT_FAILURE;
    }

    for (size_t i = *lengthString; i != valueIndex; --i) {
        if (write(terminal, "\b \b", 3) == ERROR) {
            perror("Error write");
            return EXIT_FAILURE;
        }
    }

    if (write(terminal, NEW_LINE, strlen(NEW_LINE)) == ERROR) {
        perror("Error write");
        return EXIT_FAILURE;
    }

    *lengthString = 0;
    while (valueIndex != STRING_LENGTH) {
        ++valueIndex;
        if (write(terminal, &currentString[valueIndex], 1) == ERROR) {
            perror("Error write");
            return EXIT_FAILURE;
        }
        currentString[*lengthString] = currentString[valueIndex];
        ++*lengthString;
    }

    return EXIT_SUCCESS;
}

int printChar(int terminal, char *string, size_t *lengthString, char c) {
    string[*lengthString] = c;
    if (STRING_LENGTH == *lengthString) {
        writeWithNewLine(terminal, lengthString, string);
        return EXIT_SUCCESS;
    }

    ++*lengthString;
    if (write(terminal, &c, 1) == ERROR) {
        perror("Error write");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

strHandler getHandler(char funcName, struct termios *newOption) {
    if(CWERASE == funcName) {
        return eraseWord;
    }
    if('\n' == funcName) {
        return newLine;
    }
    if (newOption->c_cc[VKILL] == funcName) {
        return eraseString;
    }
    if (newOption->c_cc[VERASE] == funcName) {
        return erase;
    }
    if (!isprint(funcName)) {
        return tune;
    }

    return printChar;
}

int editor(int terminal, struct termios *newOption) {
    char buffer;
    ssize_t code;
    size_t lengthString = 0;
    char currentString[STRING_LENGTH] = {0};

    while((code = read(terminal, &buffer, 1)) > 0) {
        if(CEOT == buffer && lengthString == 0) {
            break;
        }

        strHandler handler = getHandler(buffer, newOption);
        if(handler(terminal, currentString, &lengthString, buffer) != EXIT_SUCCESS) {
            return EXIT_FAILURE;
        }
    }

    if(code == ERROR) {
        perror("Read error");
    }

    return EXIT_SUCCESS;
}

int main() {
    int terminal = open("/dev/tty", O_RDWR);
    if (terminal == ERROR) {
        perror("Error open terminal");
        return EXIT_FAILURE;
    }

    if(!isatty(terminal)) {
        perror("Open not terminal");
        close(terminal);
        return EXIT_FAILURE;
    }

    struct termios oldOption;
    struct termios newOption;
    if(setNewAttr(terminal, &newOption, &oldOption) != EXIT_SUCCESS) {
        close(terminal);
        return EXIT_FAILURE;
    }

    editor(terminal, &newOption);

    if(setOldAttr(terminal, &oldOption) != EXIT_SUCCESS) {
        close(terminal);
        return EXIT_FAILURE;
    }

    close(terminal);
    return EXIT_SUCCESS;
}
