#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

void listfile(char *name)
{
	struct stat sb;
	if (stat(name, &sb) < 0)
	{
		perror(name);
		exit(2);
	}

	// Array of file types, indexed by the top four bits of st_mode
	char *filetype[] = { "?", "p", "c", "?", "d", "?", "b", "?", "-", "?", "l", "?", "s" };
	
	// File type (using same indicator characters as ls)
	printf("%s", filetype[(sb.st_mode >> 12) & 017]);

	// Permissions (same as ls)
	printf("%c%c%c%c%c%c%c%c%c",
				 (sb.st_mode & S_IRUSR) ? 'r' : '-',
				 (sb.st_mode & S_IWUSR) ? 'w' : '-',
				 (sb.st_mode & S_IXUSR) ? 'x' : '-',
				 (sb.st_mode & S_IRGRP) ? 'r' : '-',
				 (sb.st_mode & S_IWGRP) ? 'w' : '-',
				 (sb.st_mode & S_IXGRP) ? 'x' : '-',
				 (sb.st_mode & S_IROTH) ? 'r' : '-',
				 (sb.st_mode & S_IWOTH) ? 'w' : '-',
				 (sb.st_mode & S_IXOTH) ? 'x' : '-' );

	printf("%8ld", sb.st_size);

	char *modtime = ctime(&sb.st_mtime);

	// ctime() string has \n at end, we need to remove this
	modtime[strlen(modtime) - 1] =  '\0';
	printf("  %s  ", modtime);
	printf("%s\n", name);
}

int main(int argc, char * argv[])
{
	int allflag = 0;

	// Delete this to see getopt's own error messages
	opterr = 0;

	int c;
	while ( (c = getopt(argc, argv, "a")) != EOF)
	{
		switch (c)
		{
			case 'a':
				allflag = 1;
				break;
			case '?':
				fprintf(stderr, "invalid option: -%c\n", optopt);
		}
	}

	// Adjust to move past the options
	argv += optind;  
	argc -= optind;

	// Now argc is the number of non-option args and argv[0] is the first one
	if (argc != 1)
	{
		fprintf(stderr, "usage: listdir2 [-a] dirname\n");
		exit(1);
	}

	chdir(argv[0]);
	DIR *d = opendir(".");

	struct dirent *info;
	while ((info = readdir(d)) != NULL)
	{
		// only list hidden files if allflag is setn
		if (info->d_name[0] != '.' || allflag)
			listfile(info->d_name);
	}
}
