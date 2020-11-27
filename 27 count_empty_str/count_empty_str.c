#include <stdlib.h> // exit(), EXIT_SUCCESS, EXIT_FAILURE
#include <stdio.h>  // popen(), pclose()
#include <string.h> // strcat()

void close_pipe(FILE *pipe)
{
    int status = pclose(pipe);
    if (status == -1)
    {
        perror("pclose");
        exit(EXIT_FAILURE);
    }
    else if (WEXITSTATUS(status) != 0)
    {
        perror("child process returned bad exit status");
    }
    else if (WIFSIGNALED(status) != 0)
    {
        perror("child process interrupted by signal");
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s [filename]\n", argv[0]);
        return EXIT_FAILURE;
    }

    // ============= Building command. ==================================================
    char cmd[256];
    cmd[0] = '\0';
    strcat(cmd, "grep -r ^$ ");
    strcat(cmd, argv[1]);
    strcat(cmd, " | wc -l");
   
    // ============= Opening pipe. ======================================================
    FILE *pipe = popen(cmd, "r");
    if (pipe == NULL)
    {
        perror("popen");
        return EXIT_FAILURE;
    }
   
    // ========= Obtaining, printing result. ============================================
    unsigned result;
    if (EOF == fscanf(pipe, "%ul", &result))
    {
        perror("fscanf");
        close_pipe(pipe);
        return EXIT_FAILURE;
    }
    printf("Result is: %d\n", result);
   
    close_pipe(pipe);

    return EXIT_SUCCESS;
}
