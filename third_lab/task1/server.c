#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

typedef enum {
    OK,
    KEY_ERROR,
    QUEUE_INIT_ERROR,
    INVALID_PARAMETER
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
    }
}

struct msg_buffer {
    long msg_type;
    char msg_text[BUFSIZ];
};

const int PRIOR_MAX = 100;

status_codes messageProcessing(char *message) {
    if (!message) {
        return INVALID_PARAMETER;
    }
    size_t message_size = strlen(message);
    char copy[message_size];
    int space_flag = 0;
    size_t copy_size = 0;
    for (int i = 0; i < message_size; i++) {
        if (!isspace(message[i]) || !space_flag) {
            copy[copy_size++] = message[i];
        }
        space_flag = (isspace(message[i])) ? 1 : 0;
    }
    copy[copy_size] = '\0';
    strcpy(message, copy);
    return OK;
}

int main(int agrc, char *argv[]) {
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

    while (1) {
        msgrcv(server_queue, &mesg, sizeof(mesg), -PRIOR_MAX, IPC_NOWAIT);
        if (mesg.msg_type == 1) {
            break;
        }
        printf("%ld\n", mesg.msg_type);
        if (messageProcessing(mesg.msg_text) == INVALID_PARAMETER) {
            errorProcessing(INVALID_PARAMETER);
            msgctl(server_queue, IPC_RMID, NULL);
            msgctl(client_queue, IPC_RMID, NULL);
            return -1;
        }
        mesg.msg_type = 1;
        msgsnd(client_queue, &mesg, sizeof(mesg), 0);
    }
    msgctl(server_queue, IPC_RMID, NULL);
    msgctl(client_queue, IPC_RMID, NULL);
    return 0;
}
