#include <stdio.h>  // perror()
#include <stdlib.h> // EXIT_SUCCESS
#include "Search_Table.h"

#define END_LINE_NUMBER 0

// Loop proccessing of user input.
// Prints line by its number (starts form 1).
int print_table_console(int file_descriptor, Line_Record *search_table, unsigned search_table_size)
{
    printf("Lines range: [%d, %d]\n", 1, search_table_size);
    while (1)
    {
        int line_number, scanf_result;
        char ending;
        printf("$ ");
        scanf_result = scanf("%d%c", &line_number, &ending);

        if (scanf_result != 2 || ending != '\n')
        {
            fprintf(stderr, "Format Error\n");
            return EXIT_FAILURE;
        }

        if (line_number == END_LINE_NUMBER)
        {
            break;
        }
        if (line_number < 0 || line_number > search_table_size)
        {
            fprintf(stderr, "Out Of Range Error: line_number %d out of range [%d, %d]\n",
                    line_number, 1, search_table_size);
            continue;
        }

        print_line(file_descriptor, search_table[line_number]);
        putchar('\n');
    }

    return EXIT_SUCCESS;
}