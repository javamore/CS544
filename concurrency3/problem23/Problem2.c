/*Problem 2

Three kinds of threads share access to a singly-linked list: searchers, inserters and deleters. Searchers merely 
examine the list; hence they can execute concurrently with each other. Inserters add new items to the end of the list; 
insertions must be mutually exclusive to preclude two inserters from inserting new items at about the same time. 
However, one insert can proceed in parallel with any number of searches. Finally, deleters remove items from anywhere 
in the list. At most one deleter process can access the list at a time, and deletion must also be mutually exclusive with 
searches and insertions. Write concurrent code in any language you like for searchers, inserters and deleters that enforces 
this kind of three-way categorical mutual exclusion. Remember, some languages have the constructs for concurrency but do NOT 
actually execute in parallel. Be aware of this and choose wisely.

Author: Chittibabu Tirupathi
Assignment: Concurrency 3
Class: CS544

*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>


/****** For Random number generation twister **********/

#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */

static unsigned long mt[N]; /* the array for the state vector  */
static int mti=N+1; /* mti==N+1 means mt[N] is not initialized */

void insert(int *);
void search(int *);
void delete(int *);

struct node 
{
	unsigned long int data;
	struct node *next;
};

struct node *head=NULL,*tail=NULL,*ptr=NULL,*temp;

pthread_t inserter[3];										/*Defing thread arrays for inserter,searcher and deleters*/
pthread_t searcher[3];
pthread_t deleter[3];

int in[3]={1,2,3};											/*Thread identifiesrs for insert,search and delete threads*/
int de[3]={1,2,3};
int se[3]={1,2,3};

int searchcount=0;//variable to store active search threads
int insertcount=0;//variable to store active insert threads
sem_t mutex1;
sem_t mutex2;
sem_t noSearcher;//mutex for delete function to make sure that no searcher thread is active
sem_t noInserter;//mutex for delete function to make sure that no insert thread is active
sem_t insertmutex;//mutex to achive mutual exclusion among to insert threads
sem_t searchm;
sem_t deletem;

int searchfound=0;
int deletecomplete=0;

/****************** Random number geration twister functions starts here  ************/

void init_genrand(unsigned long s)
{
    mt[0]= s & 0xffffffffUL;
    for (mti=1; mti<N; mti++) {
        mt[mti] = 
	    (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti); 
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array mt[].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */
        mt[mti] &= 0xffffffffUL;
        /* for >32 bit machines */
    }
}

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
/* slight change for C++, 2004/2/26 */
void init_by_array(unsigned long init_key[], int key_length)
{
    int i, j, k;
    init_genrand(19650218UL);
    i=1; j=0;
    k = (N>key_length ? N : key_length);
    for (; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1664525UL))
          + init_key[j] + j; /* non linear */
        mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++; j++;
        if (i>=N) { mt[0] = mt[N-1]; i=1; }
        if (j>=key_length) j=0;
    }
    for (k=N-1; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1566083941UL))
          - i; /* non linear */
        mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++;
        if (i>=N) { mt[0] = mt[N-1]; i=1; }
    }

    mt[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */ 
}

