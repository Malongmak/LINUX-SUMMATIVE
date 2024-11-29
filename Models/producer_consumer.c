#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_BOTTLES 10 

int bottles_on_queue = 0;
pthread_mutex_t mutex;     
pthread_cond_t cond_producer; 
pthread_cond_t cond_consumer; 

void *producer(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        while (bottles_on_queue >= MAX_BOTTLES) {
            pthread_cond_wait(&cond_producer, &mutex);
        }

        bottles_on_queue++;
        printf("Producer: Added a bottle. Queue size: %d\n", bottles_on_queue);

        pthread_cond_signal(&cond_consumer);

        pthread_mutex_unlock(&mutex);

        sleep(2);
    }
    return NULL;
}

void *consumer(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        while (bottles_on_queue == 0) {
            pthread_cond_wait(&cond_consumer, &mutex);
        }

        bottles_on_queue--;
        printf("Consumer: Removed a bottle. Queue size: %d\n", bottles_on_queue);

        pthread_cond_signal(&cond_producer);

        pthread_mutex_unlock(&mutex);

        sleep(3);
    }
    return NULL;
}

int main() {
    pthread_t producer_thread, consumer_thread;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_producer, NULL);
    pthread_cond_init(&cond_consumer, NULL);

    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);

    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_producer);
    pthread_cond_destroy(&cond_consumer);

    return 0;
}
