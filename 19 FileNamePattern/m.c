#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>

#define MASK_SIZE (300)

void simplifyMask(char* const mask)
{
    int i = 0;
    int count = 1;
    int len = strlen(mask);
    for (i = 1; i < len; i++)
        if (!('*' == mask[i - 1]) || !('*' == mask[i]))
	    mask[count++] = mask[i];
    mask[count] = 0;
}

int cellExists(const int x, const int y)
{
    return (0 <= x && 0 <= y);
}

int match(const char* const word, const char* const mask)
{
    int a[MASK_SIZE][FILENAME_MAX];
    int i = 0;
    int j = 0;
    int wordLen = strlen(word);
    int maskLen = strlen(mask);
    
    if (!wordLen || !maskLen)
	return 0;

    for (i = 0; i < wordLen; i++)
	for (j = 0; j < maskLen; j++)
	    a[i][j] = 0;

    a[0][0] = ('*' == mask[0] || '?' == mask[0] || word[0] == mask[0]);
    for (i = 0; i < wordLen; i++)
    	for (j = 0; j < maskLen; j++)
	{
	    if (!i && !j)
		continue;
	    switch (mask[j])
	    {
		case '?':
		    a[i][j] = (cellExists(i - 1, j - 1) && a[i - 1][j - 1]) || (0 == i && 1 == j && '*' == mask[j - 1]);
		    break;
		case '*':
		    a[i][j] = (cellExists(i - 1, j) && a[i - 1][j]) || (cellExists(i, j - 1) && a[i][j - 1]);
		    break;
		default:
		    a[i][j] = (word[i] == mask[j])  && ((cellExists(i - 1, j - 1) && a[i - 1][j - 1]) || (0 == i && 1 == j && '*' == mask[j - 1]));
		    break;
		}
	    }
    return a[wordLen - 1][maskLen - 1];
}

int main(void)
{
    DIR* dir = NULL;
    struct dirent* dirRecord = NULL;
    char mask[MASK_SIZE] = {0};
    char oldMask[MASK_SIZE] = {0};
    int isMatched = 0;
    int i=0;
    
    puts("Enter mask:");
    fgets(mask, MASK_SIZE, stdin);
    if('\n' == mask[strlen(mask) - 1])
    	mask[strlen(mask) - 1] = 0;

    if(NULL != strchr(mask, '/'))
    {
    	fprintf(stderr, "%s\n", "Illegal mask. There is '/' symbol.");
	return 2;
    }

    dir = opendir(".");
    if(NULL == dir)
    {
    	perror("It's impossible to open this directory to search");
    	return 1;
    }

    strcpy(oldMask, mask);
    simplifyMask(mask);

    while(NULL != (dirRecord = readdir(dir)))
	if(match(dirRecord->d_name, mask))
	{
	    printf("match %d: ",++i);
	    puts(dirRecord->d_name);
	    isMatched = 1;
			    
	}
    if(!isMatched)
    	printf("%s not found!\n",oldMask);

    return 0;
}