/* generates a random number on [0,0xffffffff]-interval */
unsigned long genrand_int32(void)
{
    unsigned long y;
    static unsigned long mag01[2]={0x0UL, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti >= N) { /* generate N words at one time */
        int kk;

        if (mti == N+1)   /* if init_genrand() has not been called, */
            init_genrand(5489UL); /* a default initial seed is used */

        for (kk=0;kk<N-M;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (;kk<N-1;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
        mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mti = 0;
    }
  
    y = mt[mti++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}

/* generates a random number on [0,0x7fffffff]-interval */
long genrand_int31(void)
{
    return (long)(genrand_int32()>>1);
}

/* generates a random number on [0,1]-real-interval */
double genrand_real1(void)
{
    return genrand_int32()*(1.0/4294967295.0); 
    /* divided by 2^32-1 */ 
}

/* generates a random number on [0,1)-real-interval */
double genrand_real2(void)
{
    return genrand_int32()*(1.0/4294967296.0); 
    /* divided by 2^32 */
}

/* generates a random number on (0,1)-real-interval */
double genrand_real3(void)
{
    return (((double)genrand_int32()) + 0.5)*(1.0/4294967296.0); 
    /* divided by 2^32 */
}

/* generates a random number on [0,1) with 53-bit resolution*/
double genrand_res53(void) 
{ 
    unsigned long a=genrand_int32()>>5, b=genrand_int32()>>6; 
    return(a*67108864.0+b)*(1.0/9007199254740992.0); 
} 

/**************** Random number generation twister function ends here and linked list creation function starts *******/

int main()
{
	int i;
	sem_init(&mutex1,0,1);										/*The following statements inializes mutexes*/
	sem_init(&mutex2,0,1);
	sem_init(&noSearcher,0,1);
	sem_init(&noInserter,0,1);
	sem_init(&insertmutex,0,1);
	sem_init(&searchm,0,1);
	sem_init(&deletem,0,1);
	for(i=0;i<3;i++)											/*This loop created insert,search and delete threads*/
	{
		pthread_create(&inserter[i],NULL,insert,&in[i]);
		pthread_create(&searcher[i],NULL,search,&se[i]);
		pthread_create(&deleter[i],NULL,delete,&de[i]);
	}
	for(i=0;i<3;i++)											/*This loop created insert,search and delete threads*/
	{
		pthread_join(inserter[i],NULL);
		pthread_join(searcher[i],NULL);
		pthread_join(deleter[i],NULL);
	}
	return 0;
}

void insert(int *i)
{
	while(1)
	{
		sem_wait(&mutex1);//This wait mutex is to increase insert count 
		insertcount++;
		if(insertcount==1)
			sem_wait(&noInserter);//if this insert thread is starting it will make delete theread to wait  
		sem_post(&mutex1);//This signal mutex is to tell increase of count is done
		sem_wait(&insertmutex);//This wait mutex is to achive mutual exclusion among two insert threads
		
		/***************  Insert operation in linked list happends in the following lines of code   ******************/
		ptr=(struct node*)malloc(sizeof(struct node));
		if(ptr==NULL)
		{
			printf("Heep memory not available now");
			continue;
		}
		if(head==NULL && tail==NULL)
		{
			ptr->data=genrand_int32()%20;
			ptr->next=NULL;
			head=ptr;
			tail=ptr;
			printf("Inserter %d inserted data : %10lu\n",*i,ptr->data);
		}else
		{
			ptr->data=genrand_int32()%20;
			ptr->next=NULL;
			tail->next=ptr;
			tail=ptr;
			printf("Inserter %d inserted data : %10lu\n",*i,ptr->data);
		}
		
		/*********************************/
		sem_post(&insertmutex); //This signal mutex is to achive mutual exclusion among two insert threads 
		sem_wait(&mutex1);//This wait mutex is to decrease search count
		insertcount--;
		if(insertcount==0)
			sem_post(&noInserter);//if this insert thread is last it will make delete theread to signal to proceed  
		sem_post(&mutex1); //This signal mutex is to tell increase of count is done
		sleep(1);
	}
}

void search(int *i)
{
	while(1)
	{
		sem_wait(&mutex2);/*This mutex is increase the count of search thread count*/
		searchcount++;
		if(searchcount==1)
			sem_wait(&noSearcher);/*This wait mutex to make del function to stop until all the search operations done*/
		sem_post(&mutex2);
		/***************    Search operation in linked list happends in the following lines of code   ******************/
		struct node *t=head;
		unsigned long int s_element=genrand_int32()%20;
		//sem_wait(&searchm);
		while(t)
		{
			if(t->data==s_element)
			{
				printf("\t\t\tSearcher %d found element %10lu\n",*i,s_element);//return True;
				sem_wait(&searchm);
				searchfound=1;
				break;
			}
			t=t->next;
		}
		if(searchfound==0)
		{
			printf("\t\t\tSearcher %d couldn't find element %10lu\n",*i,s_element);
			sem_post(&searchm);
		}
		else
		{
			searchfound=1;
			sem_post(&searchm);
		}
		//sem_post(&searchm);
		
		/*********************************/
		sem_wait(&mutex2); /*This mutex is decrease the count of search thread count*/
		searchcount--;
		if(searchcount==0)
			sem_post(&noSearcher);/*This signal mutex to make del function to proceed the delete operations */
		sem_post(&mutex2);
		sleep(1);
	}
}

void delete(int *i)
{
	while(1)
	{
		sem_wait(&noSearcher);/*This wait mutex makes all the search threads to wait until delete operation is over*/
		sem_wait(&noInserter);/*This wait mutex makes all the inser threads to wait until delete operation is over*/
		/**************    Delete operation in linked list happends in the following lines of code   *******************/
		if(head==NULL)
		{
				sem_post(&noInserter);
				sem_post(&noSearcher);
				continue;
		}	
		struct node *t=head;
		unsigned long int d=genrand_int32()%20;
		if(t->data==d)
		{
			if(t->next==NULL)
			{
				free(t);
				head=NULL;
				tail=NULL;
				printf("\t\t\t\t\t\t\t\tDeleter %d deleted element %10lu from list.\n",*i,d);
				sem_wait(&deletem);
				deletecomplete=1;
				sem_post(&noInserter);
				sem_post(&noSearcher);
				continue;
			}
			else
			{
				head=t->next;
				free(t);
				printf("\t\t\t\t\t\t\t\tDeleter %d deleted element %10lu from list.\n",*i,d);
				sem_post(&noInserter);
				sem_post(&noSearcher);
				continue;
			}
		}		
		while(t)
		{
			if(t->next==NULL)
			{
				printf("\t\t\t\t\t\t\t\tDeleter %d cannot locate the element %10lu in the list.\n",*i,d);
				break;
			}
			if(t->next->data==d)
			{
				sem_wait(&deletem);
				deletecomplete=1;
				temp=t->next;
				t->next=t->next->next;
				free(temp);
				printf("\t\t\t\t\t\t\t\tDeleter %d deleted element %10lu from list.\n",*i,d);
				break;
			}
			t=t->next;
		}
		if(deletecomplete==0)
		{
			printf("\t\t\t\t\t\t\t\tDeleter %d cannot locate the element %10lu in the list.\n",*i,d);
			sem_post(&deletem);
		}
		else
		{
			deletecomplete=0;
			sem_post(&deletem);
		}
		
		/*********************************/
		sem_post(&noInserter);/*This signal mutex makes all the search threads to proceed with their operations*/
		sem_post(&noSearcher);/*This signal mutex makes all the insert threads to proceed with their operations*/
		sleep(2);
	}
}
