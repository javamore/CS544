#include <stdio.h>
#include <cstdlib>
#include <pthread.h>
#include <iostream>
#include <time.h>
#include <unistd.h>
#define WAIT_TIME 10

//Mutex for accessing the shop
pthread_mutex_t shop_mutex;

//Mutex for cutting hair next
pthread_mutex_t next_customer_mutex;

//Mutex for when the barber needs a nap
pthread_mutex_t barber_sleep_mutex;

//Whether the barber is sleeping
pthread_cond_t barber_sleepStatus;

//Whether the barber is working
pthread_cond_t barber_workStatus;

int numCustomers = 0;
int barberSleeping = 0;
int haircutTime = 0;
int MAX_SEATS = 10;

void get_hair_cut(int);
void cut_hair();
void *customer(void* );
void *barber_room(void* );


void get_hair_cut(int id){
    printf("Customer [%d] is getting a haircut... \n",id);
    sleep(haircutTime);
 }

void cut_hair(){
    printf("Barber is cutting hair... \n");
    sleep(haircutTime);
}

void *customer(void *t){ 
    int id = (intptr_t) t;
    pthread_mutex_lock(&shop_mutex); 
    numCustomers++; 
    printf("A customer [%d] has entered the waiting room. Checking for available seats...\n",id);

    if(numCustomers < MAX_SEATS){
        //Then checks to see if the barber is asleep. If so, wake him up.
        if(barberSleeping == 1){
            printf("The barber is sleeping! The customer [%d] wake him up. \n",id);
            pthread_cond_signal(&barber_sleepStatus);
        }
        pthread_mutex_unlock(&shop_mutex);

        printf("There is a seat! The customer [%d] sits down.\n",id);
        pthread_mutex_lock(&next_customer_mutex);
        pthread_cond_wait(&barber_workStatus, &next_customer_mutex);
        get_hair_cut(id);
        pthread_mutex_unlock(&next_customer_mutex);
        numCustomers--;
        pthread_exit(0);
    }
    
    //If there are no available seats, the customer leaves.
    if(numCustomers >= MAX_SEATS){
        numCustomers--;
        pthread_mutex_unlock(&shop_mutex);
        printf("All the seats are filled,  The customer [%d] leaves the shop. \n", id);
        pthread_exit(0);
    }
}

void *barber_room(void *){

    while(1){    
        //If there are no customers, the barber will fall asleep until a customer wakes him up.
        if(numCustomers == 0){
            pthread_mutex_lock(&barber_sleep_mutex);
            barberSleeping = 1;
            printf("No one is in the barbershop, barber go to sleep.\n");
            pthread_cond_wait(&barber_sleepStatus, &barber_sleep_mutex);
            barberSleeping = 0;
            printf("The barber woke up! Time to get back to work.\n");
        }
        //The barber will signal for the next customer to begin the haircut.
        else{
            printf("The barber calls for the next customer.\n");
            srand(time(NULL));
            haircutTime = rand()%5 + 1;
            pthread_cond_signal(&barber_workStatus);
            cut_hair();
            printf("The barber finished cutting the hair!\n");
			printf("===========New Seat free=============\n");
        }      
    }   
}


int main(int argc, char *argv[]){

    int i = 0;
    if(argc == 2){
        MAX_SEATS = atoi(argv[1]);
    }

    pthread_t barber;
    pthread_t customers[MAX_SEATS];
    
    pthread_attr_t barber_attr;
    pthread_attr_t customers_attr[MAX_SEATS];
    
    pthread_attr_init(&barber_attr);

    // only one barber
    pthread_create(&barber, &barber_attr, barber_room, NULL);
   
    // many customer
    while(1){
        sleep(rand()%5 + 1);
        for(i=0; i < MAX_SEATS ; i++) {
                pthread_attr_init(&customers_attr[i]);
                pthread_create(&customers[i], &customers_attr[i], customer, (void*)(intptr_t) i );
        }
    }

    pthread_join(barber, NULL);
    for (i=0 ; i < MAX_SEATS; i++){
        pthread_join(customers[i],NULL);
    }

    return 0;
}
