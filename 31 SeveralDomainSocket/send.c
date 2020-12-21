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
    char mtext[SIZEMESS];
} message;
	
 
int main(int ac,char **av)
{

    int msgid;
    int rtn = 0;
    int readCount = 0 ;
    int count = 2;
    char buf[SIZEMESS];
    struct timeval tv;
    int retval;

    message rcvbuf,sndbuf;
    int fd;
    int i;
    int countMesg = 0;

    fd = open("/dev/tty", O_RDWR);
    if( -1 ==(msgid =msgget(getuid(),IPC_CREAT|0660)))
    {
	perror("msgget");
        return -1;
    }

    while(1)
    {
	fd_set set;
        struct timeval tv;tv.tv_sec = 0;tv.tv_usec = 50;
        FD_ZERO(&set);
        FD_SET(1,&set);
        select(2,&set,NULL,NULL,&tv);
    	
	rtn=msgrcv(msgid,&rcvbuf,SIZEMESS,1,IPC_NOWAIT);
        
	if(rtn != -1)
	{
	    if(!strcmp(rcvbuf.mtext,"new"))
	    {
	        count++;
	        sndbuf.mtext[0] = count;
		sndbuf.mtype = 2; 
		msgsnd ( msgid, &sndbuf, 2, 0);
	    }
	}

        if(FD_ISSET(1,&set))
	{
	    if(!(readCount = read(fd,buf,SIZEMESS)))
	    {
		strcpy(sndbuf.mtext, "end");
	        for( i = 3; i <= count; i++)
		{
    		    sndbuf.mtype = i ;
		    msgsnd ( msgid, &sndbuf, 4, 0);
	        }
		break;	    
	    } 
	    else
	    {
		strcpy(sndbuf.mtext, buf);
	        countMesg++;
		for( i = 3; i <= count; i++)
		{
		    sndbuf.mtype = i ;	
		    msgsnd ( msgid, &sndbuf, readCount + 1, 0);
	        }
	    }
	}
    }
    msgctl(msgid, IPC_RMID, 0);
    close(fd);
    return 0;
}
