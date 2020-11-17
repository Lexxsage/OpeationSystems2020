#include <stdio.h>     // printf(), perror()
#include <stdlib.h>    // EXIT_FAILURE, EXIT_SUCCESS
#include <sys/types.h> // pid_t
#include <wait.h>      // wait()
#include <unistd.h>    // fork(), execvp()

#define CHILD_ID 0

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s [executable] <args>\n", argv[0]);
        return EXIT_FAILURE;
    }

    pid_t process_id = fork();

    if (process_id == -1)
    {
        perror("Fork error: ");
        return EXIT_FAILURE;
    }

    if (process_id == CHILD_ID)
    {
        // The  execv(),  execvp(), and execvpe() functions provide an array of pointers
        // to null-terminated strings that represent the argument list available to the
        // new program.
        // The first argument, by convention, should point to the filename associated
        // with the file being executed.
        // The array of pointers must be terminated by a null pointer.
        execvp(argv[1], &argv[1]);
        return EXIT_SUCCESS;
    }

    int status;
    // The wait() system call suspends execution of the calling process until one of
    // its children terminates.
    pid_t ended_process_id = wait(&status);

    if (ended_process_id == -1)
    {
        perror("Waiting for completion of subprocess");
        return EXIT_FAILURE;
    }

    // WIFEXITED(wstatus) returns true if the child terminated normally, that is, by
    // calling exit(3) or _exit(2), or by returning from main().
    if (WIFEXITED(status))
    {
        // WEXITSTATUS(wstatus) returns the exit status of the child
        printf("Child process %d ended by exit, return's code: %d\n",
               ended_process_id, WEXITSTATUS(status));
        return EXIT_SUCCESS;
    }

    //  WIFSIGNALED(status) returns true if the child process was terminated by a signal.
    if (WIFSIGNALED(status))
    {
        // WTERMSIG(wstatus) returns the number of the signal that caused the child
        // process to terminate.
        printf("Child process %d ended by signal, signal: %d\n",
               ended_process_id, WTERMSIG(status));
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}
