#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/syscall.h>

sem_t resource;
int num_procs = 0;

void* process(void *arg)
{	
	pthread_t tid = syscall(SYS_gettid);
	
	while(1)
	{
		if (num_procs < 3)
		{
			sem_wait(&resource);
			num_procs++;
			
			printf("ID %d: Using resource\n", tid);
			sleep(5);
			printf("ID %d: Leaving resource\n", tid);
		
			sem_post(&resource);
			num_procs--;
			
			sleep(5);
		}		
		else
		{
			printf("ID %d: Blocked\n", tid);
			while(num_procs != 0)
				;
			printf("ID %d: Unblocked\n", tid);
		}
	}
}

int main(void)
{
    pthread_t processes[8];

    sem_init(&resource, 0, 3);

	puts("Creating eight threads.\n");
	
	int i;
	for (i = 0; i < 8; i++)
		pthread_create(&processes[i], NULL, process, NULL);

    sleep(999);
	
	return 0;
}
