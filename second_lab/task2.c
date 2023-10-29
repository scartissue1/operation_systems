#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char * argv[]) {
    fork();
    printf("Hi\n");
    fork();
    printf("Hi\n");
    return 0;
}