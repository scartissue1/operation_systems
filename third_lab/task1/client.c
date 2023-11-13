#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "erproc.h"

#define END_OF_CONVERSATION "END\0"

int main(int argc, char *argv[]) {
    int fd = Socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(34543);
    Inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    Connect(fd, (struct sockaddr *) &addr, sizeof addr);
    while (1) {
        printf("Input the file path: (Or Q if you want to close the connection)\n");
        char buf[128];
        scanf("%s", buf);

        if (!strcmp(buf, "Q")) {
            break;
        }

        write(fd, buf, 128);
        read(fd, buf, 127);
        printf("%s\n", buf);
    }
    close(fd);
    return 0;
}