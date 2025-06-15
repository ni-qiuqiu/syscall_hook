#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/syscalls.h>
#include <linux/string.h>
#include <linux/kprobes.h>
#include <linux/kallsyms.h>
#include <asm/unistd.h>
#include <linux/unistd.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/kobject.h>
#include <linux/random.h>
#include <linux/version.h>

MODULE_AUTHOR("ni-qiuqiu");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("syscall fphook");
MODULE_VERSION("1.0.0");

#include "util.h"
#include "syscall.h"

// echo 0 > /proc/sys/kernel/kptr_restrict
// cat /sys/fs/pstore/console-ramoops-0 |grep hook
// cd /data/local/tmp
// dmesg | grep hook
#define SYSCALL_ENTRY 56

/*
for low devices ,normal
[ 2872.710647] [hook] open /system/lib64/libnetd_client.so
[ 2872.710650] [hook] open /system/lib64/libnetd_client.so
[ 2872.711504] [hook] open /dev/__properties__/u:object_r:vendor_socket_hook_prop:s0
[ 2872.711507] [hook] open /dev/__properties__/u:object_r:vendor_socket_hook_prop:s0
[ 2872.712946] [hook] open /dev/kmsg
*/

/*
[  840.952965][ T8066] [hook] linux version code 393610
[  840.955340][  T419] CFI failure at invoke_syscall+0x50/0x114 (target: sys_openat+0x0/0xdc [syshook]; expected type: 0xb02b34d9)
high(5.x) device need to bypass cfi

*/


static int __init mod_init(void)
{
	int ret;
	LOG_INFO("linux version code %d", LINUX_VERSION_CODE);

	ret = 0;

	msys_init(SYSCALL_ENTRY);

	return ret;
}

static void __exit mod_exit(void)
{

	msys_exit(SYSCALL_ENTRY);
	LOG_INFO("see you again");
}

module_init(mod_init);
module_exit(mod_exit);
