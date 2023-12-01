#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

typedef enum {
    OK,
    KEY_ERROR,
    QUEUE_INIT_ERROR,
    INVALID_PARAMETER,
    NO_FILE
} status_codes;

void errorProcessing(const status_codes error) {
    switch (error) {
        case KEY_ERROR:
            printf("Wrong server/client key detected\n");
            return;
        case QUEUE_INIT_ERROR:
            printf("Unable to open message queue\n");
            return;
        case INVALID_PARAMETER:
            printf("Invalid message detected\n");
            return;
        case NO_FILE:
            printf("No such file\n");
            return;
    }
}

struct msg_buffer {
    long msg_type;
    char msg_text[BUFSIZ];
};

const int PRIOR_MAX = 100;

int main() {

    key_t client_key = ftok("client.txt", 1);
    key_t server_key = ftok("server.txt", 1);
    
    if (client_key == -1 || server_key == -1) {
        errorProcessing(KEY_ERROR);
        return -1;
    }
    
    int client_queue = msgget(client_key, 0666 | IPC_CREAT);
    int server_queue = msgget(server_key, 0666 | IPC_CREAT);

    if (client_queue == -1 || server_queue == -1) {
        errorProcessing(QUEUE_INIT_ERROR);
        msgctl(server_queue, IPC_RMID, NULL);
        msgctl(client_queue, IPC_RMID, NULL);
        return -1;
    }
    struct msg_buffer mesg;

    FILE *stream = fopen("message.txt", "r");
    if (!stream) {
        errorProcessing(NO_FILE);
        msgctl(server_queue, IPC_RMID, NULL);
        msgctl(client_queue, IPC_RMID, NULL);
        return -1;
    }
    
    int count = 0;
    char buf[BUFSIZ];
    while (fgets(buf, sizeof(char) * BUFSIZ, stream) != NULL) {
        count++;
        strtok(buf, "=");
        char *string_priority = strtok(NULL, " ");
        if (!string_priority) {
            errorProcessing(NO_FILE);
            msgctl(server_queue, IPC_RMID, NULL);
            msgctl(client_queue, IPC_RMID, NULL);
            return -1;
        }
        int priority = atoi(string_priority);
        priority = (priority < 0) ? 0 : priority;
        priority = (priority > PRIOR_MAX - 1) ? PRIOR_MAX - 1 : priority;
        mesg.msg_type = priority;
        printf("%ld\n", mesg.msg_type);
        strtok(NULL, "\"");
        char *message = strtok(NULL, "\n");
        if (!message) {
            errorProcessing(INVALID_PARAMETER);
            msgctl(server_queue, IPC_RMID, NULL);
            msgctl(client_queue, IPC_RMID, NULL);
            fclose(stream);
            return -1;
        }
        printf("%s\n", message);
        size_t message_size = strlen(message);
        for (int i = 0; i < message_size - 2; i++) {
            mesg.msg_text[i] = message[i];
        }
        msgsnd(server_queue, &mesg, sizeof(mesg), 0);
    }
    for (int i = 0; i < count; i++) {
        msgrcv(client_queue, &mesg, sizeof(mesg), -PRIOR_MAX, IPC_NOWAIT);
        printf("%s\n", mesg.msg_text);
    }
    mesg.msg_type = PRIOR_MAX;
    msgsnd(server_queue, &mesg, sizeof(mesg), 0);
    fclose(stream);
    msgctl(server_queue, IPC_RMID, NULL);
    msgctl(client_queue, IPC_RMID, NULL);
    return 0;
}
