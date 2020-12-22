#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define SIZEMESS 256
typedef struct {
    long mtype;
    long mnum;
    char mtext[SIZEMESS];
} message;

int main(int ac, char **av){

    message sndbuf;
    int msgtype = 0 ;
    int msgid;  
    int rtn;
    int readCount;
    char buf[SIZEMESS];
    int i;
						 
    if(-1 == (msgid = msgget(getuid(),0)))
    {
	perror("msgget");
	exit(2);
    }

    strcpy(sndbuf.mtext,"new");
    sndbuf.mtype = 1;
    sndbuf.mnum = getpid();

    if(-1 == msgsnd ( msgid, &sndbuf, 12, 0))
    {
	perror("msgsend");
        exit(1);
    }
				    
    while(1)
    {
	if(!fgets(buf,SIZEMESS,stdin))
        {
            strcpy(sndbuf.mtext, "end");
            sndbuf.mtype = 1;
            msgsnd ( msgid, &sndbuf, 12, 0);
            break;
        }
        else
        {
            strcpy(sndbuf.mtext, buf);
            sndbuf.mtype = 1;
//	    printf("%d -- %d\n",getpid(),);
            msgsnd ( msgid, &sndbuf, strlen(buf) + 1 + 8, 0);
        }
        
    }
    return 0;
}
