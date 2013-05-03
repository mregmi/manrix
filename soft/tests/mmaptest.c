#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <sys/stat.h>

int main(int argc, char *const *argv)
{
	int fd, ret;
	char *myc;
	
	fd = open("/dev/console", O_RDONLY);
	if(fd < 0)
		exit(0);
	
	dup(fd);
	dup(fd);
	fd = open("/etc/inittab", O_RDONLY);
	myc = mmap(NULL, 1024, PROT_READ, MAP_PRIVATE, fd, 0);
	printf("SS : %s", myc);
	return 0;
}

