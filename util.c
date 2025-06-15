
#include <linux/kprobes.h>
#include <linux/version.h>
#include "util.h"

unsigned long m_kallsyms_lookup_name(const char *symbol_name)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 7, 0)
    typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);

    static kallsyms_lookup_name_t lookup_name = NULL;
    if (lookup_name == NULL)
    {
        struct kprobe kp = {
            .symbol_name = "kallsyms_lookup_name"};

        if (register_kprobe(&kp) < 0)
        {
            return 0;
        }

        // 高版本一些地址符号不再导出，需要通过kallsyms_lookup_name获取
        // 但是kallsyms_lookup_name也是一个不导出的内核符号，需要通过kprobe获取
        lookup_name = (kallsyms_lookup_name_t)kp.addr;
        unregister_kprobe(&kp);
    }
    return lookup_name(symbol_name);
#else
    return kallsyms_lookup_name(symbol_name);
#endif
}

unsigned long *find_syscall_table(void)
{
    unsigned long *syscall_table;
    syscall_table = (unsigned long *)m_kallsyms_lookup_name("sys_call_table");
    return syscall_table;
}
