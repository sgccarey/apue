#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

char c;
int count;

void read_config_file(int signum)
{
	size_t len = 0;
	char *line;
	FILE *fd = fopen("/tmp/reconfigure.conf", "r");

	getline(&line, &len, fd);
	c = line[0];

	getline(&line, &len, fd);
	
	count = atoi(line);
	
	printf("read char = %c and count = %d\n", c, count);
	fclose(fd);
}

int main(void)
{
	signal(SIGHUP, read_config_file);
	raise(SIGHUP);

	while (1)
	{
		for (int i = 0; i < count; i++)
			putchar(c);

		putchar('\n');
		sleep(3);
	}
}
