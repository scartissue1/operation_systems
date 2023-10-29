#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int num_validation(const char * number) {
    size_t size = strlen(number);
    for (size_t i = 0; i < size; i++) {
        if (!((number[i] >= '0') && (number[i] <= '9'))) return 0;
    }
    return 1;
}

typedef enum {
    MAN,
    WOMAN,
    FREE
} gender;

gender room_sign = FREE;

pthread_cond_t man_cond, woman_cond;
pthread_mutex_t mutex;

int max_people;
int in_room = 0;


void man_wants_to_enter() {
    pthread_mutex_lock(&mutex);
    while (in_room == max_people || room_sign == WOMAN) pthread_cond_wait(&woman_cond, &mutex);
    if (room_sign == FREE) printf("bathroom is for man now\n");
    room_sign = MAN;
    in_room++;
    printf("man entered\npeople in bath is %d now\n", in_room);
    pthread_mutex_unlock(&mutex);
}

void man_leaves() {
    pthread_mutex_lock(&mutex);
    in_room--;
    if (!in_room) {
        room_sign = FREE;
        printf("bath is free\n");
        pthread_cond_broadcast(&man_cond);
    }
    pthread_mutex_unlock(&mutex);
}

void * man_is_doing_deeds(void * args) {
    man_wants_to_enter();
    sleep(1);
    man_leaves();
    return NULL;
}

void woman_wants_to_enter() {
    pthread_mutex_lock(&mutex);
    while (in_room == max_people || room_sign == MAN) pthread_cond_wait(&man_cond, &mutex);
    if (room_sign == FREE) printf("bathroom is for woman now\n");
    room_sign = WOMAN;
    in_room++;
    printf("woman entered\npeople in bath is %d now\n", in_room);
    pthread_mutex_unlock(&mutex);
}

void woman_leaves() {
    pthread_mutex_lock(&mutex);
    in_room--;
    if (!in_room) {
        room_sign = FREE;
        printf("bath is free\n");
        pthread_cond_broadcast(&woman_cond);
    }
    pthread_mutex_unlock(&mutex);
}

void * woman_is_doing_deeds(void * args) {
    woman_wants_to_enter();
    sleep(1);
    woman_leaves();
    return NULL;
}

void bath(const int man_count, const int woman_count) {
    pthread_t man[man_count];
    pthread_t woman[woman_count];
    for (int i = 0; i < man_count; i++) pthread_create(&man[i], NULL, man_is_doing_deeds, NULL);
    for (int i = 0; i < woman_count; i++) pthread_create(&woman[i], NULL, woman_is_doing_deeds, NULL);
    for (int i = 0; i < man_count; i++) pthread_join(man[i], NULL);
    for (int i = 0; i < woman_count; i++) pthread_join(woman[i], NULL);

}

int main(int argc, char * argv[]) {
    if (argc != 4) {
        printf("Wrong flag\n");
        return -1;
    }
    if (!num_validation(argv[1]) || !num_validation(argv[2]) || !num_validation(argv[3])) {
        printf("Wrong number\n");
        return -1;
    }
    max_people = atoi(argv[1]);
    int man_count = atoi(argv[2]);
    int woman_count = atoi(argv[3]);
    if (man_count > max_people || woman_count > max_people) {
        printf("Too many people\n");
        return -1;
    }
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&man_cond, NULL);
    pthread_cond_init(&woman_cond, NULL);

    bath(man_count, woman_count);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&man_cond);
    pthread_cond_destroy(&woman_cond);
    return 0;
}