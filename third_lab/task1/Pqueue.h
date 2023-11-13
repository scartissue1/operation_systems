#ifndef PQUEUE_H__
#define PQUEUE_H__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "status_codes.h"

typedef struct Pqueue {
    int priority;
    char *data;
    size_t data_size;
    struct Pqueue *prev;
    struct Pqueue *next;
} Pqueue;

status_codes push_forwardPqueue(Pqueue **root, const int _priority, const char *_data, const size_t _data_size);

status_codes push_backPqueue(Pqueue *root, const int _priority, const char *_data, const size_t _data_size);

status_codes get_fromPqueue(Pqueue **root, char **_data);

void freePqueue(Pqueue *root);

void printPqueue(Pqueue *root);

#endif