#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <errno.h>

#define __NR_get_proc_state 549
#define __NR_get_proc_state_string 550

int main(int argc, char *argv[]) {
    pid_t pid;
    long state;
    char state_str[256];

    if (argc > 1) {
        pid = atoi(argv[1]);
    } else {
        pid = 0;
    }

    state = syscall(__NR_get_proc_state, pid);
    if (state < 0) {
        perror("get_proc_state failed");
        return 1;
    }

    if (syscall(__NR_get_proc_state_string, pid, state_str, sizeof(state_str)) < 0) {
        perror("get_proc_state_string failed");
        return 1;
    }

    printf("pid: %d, state_hex: %lx, state_string: %s\n", pid, state, state_str);
    return 0;
}

