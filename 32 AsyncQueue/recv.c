#include <sys/types.h>
#include <signal.h>
#include <termios.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define SIZEMESS 256
typedef struct {
    long mtype;
    long mnum;
    char mtext[SIZEMESS];
} message;
	
 
int main(int ac,char **av)
{

    int msgid;
    int rtn = 0;
    int readCount = 0 ;
    int count = 0;
    char buf[SIZEMESS];
    struct timeval tv;
    int retval;
    message rcvbuf,sndbuf;
    
    int i;
    int countMesg = 0;

    if( -1 ==(msgid =msgget(getuid(),IPC_CREAT|0660)))
    {
	perror("msgget");
        return -1;
    }

    while(1)
    {
	rtn=msgrcv(msgid,&rcvbuf,SIZEMESS+8,1,IPC_NOWAIT);
	
	if(rtn!= -1)
	{
	    if(!strcmp(rcvbuf.mtext,"new"))
	    {
//		printf("new\n");
	        count++;
	    }
	    else if(!strcmp(rcvbuf.mtext,"end"))
	    {
//		printf("no new\n");
		if(--count==0)
		    break;
	    }
	    else
	    {
		printf("Process %d: %s",rcvbuf.mnum,rcvbuf.mtext);
	    }
	}
    }
    msgctl(msgid, IPC_RMID, 0);
    return 0;
}
