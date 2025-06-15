#include "pgtable.h"
#include "syscall.h"
#include "util.h"

#include <asm/uaccess.h>

long (*old_sys_openat)(int dfd, const char __user *filename,
                       int flags, umode_t mode) = NULL;

long sys_openat(int dfd, const char __user *filename,
                int flags, umode_t mode)
{

    char name[255];
    strncpy_from_user(name, filename, 255);
    LOG_INFO("open %s\n", name);

    return old_sys_openat(dfd, filename, flags, mode);
}

void sys_replace(int nr, void *new_func, void **old_func)
{

    unsigned long *systable = find_syscall_table();

    uint64_t addr = (uint64_t)(systable + nr);
    int ret = unprotect_rodata_memory((uintptr_t)addr);

    if (ret < 0)
    {
        LOG_DEBUG("sys_replace error %d", ret);

        return;
    }

    *(uintptr_t *)old_func = *(uintptr_t *)addr;
    *(uintptr_t *)addr = (uintptr_t)new_func;

    LOG_DEBUG("sys addr: %llx", addr);
    LOG_DEBUG("newfun: %llx", *(uintptr_t *)addr);
    LOG_DEBUG("old_func: %llx", *(uintptr_t *)old_func);

    protect_rodata_memory(addr);
}

void sys_restore(int nr, void *old_func)
{
    unsigned long *systable = find_syscall_table();
    uintptr_t *target = &systable[nr];
    unprotect_rodata_memory((uintptr_t)target);
    target = (uintptr_t *)old_func;
    protect_rodata_memory((uintptr_t)target);
}

void msys_init(int nr)
{

    sys_replace(nr, sys_openat, (void **)&old_sys_openat);
}

void msys_exit(int nr)
{
    if (old_sys_openat)
    {
        sys_restore(nr, old_sys_openat);
    }
}