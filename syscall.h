
#ifndef __HOOK_SYSCALL_H
#define __HOOK_SYSCALL_H

#include <linux/completion.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/mm.h>



void msys_init(int nr);

void msys_exit(int nr);



#endif 
