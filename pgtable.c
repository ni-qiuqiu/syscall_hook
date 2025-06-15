#include "pgtable.h"
#include "util.h"
#include <linux/kallsyms.h>
#include <asm/pgtable.h>
#include <asm/tlbflush.h>
#include <asm/uaccess.h>
#include <asm/unistd.h>
#include <linux/unistd.h>
#include <linux/version.h>
#include <linux/moduleloader.h>
#include <linux/stop_machine.h>

#define pmd_huge(pmd) (pmd_val(pmd) && !(pmd_val(pmd) & PMD_TABLE_BIT))
static struct mm_struct *init_mm_ptr = NULL;

//TODO: modify page tables more safely and generally(like kernelpatch)


pte_t *get_pte(unsigned long kaddr)
{

    pgd_t *pgdp = NULL;
    // version >= 5.8.0 have p4d
    // other p4d_offset(pgd, start) (pgd)
    // so p4d can def
    p4d_t *p4dp = NULL;
    pud_t *pudp = NULL;
    pmd_t *pmdp = NULL;
    pte_t *ptep = NULL;

    if (!init_mm_ptr)
    {
        init_mm_ptr = (struct mm_struct *)m_kallsyms_lookup_name("init_mm");
    }

    pgdp = pgd_offset(init_mm_ptr, kaddr);
    if (pgd_none(*pgdp) || unlikely(pgd_bad(*pgdp)))
    {
        LOG_DEBUG("pgd is null");
        return NULL;
    }

    p4dp = p4d_offset(pgdp, kaddr);
    if (p4d_none(*p4dp) || unlikely(p4d_bad(*p4dp)))
    {
        LOG_DEBUG("p4d is null");
        return NULL;
    }

    pudp = pud_offset(p4dp, kaddr);
    if (pud_none(*pudp) || pud_bad(*pudp))
    {
        LOG_DEBUG("pud is null");
        return NULL;
    }
    // huge page
    pmdp = pmd_offset(pudp, kaddr);
    // hook_info("*pmd=%016llx", pmd_val(*pmdp));
    // syscall内核函数 在大页里面
    if (pmd_huge(*pmdp))
    {
        if (!pte_valid(*(pte_t *)pmdp))
        {
            return NULL;
        }
        return (pte_t *)pmdp;
    }
    if (pmd_none(*pmdp))
    {
        LOG_DEBUG("pmd is null");
        return NULL;
    }

    ptep = pte_offset_kernel(pmdp, kaddr);
    if (pte_none(*ptep) || !pte_present(*ptep))
    {
        LOG_DEBUG("pte is null");
        return NULL;
    }

    return ptep;
}

pte_t *page_from_virt_kernel(unsigned long addr)
{
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pudp, pud;
    pmd_t *pmdp, pmd;
    pte_t *ptep;

    if (addr & PAGE_SIZE - 1)
    {
        addr = addr + PAGE_SIZE & ~(PAGE_SIZE - 1);
    }

    if (!init_mm_ptr)
    {
        init_mm_ptr = (struct mm_struct *)m_kallsyms_lookup_name("init_mm");
    }

    pgd = pgd_offset(init_mm_ptr, addr);
    if (pgd_none(*pgd) || pgd_bad(*pgd))
    {
        return NULL;
    }

    p4d = p4d_offset(pgd, addr);
    if (p4d_none(*p4d) || p4d_bad(*p4d))
    {
        return 0;
    }
    LOG_DEBUG("p4d %llx", p4d);

    pudp = pud_offset(p4d, addr);
    pud = READ_ONCE(*pudp);
    if (pud_none(pud) || pud_bad(pud))
    {
        return NULL;
    }
    LOG_DEBUG("pud %llx", pud);

// need to handle large page
#if defined(pud_leaf)
    // present and not table like syscalltable
    if (pud_leaf(pud))
    {
        LOG_DEBUG("pud_leaf %llx", pud);
        return (pte_t *)pudp;
    }

#endif

    // huge page
    pmdp = pmd_offset(pudp, addr);
    pmd = READ_ONCE(*pmdp);
    if (pmd_none(pmd))
    {
        LOG_DEBUG("pmd bad");
        return NULL;
    }
    LOG_DEBUG("pmd %llx", pmd);
#if defined(pmd_leaf)
    if (pmd_leaf(pmd))
        return (pte_t *)pmdp;
#endif

    ptep = pte_offset_kernel(pmdp, addr);
    if (!ptep)
    {
        return NULL;
    }
    LOG_DEBUG("pte %llx", ptep);
    return ptep;
}

