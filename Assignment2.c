#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

sem_t empty;
sem_t mutex;
sem_t full;
sem_t empty2;
sem_t mutex2;
sem_t full2;

int buffer_milk[9] = {0,0,0,0,0,0,0,0,0};
int buffer_cheese[4] = {0,0,0,0};
int write_idx = 0;
int read_idx = 0;
int cheeseWT_idx = 0;
int cheeseRD_idx = 0;
const int N = 9;

struct arg_struct {
    int id;
    int quantity;
};

void *milk_producer(void* arg) {
    //produce some data
    int i, j, total;

    struct arg_struct *arguments = arg;

    for (int i = 0; i < arguments->quantity; i++) {

        total = arguments->id;

        sem_wait(&empty); //wait(empty)
        sem_wait(&mutex);

        // critical section

        buffer_milk[write_idx] = total;
        write_idx++;
        if (write_idx == N)
            write_idx = write_idx % N;

        sem_post(&mutex);
        sem_post(&full);
    }
}

void *cheese_producer(void* arg) {
    int i;
    int slice[4] = {0,0,0,0};
    int cheese = 0;

    struct arg_struct *arguments = arg;
    slice[3] = arguments->id;
    for (int j = 0; j < arguments->quantity; j++) {
        for (i = 0; i < 3; ++i) {

            sem_wait(&full);
            sem_wait(&mutex);

            slice[i] = buffer_milk[read_idx];
            ++read_idx;
            if (read_idx == N)
                read_idx = read_idx % N;

            sem_post(&mutex);
            sem_post(&empty);
        }

        sem_wait(&empty2); //wait(empty)
        sem_wait(&mutex2);

        buffer_cheese[cheeseWT_idx] = slice[0] * 1000 + slice[1] * 100 + slice[2] *10 
                                    + slice[3];
        cheeseWT_idx++;
        if (cheeseWT_idx == 4)
            cheeseWT_idx = cheeseWT_idx % 4;
        
        sem_post(&mutex2);
        sem_post(&full2);
    }
}

void *cheeseburger_producer(void* arg) {
    int burger[2] = {0,0};
    int i;

    for (i = 0; i < *(int*)arg; ++i) {
        for (int j = 0; j < 2; ++j) {
            sem_wait(&full2);
            sem_wait(&mutex2);

            burger[j] = buffer_cheese[cheeseRD_idx];
            ++cheeseRD_idx;
            if (cheeseRD_idx == 4)
                cheeseRD_idx = cheeseRD_idx % 4;

            sem_post(&mutex2);
            sem_post(&empty2);
        }
        printf("%d%d\n",burger[0], burger[1]);
    } 
}

int main() {
    sem_init(&mutex, 0, 1);
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, N);
    sem_init(&mutex2, 0, 1);
    sem_init(&full2, 0, 0);
    sem_init(&empty2, 0, 4);

    int milkID[3] = {1, 2, 3};
    int cheeseID[2] = {4,5};
    int userInput;
    struct arg_struct args1, args2, args3, args4, args5;

    pthread_t t_p1, t_p2, t_p3;
    pthread_t t_c1, t_c2;
    pthread_t t_b1;

    printf("How many cheeseburgers would you like?\n");
    scanf("%d", &userInput);

    args1.id = milkID[0];
    args1.quantity = userInput * 2;
    args2.id = milkID[1];
    args2.quantity = userInput * 2;
    args3.id = milkID[2];
    args3.quantity = userInput * 2;
    args4.id = cheeseID[0];
    args4.quantity = userInput;
    args5.id = cheeseID[1];
    args5.quantity = userInput;

    pthread_create(&t_p1, NULL, milk_producer, (void*)&args1);
    pthread_create(&t_p2, NULL, milk_producer, (void*)&args2);
    pthread_create(&t_p3, NULL, milk_producer, (void*)&args3);
    pthread_create(&t_c1, NULL, cheese_producer, (void*)&args4);
    pthread_create(&t_c2, NULL, cheese_producer, (void*)&args5);
    pthread_create(&t_b1, NULL, cheeseburger_producer, (void*)&userInput);

    sleep(2);

    pthread_join(t_p1, NULL);
    pthread_join(t_p2, NULL);
    pthread_join(t_p3, NULL);
    pthread_join(t_c1, NULL);
    pthread_join(t_c2, NULL);
    pthread_join(t_b1, NULL);

    sem_destroy(&mutex);
    sem_destroy(&full);
    sem_destroy(&empty);
    sem_destroy(&mutex2);
    sem_destroy(&full2);
    sem_destroy(&empty2);

    return 0;
}