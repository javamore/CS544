 /* 
 * Programmers: Andrew Bowers, Shuai Peng, Anya Lehman 
 * Class: CS 444 - Operating Systems II
 * Date: 5/29/17
 * Description: Barbershop Problem - Problem 1
 */

#include "concurrency.hpp"

#define MaxThreadNum 5
static volatile int run = 1;

worker workers[] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}}; 
pthread_mutex_t mutexnum;
pthread_cond_t w_empty;

// Global struct
resource values;

void* watcher(void*) 
{
	// Check if we have 3 threads
	while(1) {
		/* if there are more than 2 workers/threads using the resource
 		* tell other threads to wait and stop the other workers   
		*/
		if(values.num_using == 3) {
			cout << "BOSS said you should wait." << endl;
			end_workers();	
		}
		else {
			pthread_cond_broadcast(&w_empty);
		}
	}
}

void end_workers() 
{	
	// Lock resource, find all threads that are in use, mark they have worked so they 
	// can be put to sleep, set number of workers back to 0
	//pthread_mutex_lock(&mutexnum);
	cout << "============start ending other threads=================" << endl;
	for(int i = 0; i < 4; i++) {
		if(workers[i].in_use == 1) {
			workers[i].in_use = 0;
			workers[i].need_sleep = 1;
		}
	}
	values.num_using = 0;
	cout << "=========done end other threads=========================\n" << endl;
	//pthread_cond_broadcast(&w_empty);
}

void *use_resource(void* t)
{
	// Thread ID
	int id = (intptr_t) t;
        
	// Sleep for random time
	int x = rand() % 3 + 1;
	cout << "Worker [" << id<< "] is getting ready...\n" << endl;
	sleep(x);
	while(1) {
		pthread_mutex_lock(&mutexnum);
		pthread_cond_wait(&w_empty, &mutexnum);	
		/* Check to see if a given thread has worked (has been told to stop working)
 		* put it to sleep for 1-10 seoncds.
 		*/
		if (workers[id-1].need_sleep == 1) {	
			int x = rand() % 3 + 1;
			//cout << "Worker id[" << id << "] is taking a " << x << " second break."  << endl;
			sleep(x);
			workers[id-1].need_sleep = 0;
			//cout << "Worker id[" << id << "] has finished sleeping." << endl;
		}
	
		/* if a thread hasn't worked, increase number of threads usng resource
 		* and mark that the thread is working
		*/
		if(workers[id-1].in_use == 0) {
			values.num_using += 1;
			workers[id-1].in_use = 1;
			cout << "Worker [" << id << "] is using '" << values.data  << endl;
		}
		pthread_mutex_unlock(&mutexnum);
	}
        
}


//=============================
//       main function 
//=============================
int main (int argc, char **argv)
{
    	int i;
	srand (time(NULL));
	values.num_using = 0;
	//create thread
        pthread_t threads[MaxThreadNum];

	pthread_mutex_init(&mutexnum, NULL);
	pthread_cond_init(&w_empty, NULL);
        
        // Generate five threads for each of them 
        // Searcher
        pthread_create(&threads[0], NULL, use_resource, (void*) (intptr_t)1);
        pthread_create(&threads[1], NULL, use_resource, (void*) (intptr_t)2);
        pthread_create(&threads[2], NULL, use_resource, (void*) (intptr_t)3);
        pthread_create(&threads[3], NULL, use_resource, (void*) (intptr_t)4);       
        pthread_create(&threads[4], NULL, watcher, (void*) (intptr_t)5);       
        
	for (i = 0; i < MaxThreadNum; i++) {
                pthread_join(threads[i], NULL);
        }

        return 0;
}
        