// lower linux version android device(maybe 4.x) not has large pages in userspace
//   memory.c -> follow_pte(mm, addr, &pte, &ptlp);//not handle large page (userspace appear has large pages)
//  fault.c -> show_pte
pte_t *page_from_virt_user(struct mm_struct *mm, unsigned long addr)
{
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pudp, pud;
    pmd_t *pmdp, pmd;
    pte_t *ptep;

    pgd = pgd_offset(mm, addr);
    if (pgd_none(*pgd) || pgd_bad(*pgd))
    {
        return NULL;
    }
    // return if pgd is entry is here

    p4d = p4d_offset(pgd, addr);
    if (p4d_none(*p4d) || p4d_bad(*p4d))
    {
        return 0;
    }

    pudp = pud_offset(p4d, addr);
    pud = READ_ONCE(*pudp);
    if (pud_none(pud) || pud_bad(pud))
    {
        return NULL;
    }

#if defined(pud_leaf) && defined(BIG_PAGE)
    // 处理 PUD 级大页，直接操作 pud_val
    if (pud_leaf(pud))
    {
        ptep = (pte_t *)pudp;
        return ptep;
    }
#endif

    pmdp = pmd_offset(pudp, addr);
    pmd = READ_ONCE(*pmdp);
    if (pmd_none(pmd))
    {
        return NULL;
    }

#if defined(pmd_leaf) && defined(BIG_PAGE)
    if (pmd_leaf(pmd))
    {
        ptep = (pte_t *)pmdp;
        return ptep;
    }
#endif

    ptep = pte_offset_kernel(pmdp, addr);
    if (!ptep)
    {
        return NULL;
    }

    return ptep;
}

// set pte -> readonly
int protect_rodata_memory(unsigned long addr)
{
    pte_t pte;
    pte_t *ptep;

    // ptep = page_from_virt_kernel(addr);
    ptep = get_pte(addr);
    LOG_DEBUG("pte :%llx ", ptep);
    if (ptep == NULL)
    {
        LOG_DEBUG(" failed to get ptep from 0x%lx\n", addr);
        return -2;
    }

    if (!pte_valid(READ_ONCE(*ptep)))
    { // arm64
        LOG_DEBUG(" failed to get ptep from 0x%lx\n", addr);
        return -2;
    }
    pte = *ptep;

    pte = pte_wrprotect(pte);

    set_pte(ptep, pte);

    // flush_icache_range(addr, addr + PAGE_SIZE);
    //__clean_dcache_area_pou(data_addr, sizeof(data));
    __flush_tlb_kernel_pgtable(addr); // arm64
    return 0;
}

// default pte is readonly ,so must can read
// set pte -> write
int unprotect_rodata_memory(unsigned long addr)
{
    pte_t pte;
    pte_t *ptep;

    // ptep = page_from_virt_kernel(addr);
    ptep = get_pte(addr);

    if (ptep == NULL)
    {
        LOG_DEBUG("failed to get ptep from 0x%lx\n", addr);
        return -2;
    }

    LOG_DEBUG("pte :%llx ", ptep);

    if (!pte_valid(READ_ONCE(*ptep)))
    {
        LOG_DEBUG("failed to get ptep from 0x%lx\n", addr);
        return -2;
    }

    pte = READ_ONCE(*ptep);
    pte = pte_mkwrite(pte);

    set_pte(ptep, pte);
 
    __flush_tlb_kernel_pgtable(addr);
    return 0;
}
