#ifndef concurrency
#define concurrency

#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <immintrin.h>
#include <stdio.h>
#include <signal.h>
#include <iostream>
#include <semaphore.h>

using namespace std;

struct worker{
	int in_use;
	int need_sleep;
};

struct resource {
	string data = "the data";
	int num_using;
};

void* watcher(void*);
void end_workers();
void *use_resource(void*);

#endif
