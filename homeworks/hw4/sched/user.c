#include <stdint.h>

#include "kernel/syscall.h"

#define USER_TEXT __attribute__((section(".user.text,\"ax\",@progbits#")))

#define SYSCALL0(n, res) __asm__ volatile ("syscall" : "=a"(res) : "a"(n) : "rdi", "rsi", "rdx", "rcx", "r8", "r9", "r10", "r11", "memory" )
#define SYSCALL1(n, arg0, res) __asm__ volatile ("syscall" : "=a"(res) : "a"(n), "D"(arg0) : "rsi", "rdx", "rcx", "r8", "r9", "r10", "r11", "memory" )
#define SYSCALL2(n, arg0, arg1, res) __asm__ volatile ("syscall" :  "=a(res)": "a"(n), "D"(arg0), "S"(arg1) : "rdx", "rcx", "r8", "r9", "r10", "r11", "memory" )

USER_TEXT int64_t getpid() {
    int64_t res;
    SYSCALL0(SYS_GETPID, res);
    return res;
}

USER_TEXT int64_t sleep(uint64_t arg) {
    int64_t res;
    SYSCALL1(SYS_SLEEP, arg, res);
    return res;
}

USER_TEXT int64_t fork() {
    int64_t res;
    SYSCALL0(SYS_FORK, res);
    return res;
}

USER_TEXT int64_t exit(uint64_t arg) {
    int64_t res;
    SYSCALL1(SYS_EXIT, arg, res);
    return res;
}

USER_TEXT int main() {
    int64_t pid = getpid();

    int err = fork();
    if (err < 0) {
        return -1;
    }

    if (err == 0) {
        sleep(getpid());
    } else {
        sleep(pid);
    }

    return 0;
}

USER_TEXT void user_program() {
    exit(main());
}
