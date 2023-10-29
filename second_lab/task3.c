#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define TABLE_SIZE 5
sem_t semaphore;

typedef struct {
    char * name;
    unsigned int left_fork;
    unsigned int right_fork;
} philosopher_type;

// lock mutex - take the fork, unlock mutex - put the fork

typedef struct {
    pthread_mutex_t forks_mutexs[TABLE_SIZE];
} table_type;

typedef struct {
    table_type * table;
    philosopher_type * philosopher;
} philosopher_tag;

void philosopher_init(philosopher_type * philosopher, char * pname, unsigned int left_fork, unsigned int right_fork) {
    // philosopher_type * (*philosopher) = (philosopher_type *)malloc(sizeof(philosopher_type));
    philosopher->name = pname;
    philosopher->left_fork = left_fork;
    philosopher->right_fork = right_fork;
}

void table_init(table_type * table) {
    for (size_t i = 0; i < TABLE_SIZE; i++)
        pthread_mutex_init(&table->forks_mutexs[i], NULL);
}

void table_destroy(table_type * table) {
    for (size_t i = 0; i < TABLE_SIZE; i++) {
        pthread_mutex_destroy(&table->forks_mutexs[i]);
    }
}

pthread_mutex_t entry = PTHREAD_MUTEX_INITIALIZER;

void * dinner(void * arguments) {
    philosopher_tag * argument = (philosopher_tag *)arguments;
    philosopher_type * philo = argument->philosopher;
    table_type * table = argument->table;

    unsigned int random;
    do {
        printf("%s is thinking\n", philo->name);
        sem_wait(&semaphore);
        pthread_mutex_lock(&entry);
        pthread_mutex_lock(&table->forks_mutexs[philo->left_fork]);
        
        random = rand() % 10;
        sleep(random);
        
        pthread_mutex_lock(&table->forks_mutexs[philo->right_fork]);
        printf("%s is eating\n", philo->name);

        random = rand() % 20;
        sleep(random);

        pthread_mutex_unlock(&entry);
        pthread_mutex_unlock(&table->forks_mutexs[philo->right_fork]);
        
        random = rand() % 10;
        sleep(random);
        pthread_mutex_unlock(&table->forks_mutexs[philo->left_fork]);
        sem_post(&semaphore);
        printf("%s is stopped eating\n", philo->name);
    } while (1);
}

int main(int argc, char * argv[]) {
    pthread_t threads[TABLE_SIZE];
    philosopher_type philosophers_array[TABLE_SIZE];
    philosopher_tag philosophers_tag_array[TABLE_SIZE];
    table_type table;

    table_init(&table);
    sem_init(&semaphore, 0, TABLE_SIZE - 1);

    philosopher_init(&philosophers_array[0], "Socrates", 0, 1);
    philosopher_init(&philosophers_array[1], "Euclid", 1, 2);
    philosopher_init(&philosophers_array[2], "Aristotle", 2, 3);
    philosopher_init(&philosophers_array[3], "Platon", 3, 4);
    philosopher_init(&philosophers_array[4], "Kant", 4, 0);

    for (size_t i = 0; i < TABLE_SIZE; i++) {
        philosophers_tag_array[i].philosopher = &philosophers_array[i];
        philosophers_tag_array[i].table = &table;
    }
    for (size_t i = 0; i < TABLE_SIZE; i++) {
        pthread_create(&threads[i], NULL, dinner, &philosophers_tag_array[i]);
    }
    for (size_t i = 0; i < TABLE_SIZE; i++) {
        pthread_join(threads[i], NULL);
    }
    table_destroy(&table);
    sem_destroy(&semaphore);
    return 0;
}