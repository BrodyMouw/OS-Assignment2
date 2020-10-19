#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

sem_t empty;
sem_t mutex;
sem_t full;

int buffer[3] = {0,0,0};
int write_idx = 0;
int read_idx = 0;
const int N = 3;

void *thread_producer(void* arg) {
    //produce some data
    int i, j, total;

    for (int i = 0; i < 2; i++) {
        total = 0;
        for (j=0; j < *(int*)arg + i*30; j++) {
            total = total + j;
        }

        sem_wait(&empty); //wait(empty)
        sem_wait(&mutex);

        // critical section

        buffer[write_idx] = total;
        write_idx++;
        if (write_idx == N)
            write_idx = write_idx % N;

        sem_post(&mutex);
        sem_post(&full);
    }
}

void *thread_consumer(void* arg) {
    int i;

    for (i = 0; i < 3; ++i) {

        sem_wait(&full);
        sem_wait(&mutex);

        printf("%d, counter: %d\n", buffer[read_idx], read_idx);
        ++read_idx;
        if (read_idx == N)
            read_idx = read_idx % N;

        sem_post(&mutex);
        sem_post(&empty);
    }
}

int main() {
    sem_init(&mutex, 0, 1);
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, N);

    int limits[3] = {10, 20, 30};

    pthread_t t_p1, t_p2, t_p3;
    pthread_t t_c1, t_c2;

    pthread_create(&t_p1, NULL, thread_producer, (void*)&limits[0]);
    pthread_create(&t_p2, NULL, thread_producer, (void*)&limits[1]);
    pthread_create(&t_p3, NULL, thread_producer, (void*)&limits[2]);
    pthread_create(&t_c1, NULL, thread_consumer, NULL);
    pthread_create(&t_p1, NULL, thread_consumer, NULL);

    sleep(2);

    pthread_join(t_p1, NULL);
    pthread_join(t_p2, NULL);
    pthread_join(t_p3, NULL);
    pthread_join(t_c1, NULL);
    pthread_join(t_c2, NULL);

    sem_destroy(&mutex);
    sem_destroy(&full);
    sem_destroy(&empty);

    return 0;
}