#ifndef UTIL_H
#define UTIL_H

#include <linux/types.h>
#include <linux/printk.h>

#define TAG "hook"

#define LOG_INFO(fmt, ...) printk(KERN_INFO "[%s] " fmt, TAG, ##__VA_ARGS__)
#define LOG_ERR(fmt, ...) printk(KERN_ERR "[%s] " fmt, TAG, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) printk(KERN_WARNING "[%s] " fmt, TAG, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) printk(KERN_DEBUG "[%s] " fmt, TAG, ##__VA_ARGS__)
#define LOG_EMERG(fmt, ...) printk(KERN_EMERG "[%s] " fmt, TAG, ##__VA_ARGS__)
#define LOG_ALERT(fmt, ...) printk(KERN_ALERT "[%s] " fmt, TAG, ##__VA_ARGS__)
#define LOG_CRIT(fmt, ...) printk(KERN_CRIT "[%s] " fmt, TAG, ##__VA_ARGS__)

unsigned long m_kallsyms_lookup_name(const char *symbol_name);

unsigned long *find_syscall_table(void);

#endif