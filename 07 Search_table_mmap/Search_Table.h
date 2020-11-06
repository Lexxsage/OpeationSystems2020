#ifndef INC_05_SEARCH_TABLE_SEARCH_TABLE_H
#define INC_05_SEARCH_TABLE_SEARCH_TABLE_H

#include <sys/types.h>

typedef struct Line_Record
{
    off_t offset;
    unsigned length;
} Line_Record;

int build_search_table(int file_descriptor, Line_Record *search_table, unsigned max_size);
void print_line(int file_descriptor, Line_Record line_record);

#endif //INC_05_SEARCH_TABLE_SEARCH_TABLE_H