#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <malloc.h>
#include <errno.h>

int main( int ac, char*av[])
{
    int	 fd;
    struct flock fstr;
     
    fstr.l_type   = F_WRLCK;
    fstr.l_whence = SEEK_SET;
    fstr.l_start  = 0;
    fstr.l_len    = 0;

//    system("chmod +l txt");
   
    if(( fd = open( "txt", O_RDWR)) == -1)
	return fprintf(stderr,"Error in opening file \n");
    
    if(fcntl( fd, F_SETLK, &fstr )==-1)
    {
       if ((errno == EACCES) || (errno == EAGAIN)) {
              printf("%s try later\n", av[1]);
              exit(2);
          }
          perror(av[1]);
          exit(3);    
    }

    system("vi txt");
    
    fstr.l_type = F_UNLCK; 
    fcntl( fd, F_SETLK, &fstr);
    close(fd);
    return 0;    
}
