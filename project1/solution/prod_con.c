#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include "mt19937ar.h"


#define count_producer 4	// The number of producer and consumer
#define count_consumer 3
#define BufferSize 32		// The number of buffer

unsigned long createRandom(void);

struct Item
{
	unsigned long value;
    unsigned long sleepTime;
};

int location = 0; 			// The location in buffer
struct Item buffer[BufferSize];

sem_t fullSem;				// Semaphore for full
sem_t emptySem;				// Semaphore for empty
pthread_mutex_t mutex;		//Mutex

void *producer()
{
	unsigned long timeProducer;
	unsigned long value;
    while(1)
    {
		timeProducer = createRandom() % 5 + 3;
		value = createRandom() % 8 + 2;
		
        sem_wait(&fullSem);
		sleep(timeProducer);
        pthread_mutex_lock(&mutex);
		buffer[location].sleepTime = value;
		buffer[location].value = createRandom() % 1000;

		printf("producer sleep: %d\n", timeProducer);
		printf("item %d at location %d\n\n", buffer[location].value, location);
		location++;
        pthread_mutex_unlock(&mutex);
        sem_post(&emptySem);
    }
}

void *consumer()
{
	unsigned long timeConsumer;
	struct Item item;
    while(1)
    {
        sem_wait(&emptySem);
        pthread_mutex_lock(&mutex);
        location--;
		item.sleepTime = buffer[location].sleepTime;
		item.value = buffer[location].value;

		printf("consumer sleep %d\n", buffer[location].sleepTime);
		printf("item %d at location %d\n\n", buffer[location].value, location);
        pthread_mutex_unlock(&mutex);
        sem_post(&fullSem);
		sleep(item.sleepTime);
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

int main(void)
{
    pthread_t producers[count_producer];
    pthread_t consumers[count_consumer];
    int i;

	// Initialize the Semaphore
    sem_init(&emptySem, 0, 0);
    sem_init(&fullSem, 0, BufferSize);

	// Initialize the mutex
    pthread_mutex_init(&mutex, NULL);

	// Create producer thread
    for(i = 0; i < count_producer; i++)
    {
        pthread_create(&producers[i], NULL, producer, NULL);
    }

	// Create consumer thread
  	for(i = 0; i < count_consumer; i++)
    {
        pthread_create(&consumers[i], NULL, consumer, NULL);
    }

	// pthread_join
    for(i = 0; i < count_producer; i++)
    {
        pthread_join(producers[i],NULL);
    }
	
	for(i = 0; i < count_consumer; i++)
    {
        pthread_join(consumers[i],NULL);
    }
    return 0;
}
