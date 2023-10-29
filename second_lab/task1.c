#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char * argv[]) {
    pid_t process_id = getpid();
    printf("Current process id: %d\n", process_id);
    pid_t parrent_process_id = getppid();
    printf("Current parrent process id: %d\n", parrent_process_id);
    gid_t group_id = getpgrp();
    printf("Group ID: %d\n", group_id);
    uid_t user_id = getuid();
    printf("Real User ID: %d\n", user_id);
    gid_t real_group_id = getgid();
    printf("Real Group ID: %d\n", real_group_id);
    uid_t effective_current_user_id = geteuid();
    printf("Effective User ID: %d\n", effective_current_user_id);
    gid_t effective_group_id = getegid();
    printf("Effective Group ID: %d\n", effective_group_id);
    return 0;
}
