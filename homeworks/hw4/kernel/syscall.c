#include "syscall.h"
#include "kernel/printk.h"
#include "kernel/errno.h"
#include "arch/x86/arch.h"
#include "sched/sched.h"
#include "common.h"

int64_t sys_sleep(arch_regs_t* regs) {
    uint64_t ms = syscall_arg0(regs);
    // TODO: implement me.

    return 0;
}

int64_t sys_fork(arch_regs_t* regs);
int64_t sys_getpid(arch_regs_t* regs);
int64_t sys_exit(arch_regs_t* regs);

syscall_fn_t syscall_table[] = {
    [SYS_SLEEP] = sys_sleep,
    [SYS_FORK] = sys_fork,
    [SYS_GETPID] = sys_getpid,
    [SYS_EXIT] = sys_exit,
};

uint64_t do_syscall(uint64_t sysno, arch_regs_t* regs) {
    if (sysno >= SYS_MAX) {
        return -ENOSYS;
    }
    syscall_fn_t syscall = syscall_table[sysno];
    return syscall(regs);
}
