#include "Pqueue.h"

status_codes push_forwardPqueue(Pqueue **root, const int _priority, const char *_data, const size_t _data_size) {
    Pqueue *tmp = (Pqueue *)malloc(sizeof(Pqueue));
    if (!tmp) {
        return NO_MEMORY;
    }
    tmp->priority = _priority;
    tmp->data = (char *)malloc(sizeof(char) * _data_size);
    memcpy(tmp->data, _data, _data_size);
    tmp->data_size = _data_size;
    tmp->prev = NULL;
    tmp->next = (*root);
    (*root) = tmp;
    return OK;
}

status_codes push_backPqueue(Pqueue *root, const int _priority, const char *_data, const size_t _data_size) {
    while (root->next && root->next->priority > _priority) {
        root = root->next;
    }
    Pqueue *tmp = (Pqueue *)malloc(sizeof(Pqueue));
    if (!tmp) {
        return NO_MEMORY;
    }
    tmp->prev = root;
    tmp->priority = _priority;
    tmp->data = (char *)malloc(sizeof(char) * _data_size);
    memcpy(tmp->data, _data, _data_size);
    tmp->data_size = _data_size;
    tmp->next = root->next;
    root->next = tmp;
    return OK;
}

status_codes get_fromPqueue(Pqueue **root, char **_data) {
    if (!(*root)) {
        return INVALID_PARAMETER;
    }
    (*_data) = (char *)malloc(sizeof(char) * ((*root)->data_size));
    if (!(*_data)) {
        return NO_MEMORY;
    }
    memcpy((*_data), (*root)->data, (*root)->data_size);
    Pqueue *tmp = (Pqueue *)malloc(sizeof(Pqueue));
    if (!tmp) {
        return NO_MEMORY;
    }
    tmp = (*root)->next;
    free((*root)->data);
    free(*root);
    *root = tmp;
    return OK;
}

void freePqueue(Pqueue *root) {
    if (!root) {
        return;
    }
    freePqueue(root->next);
    free(root->data);
    free(root);
}

void printPqueue(Pqueue *root) {
    if (!root) {
        return;
    }
    printf("%d %s\n", root->priority, root->data);
    printPqueue(root->next);
}