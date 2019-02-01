#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "mt19937ar.h"
#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdbool.h>

#define processCount 10

pthread_mutex_t nubmerEnter = PTHREAD_MUTEX_INITIALIZER;
sem_t block;
int count = 0;
int waiting = 0;
bool wait = false;

unsigned long randnumber()
{
    unsigned long number;
    number = genrand_int32();
    return number;
}



void *process(int pid)
{
    while(1) {
	
		pthread_mutex_lock(&nubmerEnter);
		if(wait){
			waiting = waiting + 1;
			pthread_mutex_unlock(&nubmerEnter);
			sem_wait(&block);
		}else{
			count = count + 1;
			printf("process %2d access data (%2d)\n", pid, count);
			if(count == 3){
				printf("wait for clear\n");
				wait = true;
			}else{
				wait = false;
			}
			pthread_mutex_unlock(&nubmerEnter);
		}
		
		
		
		
        unsigned long time;
        time = randnumber()%5+1;
        printf("process %2d runs %lu seconds\n", pid, time);
        sleep(time);
		
		pthread_mutex_lock(&nubmerEnter);
		count = count-1;
		printf("process %2d away from data (%2d)\n", pid, count);
		if(count == 0){
			printf("clear is finished \n");
			int n = waiting < 3? waiting:3;
			waiting -= n;
			count = n;
			
			
			while(n>0){
				
				sem_post(&block);
				n = n-1;
			}
			if(count == 3){
				
				wait = true;
			}else{
				wait = false;
			}
		}
		pthread_mutex_unlock(&nubmerEnter);
    }
}




int main(void)
{
    int i = 0;
    pthread_t processes[processCount];
	pthread_mutex_init(&nubmerEnter, NULL);
	sem_init(&block, 0, 0);

    for(i = 0; i < processCount; i++) {
        if(pthread_create(&processes[i], NULL, process, i) !=0)
        {
          printf("error\n");
        }
    }
	
    for(i = 0; i < processCount; i++)
	  pthread_join(processes[i],NULL); 
   
    return 0;
}
