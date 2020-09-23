#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

typedef struct LinkedList
{
    char *value;
    struct LinkedList *next;
} LinkedList;

LinkedList *makeNode(char *value)
{
    LinkedList *newNode;
    if (!(newNode = (LinkedList *)malloc(sizeof(LinkedList))))
    {
        return NULL;
    }
    if (!(newNode->value = (char *)malloc(strlen(value) + 1)))
    {
        free(newNode);
        return NULL;
    }

    strcpy(newNode->value, value);
    newNode->next = NULL;

    return newNode;
}

/**
 * return: 0 if OK, -1 if error occured during the reading lines (Error NO MEMory)
*/
int readLinesToLList(LinkedList **head)
{
    char *buffer = malloc(BUFSIZ); // http://all-ht.ru/inf/prog/c/func/setbuf.html
    buffer[0] = '\0';
    LinkedList **curr_node = head;
    LinkedList *new_node;

    do
    {
        printf("> ");
        scanf("%s", buffer);
        if (!(new_node = makeNode(buffer)))
        {
            return -1;
        }
        *curr_node = new_node;
        curr_node = &(new_node->next);
    } while (buffer[0] != '.');

    free(buffer);
    return 0;
}

void printLines(LinkedList *head)
{
    for (LinkedList *curr = head; curr; curr = curr->next)
    {
        puts(curr->value);
    }
}

void disposeLList(LinkedList *head)
{
    LinkedList *curr = head, *next;
    while (curr)
    {
        next = curr->next;
        free(curr->value);
        free(curr);
        curr = next;
    }
}

int main(int argc, char **argv)
{
    LinkedList *head;

    if (readLinesToLList(&head) == -1)
    {
        perror(argv[0]);
        disposeLList(head);
        return ENOMEM;
    }

    printLines(head);
    disposeLList(head);
    return EXIT_SUCCESS;
}