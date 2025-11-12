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
 
 




}

