#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <errno.h>

#define __NR_set_proc_state 548

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <pid> <state_hex_value>\n", argv[0]);
        return 1;
    }

    pid_t pid = atoi(argv[1]);
    int state = strtol(argv[2], NULL, 16);

    if (syscall(__NR_set_proc_state, pid, state) < 0) {
        perror("set_proc_state failed");
        return 1;
    }

    printf("Process %d state changed to %x\n", pid, state);
    return 0;
}

