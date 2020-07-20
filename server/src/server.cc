#include "base.h"
#include "BlockQueue.h"
#include "task.h"





int main()
{
	int socket_fd = tcp_init(6666);
	int new_fd;
	BlockingQueue<int> task_que;
	
	int i = 4;
	pthread_t pid;
	while(i--)
	{
		pthread_create(&pid,NULL,do_task,(void*)& task_que);
		printf("%ld\n",pid);
	}
	

	while(1)
	{
		new_fd = tcp_accept(socket_fd);
		task_que.put(new_fd);
	}
}





















