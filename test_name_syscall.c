#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>

#define __NR_name 551

int main(int argc, char *argv[]) {
    long rv = syscall(__NR_ismin, "Hello from my syscall!\n");
    
    if (rv < 0) {
        perror("Syscall failed");
    } else {
        printf("Syscall ran successfully, check dmesg output.\n");
    }

    return 0;
}

