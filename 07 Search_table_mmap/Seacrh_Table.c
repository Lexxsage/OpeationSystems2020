#include <unistd.h>   // lseek
#include <stdio.h>    // perror
#include <stdlib.h>   // exit
#include <sys/mman.h> // mmap
#include <sys/stat.h> // fstat
#include "Search_Table.h"

off_t get_file_size(int file_descriptor)
{
    struct stat file_stat;
    if (fstat(file_descriptor, &file_stat) == -1)
    {
        {
            perror("fstat() Error: ");
            exit(EXIT_FAILURE);
        }
    }
    return file_stat.st_size;
}

char *map_whole_file(int file_descriptor, off_t file_size)
{
    char *file_map = mmap(NULL, file_size, PROT_READ, MAP_SHARED, file_descriptor, 0L);
    if (file_map == MAP_FAILED)
    {
        perror("mmap() Error: ");
        exit(EXIT_FAILURE);
    }

    return file_map;
}

// Returns size of result search table.
int build_search_table(int file_descriptor, Line_Record *search_table, unsigned max_size)
{
    off_t file_size = get_file_size(file_descriptor);
    char *file_map = map_whole_file(file_descriptor, file_size);
    unsigned current_line = 1;
    unsigned current_line_position = 0;
    off_t global_position = 0L;
    search_table[1].offset = 0L;

    for (global_position = 0; global_position < file_size; global_position++)
    {
        current_line_position++;
        if (file_map[global_position] == '\n')
        {
            search_table[current_line].length = current_line_position;
            search_table[current_line + 1].offset = global_position + 1;
            current_line_position = 0;
            current_line++;
        }
    }

    search_table[current_line].length = current_line_position;
    if (munmap(file_map, file_size) == -1)
    {
        perror("mmap() Error:");
        exit(EXIT_FAILURE);
    }

    return current_line;
}

void print_line(int file_descriptor, Line_Record line_record)
{
    char *file_map = map_whole_file(file_descriptor, get_file_size(file_descriptor));

    unsigned i;
    for (i = 0; i < line_record.length; i++)
    {
        printf("%c", file_map[line_record.offset + i]);
    }

    if (file_map == MAP_FAILED)
    {
        perror("mmap() Error: ");
        exit(EXIT_FAILURE);
    }
}
