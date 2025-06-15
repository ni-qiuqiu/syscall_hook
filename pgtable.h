
#ifndef HOOK__PGTABLE__H
#define HOOK__PGTABLE__H
#include <linux/kernel.h>
#include <linux/mm.h>
#include <asm/pgtable-types.h>

pte_t *get_pte(unsigned long kaddr);
pte_t *page_from_virt_kernel(unsigned long addr);

pte_t *page_from_virt_user(struct mm_struct *mm, unsigned long addr);

int protect_rodata_memory(unsigned long addr);

int unprotect_rodata_memory(unsigned long addr);

#endif 
