#include <stdio.h>  // perror()
#include <stdlib.h> // EXIT_SUCCESS
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h> 
#include "Search_Table.h"

#define END_LINE_NUMBER 0

int is_timeout = 0;
void on_alarm(int code)
{
    is_timeout = 1;
}

// Loop proccessing of user input.
// Prints line by its number (starts form 1).
int print_table_console(int file_descriptor, Line_Record *search_table,
                        unsigned search_table_size)
{
    fd_set rfds;
    struct timeval tv;
    int retval;
    int max_fd = 0;
    /* wait for lines number*/
    printf("Lines range: [%d, %d]\n", 1, search_table_size);
    FD_SET(0, &rfds);
    /* Ждем не больше пяти секунд. */
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    retval = select(max_fd + 1, &rfds, NULL, NULL, &tv);
    /* Не полагаемся на значение tv! */
    if (retval)
    {
      while (1)
      {
        int line_number, scanf_result;
        char ending;
        printf("$ ");
        // On alarm, stops to wait user input.
        scanf_result = scanf("%d%c", &line_number, &ending);
        // User entered line number faster than SECOND_TO_COUNT.
        // So alarm counting must to be stoped.
        alarm(0);
        if (is_timeout)
        {
            for (unsigned i = 0; i < search_table_size + 1; i++)
            {
                print_line(file_descriptor, search_table[i]);
                putchar('\n');
            }
            exit(EXIT_SUCCESS);
        }

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
    else
        printf("Данные не появились в течение пяти секунд.\n");
    return 0;
	
	

   
}
