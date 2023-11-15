#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "status_codes.h"
#include "Pqueue.h"
#include "erproc.h"

status_codes getMessage(Pqueue **root, char *filename, int *overall_index) {
    if (!filename) {
        return INVALID_PARAMETER;
    }
    FILE *stream = fopen(filename, "r");
    if (!stream) {
        return NO_FILE;
    }
    char symbol = fgetc(stream);
    if (symbol == EOF) {
        return INVALID_PARAMETER;
    }
    fseek(stream, 0, SEEK_SET);
    char *command = NULL;
    while (getline(&command, &(size_t){0}, stream) != -1) {
        if (!strlen(command)) {
            continue;
        }
        if (!isdigit(command[6])) {
            free(command);
            command = NULL;
            continue;
        }
        size_t priority_string_size = 0;
        size_t priority_string_capacity = 2;
        char *priority_string = (char *)malloc(sizeof(char) * 2);
        if (!priority_string) {
            free(command);
            freePqueue(*root);
            free(priority_string);
            fclose(stream);
            return NO_MEMORY;
        }
        int index = 6;
        while (isdigit(command[index])) {
            priority_string[priority_string_size] = command[index];
            index++;
            priority_string_size++;
            if (priority_string_size >= priority_string_capacity) {
                priority_string_capacity *= 2;
                char *tmp = (char *)realloc(priority_string, sizeof(char) * priority_string_capacity);
                if (!tmp) {
                    free(command);
                    freePqueue(*root);
                    free(priority_string);
                    fclose(stream);
                    return NO_MEMORY;
                }
                priority_string = tmp;
            }
        }
        int priority = atoi(priority_string);
        free(priority_string);
        priority_string = NULL;
        size_t command_size = strlen(command);
        size_t message_size;
        if ((int)command[command_size - 1] == 34) {
            message_size = command_size - 14 - priority_string_size;
        }
        else { 
            message_size = command_size - 16 - priority_string_size;
        }
        char *message = (char *)malloc(sizeof(char) * message_size);
        if (!message) {
            return NO_MEMORY;
        }
        for (int i = 0; i < message_size; i++) {
            message[i] = command[i + 13 + priority_string_size];
        }
        message[message_size] = '\0';
        (*overall_index) += message_size + 1;
        if (!(*root) || (*root)->priority < priority) {
            if (push_forwardPqueue(root, priority, message, message_size) == NO_MEMORY) {
                free(message);
                free(command);
                freePqueue(*root);
                return NO_MEMORY;
            }
        }
        else if (push_backPqueue((*root), priority, message, message_size) == NO_MEMORY) {
            free(message);
            free(command);
            freePqueue(*root);
            return NO_MEMORY;
        }
        free(command);
        free(message);
        command = NULL;
        message = NULL;
    }
    free(command);
    fclose(stream);
    return OK;
}

status_codes processingMessage(Pqueue *root) {
    size_t process_string_size = 0;
    size_t process_string_capacity = 2;
    char *process_string = (char *)malloc(sizeof(char) * process_string_capacity);
    if (!process_string) {
        return NO_MEMORY;
    }
    while (root) {
        int changes_flag = 0;
        for (int i = 0; i < root->data_size; i++) {
            process_string[process_string_size] = root->data[i];
            process_string_size++;
            if (process_string_size >= process_string_capacity) {
                process_string_capacity *= 2;
                char *tmp = (char *)realloc(process_string, sizeof(char) * process_string_capacity);
                if (!tmp) {
                    free(process_string);
                    return NO_MEMORY;
                }
                process_string = tmp;
            }
            int splitter_skip_index = i;
            while ((root->data[splitter_skip_index] == ' ' || root->data[splitter_skip_index] == '\t') && splitter_skip_index < root->data_size) {
                splitter_skip_index++;
                changes_flag = 1;
            }
            i = (splitter_skip_index == i) ? splitter_skip_index : splitter_skip_index - 1; 
        }
        process_string[process_string_size] = '\0';
        if (changes_flag) {
            free(root->data);
            root->data = NULL;
            root->data = (char *)malloc(sizeof(char) * process_string_size);
            if (!root->data) {
                free(process_string);
                return NO_MEMORY;
            }
            root->data_size = process_string_size;
            memcpy(root->data, process_string, process_string_size);
        }
        process_string_capacity = 2;
        process_string_size = 0;
        char *tmp = (char *)realloc(process_string, sizeof(char) * process_string_capacity);
        if (!tmp) {
            free(process_string);
            return NO_MEMORY;
        }
        process_string = tmp;
        root = root->next;
    }
    free(process_string);
    return OK;
}

status_codes writeMessage(Pqueue *root, int fd, int overall_index) {
    int index = 0;
    char *result = (char *)malloc(sizeof(char) * overall_index);
    if (!result) {
        return NO_MEMORY;
    }
    while (root) {
        for (int i = 0; i < root->data_size; i++) {
            result[index] = root->data[i];
            index++;
        }
        result[index++] = '\n';
        root = root->next;
    }
    result[index] = '\0';
    write(fd, result, overall_index);
    free(result);
    return OK;
}

int main(int argc, char *argv[]) {
    int server = Socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(34543);
    Bind(server, (struct sockaddr *) &addr, sizeof addr);
    Listen(server, 5);
    socklen_t addrlen = sizeof addr;
    int fd = Accept(server, (struct sockaddr *) &addr, &addrlen);
    ssize_t nread;
    char buf[256];
    while (nread = read(fd, buf, 256) != 0) {
        Pqueue *pqueue = NULL;
        int overall_index = 0;
        switch (getMessage(&pqueue, buf, &overall_index)) {
            case OK:
                break;
            case NO_MEMORY:
                write(fd, "No memory in server\n", 21);
                close(fd);
                close(server);
                return -1;
            case NO_FILE:
                write(fd, "No such file\n", 14);
                close(fd);
                close(server);
                return -1;
            case INVALID_PARAMETER:
                write(fd, "Invalid parameter detected\n", 28);
                close(fd);
                close(server);
                return -1;
        }
        if (processingMessage(pqueue) == NO_MEMORY) {
            write(fd, "No memory in server\n", 21);
            freePqueue(pqueue);
            close(fd);
            close(server);
            return -1;
        }
        writeMessage(pqueue, fd, overall_index);
        freePqueue(pqueue);
    }
    close(fd);
    close(server);
    return 0;
}
