#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int open_file(char *filename)
{
    if (access(filename, F_OK) == -1)
    {
        perror(NULL);
        return ENOENT; //error no entry
    }

    FILE *file = fopen(filename, "w"); // w - создает текстовый файл для записи
    if (!file)
    {
        perror(NULL);
        return errno; //code of the last error
    }
    printf("Open file: success.\n");
    fclose(file);

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s [file]\n", argv[0]);
        return EXIT_FAILURE;
    }

    printf("uid = %d, ueid = %d\n", getuid(), geteuid());

    open_file(argv[1]);

    if (setuid(geteuid()) == -1)
    {
        fprintf(stderr, "Can't change euid\n");
        perror(NULL);
        return errno;
    }
    printf("Change euid: success.\n");
    printf("uid = %d, ueid = %d\n", getuid(), geteuid());

    open_file(argv[1]);

    return EXIT_SUCCESS;
}