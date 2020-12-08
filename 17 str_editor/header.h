#ifndef _INC_17_HEADER_H_
#define _INC_17_HEADER_H_

#include <stdlib.h>

#define ERROR -1
#define COUNT_SYMBS 1
#define STRING_LENGTH 40
#define NEW_LINE "\n"
#define TUNE "\7"

typedef int(*strHandler)(int terminal, char *string, size_t *lengthString, char c);

#endif //_INC_17_HEADER_H_

