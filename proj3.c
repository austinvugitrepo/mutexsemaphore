// austin vu 
#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <unistd.h>


char **arg;
int totalargs;
 
pthread_mutex_t mutex;
sem_t empsem;          // for the mutex and semaphore
sem_t fullsem;

buffer_item bufferarr[BUFFER_SIZE]; //array declaration

int in = 0;  // indexes for the empty and full semaphores
int out = 0;

void* prodthr(void* argument); //thread func prototypes
void* consthr(void* argument);

int gsimtime;
int gmaxsleep;  //global variants of arguments defined in main
bool gbuffshow;
bool running = true;


bool buffer_insert_item( buffer_item item ) {

sem_wait(&empsem);  //acquiring the mutex and semaphore
pthread_mutex_lock(&mutex);

bufferarr[in] = item; // circular
in = (in + 1) % BUFFER_SIZE;

pthread_mutex_unlock(&mutex); //releasing lock and semaphore
sem_post(&fullsem);

 return true; // return the bool
}

bool buffer_remove_item( buffer_item *item ) {

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
        else if (i == out){  //printing rw markers
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
 pthread_create(&prodcer[i], NULL, prodthr, NULL);
 } //creating producer and consumer threads
for(int i = 0; i < consnum; i++){
 pthread_create(&consmer[i], NULL, consthr, NULL);
}
   
   sleep(gsimtime);  // to end the threads 
   running = false;

for(int i = 0; i < prodnum; i++){
 pthread_join(prodcer[i], NULL); //joining threads
}
for(int i = 0; i < consnum; i++){
 pthread_join(consmer[i], NULL);
  }

  return 0;
}

// producer thread function
void* prodthr(void* argument) {
 unsigned int seed = time(NULL) ^ pthread_self(); // making random seed by combining time with thread id
 
 while(running){

  usleep((rand_r(&seed) % gmaxsleep) * 100000); //sleeping for rand time
  buffer_item item = rand_r(&seed); // making random number for buffer 
  buffer_insert_item(item); // insert function
    
  printf("Producer %lu writes %d\n", pthread_self(), item);
  
  if(gbuffshow){
      print_buffer_snapshot();
    }


  }

  return NULL;
}
//consumer thread function
void* consthr(void* argument) {
 unsigned int seed = time(NULL) ^ pthread_self(); // making random seed by combining time with thread id
 
 while(running){

  usleep((rand_r(&seed) % gmaxsleep) * 100000); //sleeping for rand time
  buffer_item item; 
  buffer_remove_item(&item); //remove function
    
  printf("Consumer %lu reads %d\n", pthread_self(), item);
  
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



