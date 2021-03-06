#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

void timeout_handler(int unused)
{
	// Nothing to do
}

int t_getnum(int timeout)
{
	struct sigaction action;
	action.sa_handler = timeout_handler;
	sigemptyset(&action.sa_mask);
	
	// don't restart system calls 
	action.sa_flags = 0;
	sigaction(SIGALRM, &action, NULL);

	alarm(timeout);
	char line[100];
	int n = read(0, line, 100);
	
	// Cancel alarm
	alarm(0);

	// if we returned from the read because of a signal
	if (n == -1 && errno == EINTR)
		return -1;
	
	n = atoi(line);
	
	return n;
}

int main(void)
{
	while(1)
	{
		printf("enter a number: "); fflush(stdout);

		int num;
		if ((num = t_getnum(5)) == -1)
			printf("timed out!\n");
		else
			printf("You entered %d\n", num);
	}
}
