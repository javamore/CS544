#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
//wasn't sure if we needed max seats or wait time 
//change if we need to this gives me seg faults for the max seats later down the line.
#define MAX_SEATS 5
#define WAIT_TIME 5

//Mutex for accessing the shop
pthread_mutex_t shop_mutex;
//pthread_mutex_init(&shop_mutex, NULL);

//Mutex for cutting hair next
pthread_mutex_t nextCustomer_mutex;
//pthread_mutex_init(&nextCustomer_mutex, NULL);

//Mutex for when the barber needs a nap
pthread_mutex_t barberSleep_mutex;
//pthread_mutex_init(&barberSleep_mutex, NULL);

//Whether the barber is sleeping
pthread_cond_t barberSleepStatus_cond;
//pthread_cond_init(&barberSleepStatus_cond, NULL);

//Whether the barber is working
pthread_cond_t barberWorkStatus_cond;
//pthread_cond_init(&barberWorkStatus_cond, NULL);

int numCustomers = 0;
int barberSleeping = 0;
int haircutTime = 0;

void get_haircut();
void cut_hair();
void *generate_customer();
void customer();
void *waiting_room();
void *barber_room();


void get_haircut(){
    printf("Customer is getting a haircut... \n");
    sleep(haircutTime);
 }

void cut_hair(){
    printf("Barber is cutting hair... \n");
    sleep(haircutTime);
}
//Thread creates threads for each customer, up to the max number of seats defined plus one.  Whether or not a customer is generated is based on a random number. 
//If the the number is even, a customer is generated. 
void *generate_customer(){
    
    int i = 0;
    pthread_t customers[MAX_SEATS + 1];
    pthread_attr_t customers_attr[MAX_SEATS + 1];
   

    //a seg fault occurs once the maximum number of threads have been created and the function tries to create more. 
	//again this maybe my fault for trying to give us max seat defined.  I am no sure if we need them or not.
    while(1){
        
        sleep(rand()%6+1);

        for(i=0; i < (MAX_SEATS + 1); ++i) {
                pthread_attr_init(&customers_attr[i]);
                pthread_create(&customers[i], &customers_attr[i], waiting_room, NULL);
        }
    }
}

void customer(){
    
    //First checks to see if the there is an available seat.
    if(numCustomers < MAX_SEATS){

        //Then checks to see if the barber is asleep. If so, wake him up.
        if(barberSleeping == 1){
            printf("The barber is sleeping! The customer pokes the barber to wake him up. \n");
            pthread_cond_signal(&barberSleepStatus_cond);
        }

        //The customer takes the mutex and awaits for the barber's signal to sit in barber seat.
        pthread_mutex_unlock(&shop_mutex);
        printf("There is a seat! The customer rushes over to sit down before the other customer so he is first.  Our society now ugh.\n");
        pthread_mutex_lock(&nextCustomer_mutex);
        pthread_cond_wait(&barberWorkStatus_cond, &nextCustomer_mutex);
        get_haircut();
        pthread_mutex_unlock(&nextCustomer_mutex);
        pthread_exit(0);
        numCustomers--;
    }
    
    //If there are no available seats, the customer leaves.
    if(numCustomers >= MAX_SEATS){
        numCustomers--;
        pthread_mutex_unlock(&shop_mutex);
        printf("Customer: \"What!? All the seats are full!!  This is &*^$&!!!!! Customer leaves angrly and tells social media how slow the barber shop is.\" \n");
        //return;
        pthread_exit(0);
    }
}

//The customer enters the shop and attempts to take a seat
void *waiting_room(){ 
    
   //while(1){ 
        pthread_mutex_lock(&shop_mutex); 
        numCustomers++; 
        printf("A customer has entered the waiting room. Looking for available seats...\n");
        customer();
   //}

}

void *barber_room(){

    while(1){    
        //If there are no customers, the barber will fall asleep until next customer
        if(numCustomers == 0){
            pthread_mutex_lock(&barberSleep_mutex);
            barberSleeping = 1;
            printf("The barber goes to sleep because of no customers\n");
            pthread_cond_wait(&barberSleepStatus_cond, &barberSleep_mutex);
            barberSleeping = 0;
            printf("The in a rush customer wakes up barber.\n");
        }
        //The barber calls for customer
        else{
            printf("The barber calls for the next customer.\n");
            srand(time(NULL));
            haircutTime = rand()%10;
            pthread_cond_signal(&barberWorkStatus_cond);
            cut_hair();
            printf("The barber done cutting hair!\n");
        }      
    }   
}


int main(int argc, char *argv[]){

    pthread_t barber;
    pthread_t customersGenerator;
    pthread_t timer;
    
    pthread_attr_t barber_attr;
    pthread_attr_t customersGenerator_attr;
    pthread_attr_t timer_attr;
    
    pthread_attr_init(&barber_attr);
    pthread_attr_init(&customersGenerator_attr);
    pthread_attr_init(&timer_attr);

    pthread_create(&barber, &barber_attr, barber_room, NULL);
    pthread_create(&customersGenerator, &customersGenerator_attr, generate_customer, NULL);

    pthread_join(barber, NULL);
    pthread_join(customersGenerator, NULL);

    return 0;
}
