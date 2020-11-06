#include <stdio.h>  // perror
#include <errno.h>  // error codes
#include <fcntl.h>  // open
#include <stdlib.h> // malloc, free
#include "Search_Table.h"

#define LINES_COUNT_MAX 100

int print_table_console(int file_descriptor, Line_Record *search_table,
                        unsigned search_table_size);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s [file]\n", argv[0]);
        return EINVAL;
    }

    Line_Record *search_table = malloc(sizeof(Line_Record) * (LINES_COUNT_MAX + 1));

    int file_descriptor = open(argv[1], O_RDONLY | O_NDELAY);
    if (file_descriptor == -1)
    {
        perror(argv[0]);
        free(search_table);
        return EIO;
    }

    if (build_search_table == -1){
        free(search_table);
        close(file_descriptor);
        exit(EXIT_FAILURE);
    } else {
        int search_table_size =
                build_search_table(file_descriptor, search_table, LINES_COUNT_MAX);
        int exit_code =
                print_table_console(file_descriptor, search_table, search_table_size);

        free(search_table);
        close(file_descriptor);
        return exit_code;
    }
}
