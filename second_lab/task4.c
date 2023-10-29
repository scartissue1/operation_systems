#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int get_name(FILE * file, char *** names, size_t * size) {
    size_t capacity = 32;
    (*size) = 0;
    (*names) = (char **)malloc(sizeof(char *) * capacity);
    if (!(*names)) return -1;
    char filename[BUFSIZ];
    while (fscanf(file, "%s", filename) != EOF) {
        (*names)[(*size)] = strdup(filename);
        (*size)++;
        if ((*size) >= capacity) {
            capacity *= 2;
            char ** tmp = (char **)realloc((*names), capacity);
            if (!tmp) {
                free(*names);
                return -1;
            }
            (*names) = tmp;
        }
    }
    return 0;
}

int substring_finder(FILE * file, const char * substring) {
    char * buf = NULL;
    int read_status;
    while ((read_status = getline(&buf, &(size_t){0}, file)) != -1) {
        char * found_status = strstr(buf, substring);
        if (found_status) return 1;
        free(buf);
        buf = NULL;
    }
    return 0;
}

int main(int argc, char * argv[]) {
    if (argc != 3) {
        printf("Wrong flag\n");
        return -1;
    }
    FILE * main_file = fopen(argv[2], "r");
    if (!main_file) {
        printf("No file\n");
        return -1;
    }
    const char * substring = argv[1];
    char ** names = NULL;
    size_t names_size = 0;
    if (get_name(main_file, &names, &names_size) == -1) {
        printf("No memory\n");
        fclose(main_file);
        return -1;
    }
    pid_t process_id;
    for (size_t i = 0; i < names_size; i++) {
        process_id = fork();
        if (!process_id) {
            FILE * current = fopen(names[i], "r");
            if (!current) {
                printf("Unable to open %s file\n", names[i]);
                return -1;
            }
            if (substring_finder(current, substring)) {
                printf("%s\n", names[i]);
            }
            fclose(current);
            free(names[i]);
            exit(0);
        }
        else if (process_id == -1) exit(-1);
    }
    exit(0);
    fclose(main_file);
    free(names);
    return 0;
}