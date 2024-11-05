#include "memory.h"
#include "print.h"
#include "debug.h"
#include "libs/lib.h"

static void free_region(uint64_t v, uint64_t e);

static struct FreeMemRegion free_mem_region[50];
static struct Page free_memory;
static uint64_t memory_end;
extern char end;
uint64_t page_map;

void init_memory(void)
{
    int32_t count = *(int32_t *)0x9000;
    uint64_t total_mem = 0;
    struct E820 *mem_map = (struct E820 *)0x9008;
    int free_region_count = 0;

    ASSERT(count <= 50);

    for (int32_t i = 0; i < count; i++)
    {
        if (mem_map[i].type == 1)
        {
            free_mem_region[free_region_count].address = mem_map[i].address;
            free_mem_region[free_region_count].length = mem_map[i].length;
            total_mem += mem_map[i].length;
            free_region_count++;
        }
        // printk("%x   %uKB   %u\n", mem_map[i].address, mem_map[i].length / 1024, (uint64_t)mem_map[i].type);
    }

    for (int i = 0; i < free_region_count; i++)
    {
        uint64_t vstart = P2V(free_mem_region[i].address);
        uint64_t vend = vstart + free_mem_region[i].length;
        if (vstart > (uint64_t)&end)
        {
            free_region(vstart, vend);
        }
        else if (vend > (uint64_t)&end)
        {
            free_region((uint64_t)&end, vend);
        }
    }
    memory_end = (uint64_t)free_memory.next + PAGE_SIZE;
    printk("Total memory is %uMB\n", total_mem / 1024 / 1024);
    printk("%x\n", memory_end);
}

static void setup_kvm(void)
{
    page_map = (uint64_t)kalloc();
    ASSERT(page_map != 0);

    memset((void *)page_map, 0, PAGE_SIZE);
    bool status = map_pages(page_map, KERNEL_BASE, memory_end, V2P(KERNEL_BASE), PTE_P | PTE_W);
    ASSERT(status == true);
}

void init_kvm(void)
{
    setup_kvm();

    switch_vm(page_map);
    printk("Memory manager is working now");
}

void switch_vm(uint64_t map)
{
    load_cr3(V2P(map));
}

void *kalloc(void)
{
    struct Page *page_address = free_memory.next;
    if (page_address != 0)
    {
        ASSERT((uint64_t)page_address % PAGE_SIZE == 0);
        ASSERT((uint64_t)page_address >= (uint64_t)&end);
        ASSERT((uint64_t)page_address + PAGE_SIZE <= 0xffff800040000000);

        free_memory.next = page_address->next;
    }
    return (void *)page_address;
}

void kfree(uint64_t v)
{
    ASSERT(v % PAGE_SIZE == 0);
    ASSERT(v >= (uint64_t)&end);
    ASSERT(v + PAGE_SIZE <= 0xffff800040000000);

    struct Page *page_address = (struct Page *)v;
    page_address->next = free_memory.next;
    free_memory.next = page_address;
}

static PDPTR find_pml4t_entry(uint64_t map, uint64_t v, int alloc, uint32_t attribute)
{
    PDPTR *map_entry = (PDPTR *)map;
    PDPTR pdptr = NULL;
    unsigned int index = (v >> 39) & 0x1ff;
    if ((uint64_t)map_entry[index] & PTE_P)
        pdptr = (PDPTR)P2V(PDE_ADDR(map_entry[index]));
    else if (alloc == 1)
    {
        pdptr = (PDPTR)kalloc();
        if (pdptr != NULL)
        {
            memset(pdptr, 0, PAGE_SIZE);
            map_entry[index] = (PDPTR)(V2P(pdptr) | attribute);
        }
    }
    return pdptr;
}

static PD find_pdpt_entry(uint64_t map, uint64_t v, int alloc, uint32_t attribute)
{
    PDPTR pdptr = NULL;
    PD pd = NULL;
    unsigned int index = (v >> 30) & 0x1ff;

    pdptr = find_pml4t_entry(map, v, alloc, attribute);
    if (pdptr == NULL)
        return NULL;

    if ((uint64_t)pdptr[index] & PTE_P)
        pd = (PD)P2V(PDE_ADDR(pdptr[index]));
    else if (alloc = 1)
    {
        pd = (PD)kalloc();
        if (pd != NULL)
        {
            memset(pd, 0, PAGE_SIZE);
            pdptr[index] = (PD)(V2P(pd) | attribute);
        }
    }
    return pd;
}

bool map_pages(uint64_t map, uint64_t v, uint64_t e, uint64_t pa, uint32_t attribute)
{
    uint64_t vstart = PA_DOWN(v);
    uint64_t vend = PA_UP(e);
    PD pd = NULL;
    unsigned int index;
    ASSERT(v < e);
    ASSERT(pa % PAGE_SIZE == 0);
    ASSERT(pa + vend - vstart < 1024 * 1024 * 1024);
    do
    {
        pd = find_pdpt_entry(map, vstart, 1, attribute);
        if (pd == NULL)
            return false;

        index = (vstart >> 21) & 0x1ff;
        ASSERT(((uint64_t)pd[index] & PTE_P) == 0);

        pd[index] = (PDE)(pa | attribute | PTE_ENTRY);
        printk("Mapping VA %x to PA %x\n", vstart, pa); // Debug output

        vstart += PAGE_SIZE;
        pa += PAGE_SIZE;
    } while (vstart + PAGE_SIZE <= vend);
    return true;
}

static void free_region(uint64_t v, uint64_t e)
{
    for (uint64_t start = PA_UP(v); start + PAGE_SIZE <= e; start += PAGE_SIZE)
    {
        if (v + PAGE_SIZE <= 0xffff800040000000)
            kfree(start);
    }
}