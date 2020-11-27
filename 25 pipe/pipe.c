#include <stdio.h>     // printf()
#include <string.h>    // strlen()
#include <stdlib.h>    // EXIT_SUCCESS, EXIT_FAILURE
#include <sys/types.h> // pid_t
#include <ctype.h>     // toupper()
#include <unistd.h>    // fork(), write(), read(), pipe()

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s [string]\n", argv[0]);
        return EXIT_FAILURE;
    }

    int pipe_ends[2];
    if (pipe(pipe_ends) == -1)
    {
        perror("fork");
        return EXIT_FAILURE;
    }

    char str[100];

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        close(pipe_ends[0]);
        close(pipe_ends[1]);
        return EXIT_FAILURE;
    }
    // ====== Parent process. ===========================================================
    else if (pid > 0)
    {
        close(pipe_ends[0]);
        strcpy(str, argv[1]);
        if (write(pipe_ends[1], str, strlen(str) + 1) == -1)
        {
            perror("write");
            close(pipe_ends[1]);
            return EXIT_FAILURE;
        }
        printf("Parent: sent \"%s\" \n", str);
        close(pipe_ends[1]);
    }
   
    // ====== Child process. ============================================================
    else if (pid == 0)
    {
        close(pipe_ends[1]);
        if (read(pipe_ends[0], str, 100) == -1)
        {
            perror("read");
            close(pipe_ends[0]);
            return EXIT_FAILURE;
        }
        printf("Child: received \"%s\" \n", str);
        for (int i = 0; i < strlen(str); i++)
        {
            str[i] = (char)toupper(str[i]);
        }
        printf("Child: upper \"%s\" \n", str);
        close(pipe_ends[0]);
    }
   
    return EXIT_SUCCESS;
}
