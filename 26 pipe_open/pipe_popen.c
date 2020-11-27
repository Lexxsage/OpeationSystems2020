#include <stdlib.h> // exit(), EXIT_SUCCESS, EXIT_FAILURE
#include <stdio.h>  // popen(), pclose(), fread(), fwrite(), stdout
#include <ctype.h>  // toupper()

#define COMMAND_TO_WRITE_TEXT "echo SoMe TeXt"
#define BUFFER_SIZE 256

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
    // ============= Opening pipe. ======================================================
    FILE *pipe = popen(COMMAND_TO_WRITE_TEXT, "r");
    if (pipe == NULL)
    {
        perror("popen");
        return EXIT_FAILURE;
    }
   
    // ========= Reading from stdout of writing process. ================================
    char buf[BUFSIZ];
    int bytes_read = fread(buf, 1, BUFSIZ, pipe);
    if (bytes_read == -1)
    {
        perror("fread");
        close_pipe(pipe);
        return EXIT_FAILURE;
    }
   
    close_pipe(pipe);

    // ========= Converting to upper, writing into stdout. ==============================
    unsigned i;
    for (i = 0; i < bytes_read; i++)
    {
        buf[i] = toupper(buf[i]);
    }
    
    if (fwrite(buf, bytes_read, 1, stdout) == -1)
    {
        perror("fwrite");
        return EXIT_FAILURE;
    }
   
    return EXIT_SUCCESS;
}
