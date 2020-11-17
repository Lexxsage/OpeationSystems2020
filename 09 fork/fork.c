#include <stdio.h>     // srderr
#include <errno.h>     // EINVAL
#include <stdlib.h>    // EXIT_FAILURE, EXIT_SUCCESS
#include <unistd.h>    // fork(), execl()
#include <wait.h>      // wait()
#include <sys/types.h> // pid_t

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s [file]\n", argv[0]);
        return EINVAL;
    }

    pid_t process_id = fork();
    // On failure, -1 is returned in the parent, no child process is created,
    // and errno is set appropriately.
    if (process_id == -1)
    {
        perror("Fork Error: ");
        return EXIT_FAILURE;
    }
    // On  success, the PID of the child process is returned in the parent, and 0 is
    // returned in the child.
    if (process_id == 0)
    {
        execl("/bin/cat", "cat", argv[1], NULL);
        return EXIT_SUCCESS;
    }

    // The wait() system call suspends execution of the calling process until one of
    // its children terminates.
    pid_t ended_process_id = wait(NULL);
    if (ended_process_id == -1)
    {
        perror("Timeout Error: ");
        return EXIT_FAILURE;
    }

    printf("\nChild proccess terminated successfully! Exiting...\n");
    return EXIT_SUCCESS;
}
