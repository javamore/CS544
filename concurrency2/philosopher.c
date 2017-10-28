#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include "mt19937ar.h"

unsigned long createRandom(void);

sem_t fullSem;				// Semaphore for full
pthread_mutex_t m_fork[5];		//Mutex

int get_fork(int i)
{
	sem_wait(&fullSem);
	pthread_mutex_lock(&m_fork[i]);
	pthread_mutex_lock(&m_fork[(i+1)%5]);
	printf("phillosopher %d get forks\n", i);
}

int put_fork(int i)
   {
	pthread_mutex_unlock(&m_fork[i]);
	pthread_mutex_unlock(&m_fork[(i+1)%5]);
	printf("phillosopher %d put forks\n", i);
	sem_post(&fullSem);
}

void *loop( int i )
{
    while(1)
    { 
		unsigned long think;
		think = createRandom() % 8 + 2;
		sleep(think);
		printf("phillosopher %d thinks: %d\n", i, think);
		
		get_fork(i);
		
		unsigned long eat;
		eat = createRandom() % 20 + 1;
		sleep(eat);
		printf("phillosopher %d eats: %d\n", i, eat);
		
		put_fork(i);
    }
}


unsigned long createRandom(void)
{
	unsigned long result;
	unsigned int eax;
	unsigned int ebx;
	unsigned int ecx;
	unsigned int edx;
	char vendor[13];
	eax = 0x01;
	__asm__ __volatile__(
	                     "cpuid;"
	                     : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
	                     : "a"(eax)
	                     );
	if(ecx & 0x40000000)
	{
		// use rdrand
		asm ("rdrand %0" : "=r"(result));
	}
	else
	{
		// use mt19937
		result = genrand_int32();
	}
	return result;
}


int main()
{
    pthread_t thread[5];
    
    int i;
	// Initialize the Semaphore
    sem_init(&fullSem, 0, 5);

	// Initialize the mutex
	for(i = 0; i < 5; i++)
    {
		pthread_mutex_init(&m_fork[i], NULL);
	}
    
	for(i = 0; i < 5; i++)
    {
        pthread_create(&thread[i], NULL, loop, i);
    }
	
	for(i = 0; i < 5; i++)
    {
        pthread_join(thread[i],NULL);
    }
	
    return 0;
}





