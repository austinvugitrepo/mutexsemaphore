// austin vu 
#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>


char **arg;
int totalargs;
 
pthread_mutex_t mutex;
sem_t empsem;          // for the mutex and semaphore
sem_t fullsem;

buffer_item bufferarr[BUFFER_SIZE]; //array declaration from buffer.h

int in = 0;  // indexes for the empty and full semaphores
int out = 0;

void* prodthr(void* argument); //thread func prototypes
void* consthr(void* argument);

int gsimtime;
int gmaxsleep;  //global variants of arguments defined in main
bool gbuffshow;
bool running = true;

int prodtotal = 0;
int constotal = 0; // for the final summary list top half
int fbuffertime = 0;           
int ebuffertime = 0;                                       //both these for the final summary list

int *totalnumprod; // array for per thread number stats
int *totalnumcons;


bool buffer_insert_item( buffer_item item ) {

int empcount; // check to see how many empty slots
sem_getvalue(&empsem, &empcount);

if(empcount == 0){ // if full
  printf("All buffers full. Producer %p waits.\n", (void*)pthread_self());
  fbuffertime++;
}
sem_wait(&empsem);  //acquiring the mutex and semaphore
pthread_mutex_lock(&mutex);

bufferarr[in] = item; // circular
in = (in + 1) % BUFFER_SIZE;

pthread_mutex_unlock(&mutex); //releasing lock and semaphore
sem_post(&fullsem);

 return true; // return the bool
}

bool buffer_remove_item( buffer_item *item ) {

int fullcount; // check to see how many full slots
sem_getvalue(&fullsem, &fullcount);

if(fullcount == 0){ // if empty
  printf("All buffers empty. Consumer %p waits.\n", (void*)pthread_self());
  ebuffertime++;
}
sem_wait(&fullsem);  //acquiring the mutex and semaphore
pthread_mutex_lock(&mutex);

*item = bufferarr[out]; // circular but inverse to remove
bufferarr[out] = -1; // to show empty slot
out = (out + 1) % BUFFER_SIZE;

pthread_mutex_unlock(&mutex); //releasing lock and semaphore
sem_post(&empsem);

 return true; // return the bool
}

void print_buffer_snapshot() {

    int full_count;

    sem_getvalue(&fullsem, &full_count); //reads how many items are in buffer
    printf("(buffers occupied: %d)\n", full_count);

    printf("buffers:");
    for (int i = 0; i < BUFFER_SIZE; i++) {

        printf(" %d", bufferarr[i]);  //printing all buffer stuff
    }
    printf("\n");

    printf("---- ---- ---- ---- ----\n");

    for (int i = 0; i < BUFFER_SIZE; i++) {

        if (i == out && i == in){
            printf("RW ");
    }
        else if (i == out){  //printing rw markers as shown from the assignment PDF
            printf("R  ");
    }
        else if (i == in){
            printf("W  ");
    }
        else
            printf("   ");
    }

    printf("\n");
}


