#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdio.h>
#define MAXFDC 256
#define BUFSZ 256
#define TIME_OUT 1

int main(int argc, char**argv) 
{
    int i, bytesRead;
    int fdc = 0;
    FILE* fd[MAXFDC];
    char buf[BUFSZ];
    fd_set fds;
    struct timeval timeout;
    for (i=1; i<argc; i++) 
    {
	fd[fdc] = fopen(argv[i], "rb");
	if (fd[fdc]==NULL) perror(argv[i]);
	else if (++fdc==MAXFDC) break;
    }
    while (fdc) for (i=0; i<fdc; i++) 
    {
	timeout.tv_sec = TIME_OUT;
	timeout.tv_usec = 0;
	FD_ZERO(&fds);
	FD_SET(fileno(fd[i]), &fds);
	if (select(fileno(fd[i])+1, &fds, 0, 0, &timeout)>0) 
	{
	    if (fgets(buf,BUFSZ,fd[i])!=0) 
	    {
		bytesRead=strlen(buf);
	        write(1, buf, bytesRead);
	    }
    	    else 
	    {
		close(fileno(fd[i]));
		fd[i--] = fd[--fdc];
	    }
    	}
    }
    return 0;
}