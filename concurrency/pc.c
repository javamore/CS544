#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include "mt19937ar.h"

// The number of producer and consumer
#define Number 2
// The number of buffer
#define BufferSize 32

unsigned long createRandom(void);

struct items
{
    unsigned long sleepTime;
	unsigned long itemNumber;
};

int location = 0;
// The location in buffer
struct items buffer[BufferSize];
// Semaphore synchronization
sem_t fullSem;
sem_t emptySem;
// Mutex
pthread_mutex_t mutex;

// Producer
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
		buffer[location].itemNumber = createRandom() % 1000;
		printf("producer\n");
		printf("sleep time: %d\n", timeProducer);
		printf("item number: %d\n", buffer[location].itemNumber);
		printf("location: %d\n\n", location);
		location++;
        pthread_mutex_unlock(&mutex);
        sem_post(&emptySem);
    }
}

// Comsumer
void *consumer()
{
	unsigned long timeConsumer;
	struct items item;
    while(1)
    {
        sem_wait(&emptySem);
        pthread_mutex_lock(&mutex);
        location--;
		item.sleepTime = buffer[location].sleepTime;
		item.itemNumber = buffer[location].itemNumber;
		printf("consumer\n");
		printf("sleep time: %d\n", buffer[location].sleepTime);
		printf("item number: %d\n", buffer[location].itemNumber);
		printf("location: %d\n\n", location);
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
    pthread_t id1[Number];
    pthread_t id2[Number];
    int i;

	// Initialize the Semaphore
    sem_init(&emptySem, 0, 0);
    sem_init(&fullSem, 0, BufferSize);

	// Initialize the mutex
    pthread_mutex_init(&mutex, NULL);

	// Create producer thread
    for(i = 0; i < Number; i++)
    {
        pthread_create(&id1[i], NULL, producer, NULL);
    }

	// Create consumer thread
  	for(i = 0; i < Number; i++)
    {
        pthread_create(&id2[i], NULL, consumer, NULL);
    }

	// pthread_join
    for(i = 0; i < Number; i++)
    {
        pthread_join(id1[i],NULL);
        pthread_join(id2[i],NULL);
    }
    return 0;
}
