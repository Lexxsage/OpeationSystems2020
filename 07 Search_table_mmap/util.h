#ifndef _INC_4_UTIL_H_
#define _INC_4_UTIL_H_

#include <stdlib.h>

struct String {
    char *str;
    size_t length;
    size_t currentPos;
};

void * checkMalloc(size_t size);
void * checkRealloc(void *ptr, size_t size);
int extendStr(struct String *string);
int addChar(struct String *string, char c);


#endif //_INC_4_UTIL_H_

