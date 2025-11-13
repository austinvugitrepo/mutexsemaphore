// austin vu 
#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

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
out = (out + 1) % BUFFER_SIZE;

pthread_mutex_unlock(&mutex); //releasing lock and semaphore
sem_post(&empsem);

 return true; // return the bool
}







bool buffer_remove_item( buffer_item *item );

int main( int argc, char *argv[] ) {  // accepting command args here

 arg = &argv[1]; // to ignore executable name
 totalargs = argc - 1;  
 
 pthread_t prodcer; // for the producer and consumer threads
 pthread_t consmer;
 
 pthread_mutex_init( &mutex, NULL);  //initialization
 sem_init(&empsem, 0, BUFFER_SIZE);
 sem_init(&fullsem, 0, 0);

 pthread_create(&prodcer, NULL, prodthr, NULL); //creating producer and consumer threads
 pthread_create(&consmer, NULL, consthr, NULL);
 
 pthread_join(prodcer, NULL); //threads end here
 pthread_join(consmer, NULL);

}