int main( int argc, char *argv[] ) {  // accepting command args here
 
 bool buffshow; 

 arg = &argv[1]; // to ignore executable name
 totalargs = argc - 1;  
 
 int simtime = atoi(arg[0]);
 int maxsleep = atoi(arg[1]);  // turn command args (strings) to integers
 int prodnum = atoi(arg[2]);   // number of producer threads
 int consnum = atoi(arg[3]);   // number of consumer threads
 
 if (strcmp(arg[4], "yes") == 0) {
   buffshow = true;
 } 
 else if (strcmp(arg[4], "no") == 0) {    // this section compares argument string
   buffshow = false;
 }
 else {
   fprintf(stderr, "for last argument must be yes or no\n");
   exit(1);
 }
 
 totalnumprod = malloc(prodnum * sizeof(int)); //allocating memory for array for per thread number stats
 for(int i = 0; i < prodnum; i++){
  totalnumprod[i] = 0; // initialize this to zero so we can use this array thing as counter
 }
 totalnumcons = malloc(consnum * sizeof(int)); // (final summary output)
 for(int i = 0; i < consnum; i++){
  totalnumcons[i] = 0; // initialize this to zero so we can use this array thing as counter
 }
 
 
 gsimtime = simtime;
 gmaxsleep = maxsleep; //storing these to global variants
 gbuffshow = buffshow;

 pthread_t prodcer[prodnum]; // for the producer and consumer threads 
 pthread_t consmer[consnum];
 
 pthread_mutex_init( &mutex, NULL);  //initialization
 sem_init(&empsem, 0, BUFFER_SIZE);
 sem_init(&fullsem, 0, 0);

  for (int i = 0; i < BUFFER_SIZE; i++){
    bufferarr[i] = -1;                    // making all buffer slot start empty 
  }


for(int i = 0; i < prodnum; i++){
 pthread_create(&prodcer[i], NULL, prodthr, (void*)(long)i); //modified :passing thread index
 } //creating producer and consumer threads
for(int i = 0; i < consnum; i++){
 pthread_create(&consmer[i], NULL, consthr, (void*)(long)i);
}
   
   sleep(gsimtime);  // to end the threads 
   running = false;

for(int i = 0; i < prodnum; i++){
 pthread_join(prodcer[i], NULL); //joining threads
}
for(int i = 0; i < consnum; i++){
 pthread_join(consmer[i], NULL);
  }

//for number of items remaining in buffer:
int remaining = 0;
sem_getvalue(&fullsem, &remaining);

// final summary list:

printf("PRODUCER / CONSUMER SIMULATION COMPLETE\n");
printf("=======================================\n");

printf("Simulation Time: %d\n", gsimtime);
printf("Maximum Thread Sleep Time: %d\n", gmaxsleep);

printf("Number of Producer Threads: %d\n", prodnum);
printf("Number of Consumer Threads: %d\n", consnum);

printf("Size of Buffer: %d\n", BUFFER_SIZE);
printf("Total Number of Items Produced: %d\n", prodtotal);

for (int i = 0; i < prodnum; i++){
 printf("Thread %d: %d\n", i+1, totalnumprod[i]);    // should be structured like final assignment
}
printf("Total Number of Items Consumed: %d\n", constotal);

for (int i = 0; i < consnum; i++){
 printf("Thread %d: %d\n", i+1, totalnumcons[i]);
}

printf("Number Of Items Remaining in Buffer: %d\n", remaining);
printf("Number Of Times Buffer Was Full: %d\n", fbuffertime);
printf("Number Of Times Buffer Was Empty: %d\n", ebuffertime);

free(totalnumprod);
free(totalnumcons);


  return 0;

}

// producer thread function
void* prodthr(void* argument) {
 
 int id = (int)(long)argument; //unpacking the argument passed
 uintptr_t tid = (uintptr_t)pthread_self();
 unsigned int seed = (unsigned int)(time(NULL) ^ tid);  // making random seed by combining time with thread id
 
 while(running){

  usleep((rand_r(&seed) % gmaxsleep) * 100000); //sleeping for rand time
  buffer_item item = rand_r(&seed); // making random number for buffer 
  buffer_insert_item(item); // insert function
  
  prodtotal++;          // this is where the array for per thread number stat kicks in
  totalnumprod[id]++;
    
  printf("Producer %p writes %d\n", (void*)pthread_self(), item);
  
  if(gbuffshow){
      print_buffer_snapshot();
    }


  }

  return NULL;
}
//consumer thread function
void* consthr(void* argument) {

 int id = (int)(long)argument; //unpacking the argument passed
 uintptr_t tid = (uintptr_t)pthread_self();
 unsigned int seed = (unsigned int)(time(NULL) ^ tid); // making random seed by combining time with thread id
 
 while(running){

  usleep((rand_r(&seed) % gmaxsleep) * 100000); //sleeping for rand time
  buffer_item item; 
  buffer_remove_item(&item); //remove function

  constotal++;          // this is where the array for per thread number stat kicks in
  totalnumcons[id]++;
    
  printf("Consumer %p reads %d\n", (void*)pthread_self(), item);
  
  // prime checker
  bool prime = true;
  if (item <= 1){
      prime = false; // 0 and 1 not prime 
    } else {
      for (int i =2; i * i <= item; i++){ // number greater then 2 check for divisors
           
        if(item % i == 0){
         prime = false;      // if divide cleanly not prime so end 
         break;
        }
 
      }

    }

  if(prime){

    printf("* * * PRIME * * *\n"); 
    }
  if(gbuffshow){
      print_buffer_snapshot();
    }


  }

  return NULL;
}



