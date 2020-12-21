#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define SIZEMESS 256
typedef struct {
    long mtype;
    char mtext[SIZEMESS];
} message;

int main(int ac, char **av){

    message rcvbuf;
    int msgtype = 0 ;
    int msgid;  
    int rtn;

    if(-1 == (msgid = msgget(getuid(),IPC_EXCL)))
    {
	perror("msgget");
	exit(2);
    }

    strcpy(rcvbuf.mtext,"new");
    rcvbuf.mtype = 1;

    if(-1 == msgsnd ( msgid, &rcvbuf, 4, 0))
    {
	perror("msgsend");
        exit(1);
    }
				    
    msgrcv(msgid,&rcvbuf,SIZEMESS,2,0);
    msgtype =(int) (rcvbuf.mtext[0]);

    while(rtn=msgrcv(msgid,&rcvbuf,SIZEMESS,msgtype,0))
    {
	rcvbuf.mtext[rtn - 1] = '\0';
        if ( rtn == -1 ) 
	{
	    msgctl(msgid, IPC_RMID, 0);
	    break;	
	}
	else 
	{
	    if(!strcmp(rcvbuf.mtext,"quit"))
		exit(1);
    	}
        printf("Program %s(%d): %s",av[0],getpid(),rcvbuf.mtext);
    }
    return 0;
}

