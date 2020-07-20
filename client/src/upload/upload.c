#include <stdio.h>
#include "../../etc/config.h"
#include "../../include/small_file.h"
#include "../../include/big_file.h"


int main(int argc, char *argv[])
{
	int fd = open(argv[1], O_RDONLY);
	struct stat file_state;
	fstat(fd, &file_state);
	off_t file_size = file_state.st_size;
	if(file_size > SMALL_FILE_SIZE_MAX)
	{
		do_small_file_upload(fd, argv[1], file_size);
	}
	else
	{
		do_big_file_upload(fd, argv[1], file_size);
	}
	//printf("%s   %d \n",argv[1], file_size);
	close(fd);
	exit(0);
}









