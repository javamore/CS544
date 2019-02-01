#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include "mt19937ar.h"

int  searcher_count=0, insert_count  =0;  
pthread_mutex_t search_switch;
pthread_mutex_t insert_switch;
pthread_mutex_t no_searcher;
pthread_mutex_t no_insert;
pthread_mutex_t insert_mutex;     


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


void *searchers()
{    
	while(1){
		//search_switch.wait();
		pthread_mutex_lock(&search_switch);
			searcher_count +=1;
			if (searcher_count==1)
			   //no_searcher.wait();
				pthread_mutex_lock(&no_searcher);
				
		//search_switch.signal();
		pthread_mutex_unlock(&search_switch);
		
		unsigned long randomtime;
		randomtime = createRandom() % 5 + 1;
		sleep(randomtime);
		printf("searcher is searching: %d\n", randomtime);
		
		//search_switch.wait();
		pthread_mutex_lock(&search_switch);
			searcher_count --;
			if(searcher_count==0)
				//no_searcher.signal();
				pthread_mutex_unlock(&search_switch);
		//search_switch.signal();
		pthread_mutex_unlock(&search_switch);
	}
}

void *inserters()
{   
	while(1){
		//insert_switch.wait();
		pthread_mutex_lock(&insert_switch);
		insert_count +=1;
        if(insert_count==1)
			pthread_mutex_lock(&no_insert);
	       //no_insert.wait();	
        //insert_switch.signal(&insert_switch);
		pthread_mutex_unlock(&insert_switch);
		
		
		//insert_mutex.wait();
		pthread_mutex_lock(&insert_mutex);
		unsigned long randomtime;
		randomtime = createRandom() % 5 + 2;
		sleep(randomtime);
		printf("inserter is inserting: %d\n", randomtime);
		//insert_mutex.signal();
		pthread_mutex_unlock(&insert_mutex); 
		
		//insert_switch.wait();
		pthread_mutex_lock(&insert_switch);
	    insert_count -=1;
		if (insert_count==0)
			 pthread_mutex_unlock(&no_insert); 
		    //no_insert.signal();
	
        //insert_switch.signal();	  
	    pthread_mutex_unlock(&insert_switch); 
	}
}



void *deleters()
{
	while(1){
		//no_searcher.wait();
		//no_insert.wait();
		pthread_mutex_lock(&no_searcher);
		pthread_mutex_lock(&no_insert);

		unsigned long randomtime;
		randomtime = createRandom() % 5 + 1;
		sleep(randomtime);
		printf("deleter is deleting: %d\n", randomtime);
		
		pthread_mutex_unlock(&no_searcher); 
		pthread_mutex_unlock(&no_insert); 
		// no_searcher.signal();
		// no_insert.signal(); 
	}
}






int main(void)
{
    pthread_t searcher[5];
    pthread_t inserter[5];
    pthread_t deleter[5];
    int i;

	// Initialize the Semaphore
    // sem_init(&emptySem, 0, 0);
    // sem_init(&fullSem, 0, BufferSize);
	
	// Initialize the mutexes
    pthread_mutex_init(&search_switch, NULL);
	pthread_mutex_init(&insert_switch, NULL);
	pthread_mutex_init(&no_searcher, NULL);
	pthread_mutex_init(&no_insert, NULL);
	pthread_mutex_init(&insert_mutex, NULL);

	// Create searcher thread
    for(i = 0; i < 5; i++)
    {
        pthread_create(&searcher[i], NULL, searchers, NULL);
    }

	// Create insert thread
  	for(i = 0; i < 5; i++)
    {
        pthread_create(&inserter[i], NULL, inserters, NULL);
    }
	
	for(i = 0; i < 5; i++)
    {
        pthread_create(&deleter[i], NULL, deleters, NULL);
    }
	
	
	// pthread_join
    for(i = 0; i < 5; i++)
    {
        pthread_join(searcher[i],NULL);
    }
	
	for(i = 0; i < 5; i++)
    {
        pthread_join(inserter[i],NULL);
    }
	
	for(i = 0; i < 5; i++)
    {
        pthread_join(deleter[i],NULL);
    }
    return 0;
}



