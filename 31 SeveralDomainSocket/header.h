#ifndef _INC_31_HEADER_H_
#define _INC_31_HEADER_H_

#define SIZE_MSG 100
#define ERROR -1
#define EQUALS_STR 0
#define NO_FLAGS 0
#define MSG_TYPE_DATA 1
#define MSG_TYPE_REG 2
#define MSG_TYPE_END 3
#define MSG_TYPE_END_ANSWER 4
#define QUIT_MSG "quit"
#define LENGTH_QUIT_MSG 5
#define QUEUE_RIGHTS 0600
#define PROJECT_PREFIX 123
#define ARRAY_RATIO_SIZE 2
#define TRUE 1
#define FALSE 0

struct message {
    long mtype;
    pid_t pid;
    char mtext[SIZE_MSG];
};

struct array {
    pid_t *arr;
    int indexForAdd;
    int size;
};

#endif //_INC_31_HEADER_H_

