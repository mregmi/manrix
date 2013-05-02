#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <fcntl.h>

#include <sys/stat.h>

int main(int argc, char *const *argv)
{
	int fd, ret;
	struct stat st;
	char buf[40];

	printf("Testing process %d \n", getpid());
	
/*	ret = vfork();
	if(ret == 0)
	{
		printf(" I am child %d", ret);
		exit(1);
	}
/*	ret = wait(&fd);
	if(ret < 0)
		printf("ret failed with %d \n", errno);
*/
	return 0;
}

