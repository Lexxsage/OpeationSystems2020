#include <fcntl.h>   // open()
#include <unistd.h>  // write(), read(), isatty()
#include <termios.h> // tcgetattr(), tcsetattr(), termios
#include <string.h>  // strlen()
#include <stdio.h>   // printf()
#include <ctype.h>   // tolower()
#include <stdlib.h>  // EXIT_SUCCESS, EXIT_FAILURE

// https://ejudge.ru/study/3sem/term.pdf - useful.

int main(int argc, char **argv)
{
    // ====== Opening terminal. =========================================================
    int terminal_fd = open("/dev/tty", O_RDWR);
    if (terminal_fd == -1)
    {
        perror("Failed to open() '/dev/tty': ");
        return EXIT_FAILURE;
    }
    if (!isatty(terminal_fd))
    {
        perror("'/dev/tty' is not terminal: ");
        return EXIT_FAILURE;
    }
    // ----------------------------------------------------------------------------------

    // ====== Changing terminal attr. ===================================================
    struct termios old_terminal_attr, new_terminal_attr;
    if (tcgetattr(terminal_fd, &old_terminal_attr) == -1)
    {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }
    new_terminal_attr = old_terminal_attr;

    // Canon mode - append user input by lines.
    // Not canon mode - append user input by groups of 'VMIN' symbols.
    new_terminal_attr.c_lflag &= ~ICANON;
    new_terminal_attr.c_cc[VMIN] = 1;

    if (tcsetattr(terminal_fd, TCSANOW, &new_terminal_attr) == -1)
    {
        perror("tcsetattr");
        return EXIT_FAILURE;
    }
    // ----------------------------------------------------------------------------------

    // ====== Asking confirmation and handling it. ======================================
    char *message = "Confirm? [Y/n] ";
    if (write(STDOUT_FILENO, message, strlen(message)) == -1)
    {
        perror("write");
        tcsetattr(terminal_fd, TCSANOW, &old_terminal_attr);
        return EXIT_FAILURE;
    }

    char answer;
    if (read(STDIN_FILENO, &answer, 1) == -1)
    {
        perror("read");
        tcsetattr(terminal_fd, TCSANOW, &old_terminal_attr);
        return EXIT_FAILURE;
    }
    switch (tolower(answer))
    {
    case 'y':
        printf("\nYou answered 'Yes'\n");
        break;
    case 'n':
        printf("\nYou answered 'No'\n");
        break;
    default:
        printf("\nUndefined answer\n");
        break;
    }
    // ----------------------------------------------------------------------------------

    tcsetattr(terminal_fd, TCSANOW, &old_terminal_attr);
    return EXIT_SUCCESS;
}
