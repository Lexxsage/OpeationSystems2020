#include <unistd.h> // __environ
#include <stdlib.h> // putenv()
#include <stdio.h>  //perror()

int initialize_custom_execvpe(const char *file, char *const argv[], char *const envp[]);

int main()
{
    char *file = "/bin/sh";
    char *argv[] = {file, "-c", "env", NULL};
    char *envp[] = {
        "PATH=/usr/bin",
        "KEY=VALUE",
        NULL};
    if (initialize_custom_execvpe(file, argv, envp) == -1)
    {
        perror("execvpe() Error: ");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int initialize_custom_execvpe(const char *file, char *const argv[], char *const envp[])
{
    unsigned i = 0;
    // Stupid way to drop old envp.
    while (__environ[i] != NULL)
    {
        __environ[i] = NULL;
        i++;
    }
    i = 0;
    while (envp[i] != NULL)
    {
        if (putenv(envp[i]) == -1)
        {
            return -1;
        }
        i++;
    }
    return execvp(file, argv);
}
