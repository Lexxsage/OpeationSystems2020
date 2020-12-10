#include <stdio.h>
#include <signal.h>
#include <termios.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>

#define ERROR -1
#define TUNE "\7"

int count;

struct termios oldOption;
int terminal;

void sigcatch(int sig) {
	printf("Count = %d\n",count);

	setOldAttr(terminal, &oldOption);
    close(terminal);
	exit(0);
}

void sigDel(int sig) {
	if (write(terminal, TUNE, strlen(TUNE)) == ERROR) {
        perror("Error write");
        exit(EXIT_FAILURE);
    }
	++count;

	signal(SIGINT, sigDel);
}

int setNewAttr(int terminal, struct termios *oldOption) {
	struct termios newOption;
    if (tcgetattr(terminal, oldOption) == ERROR) {
        perror("Error tcgetattr");
        return EXIT_FAILURE;
    }

    newOption = *oldOption;
    newOption.c_lflag &= ~ECHO;
    newOption.c_cc[VINTR] = 27;

    if (tcsetattr(terminal, TCSANOW, &newOption) == ERROR) {
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

int main() {
	terminal = open("/dev/tty", O_RDWR);
    if (terminal == ERROR) {
        perror("Error open terminal");
        return EXIT_FAILURE;
    }

    if(!isatty(terminal)) {
        perror("Open not terminal");
        close(terminal);
        return EXIT_FAILURE;
    }

    if(setNewAttr(terminal, &oldOption) != EXIT_SUCCESS) {
        close(terminal);
        return EXIT_FAILURE;
    }

    signal(SIGINT, sigDel);
    signal(SIGQUIT,sigcatch);

    for(;;);

	return EXIT_SUCCESS;
}

