#include <stdio.h>
#include <signal.h>
#include <termios.h>

int count;

struct termios oldsettings;
struct termios newsettings;

void sigcatch(int sig)
{
	if(sig==SIGQUIT)
	{
		printf("Count = %d\n",count);
		tcsetattr(fileno(stdin),TCSANOW,&oldsettings);
		exit(0);
	}
	printf("\a");
	fflush(stdout);
	++count;
}

int main()
{
	tcgetattr(fileno(stdin),&oldsettings);
	newsettings = oldsettings;
	newsettings.c_lflag &= ~ECHO;
	newsettings.c_cc[VINTR] = 27;
	tcsetattr(fileno(stdin),TCSANOW,&newsettings);
	for(;;)
	{
		signal(SIGINT,sigcatch);
		signal(SIGQUIT,sigcatch);
	}
	return 0;
}
