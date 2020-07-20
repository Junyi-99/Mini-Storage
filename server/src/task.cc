#include "task.h"
#include <stdio.h>


void* do_task(void *pBQ)
{
	BlockingQueue<int>* p_que = (BlockingQueue<int>*) pBQ;
	while(1)
	{
		int new_fd = p_que->take();
		//ganhuo
		//close(new_fd);
		printf("%d\n",new_fd);
	}
}

