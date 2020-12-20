#ifndef _INC_30_HEADER_H_
#define _INC_30_HEADER_H_

#define SIZE_MSG 100
#define ERROR -1
#define EQUALS_STR 0
#define NO_FLAGS 0
#define MSG_ALL_TYPES 0
#define MSG_TYPE_DATA 1
#define MSG_TYPE_END 2
#define MSG_TYPE_ANSWER 3
#define QUIT_MSG "quit"
#define LENGTH_QUIT_MSG 5
#define QUEUE_RIGHTS 0600
#define TRUE 1
#define FALSE 0
#define EMPTY 0

struct message {
    long mtype;
    char mtext[SIZE_MSG];
};

#endif //_INC_30_HEADER_H_

