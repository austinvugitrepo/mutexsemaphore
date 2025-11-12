// austin vu 
#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

char **arg;
int totalargs;
int word_counter = 0;

int main( int argc, char *argv[] ) {  // accepting command args here

 arg = &argv[1]; // to ignore executable name
 totalargs = argc - 1;  
 
 pthread_t prodcer; // for the producer and consumer threads
 pthread_t consmer;
 
 pthread_mutex_t mutex;
 sem_t empsem;          // for the mutex and semaphore
 sem_t fullsem;
 
 pthread_mutex_init( &mutex, NULL);  //initialization
 sem_init(&empsem, 0, BUFFER_SIZE);
 sem_init(&fullsem, 0, 0);

 pthread_create(&prodcer, NULL, prodthr, NULL); //creating producer and consimer threads
 pthread_create(&consmer, NULL, consthr, NULL);


}

