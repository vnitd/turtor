#include "virtual_memory.h"
#include "../libs/lib.h"

page_directory *current_page_directory = 0;
physical_address current_pd_address = 0;

pt_entry *get_pt_entry(page_table *pt, virtual_address address)
{
    if (pt)
        return &pt->entries[PT_INDEX(address)];
    return 0;
}

pd_entry *get_pd_entry(page_table *pd, virtual_address address)
{
    if (pd)
        return &pd->entries[PD_INDEX(address)];
    return 0;
}

pt_entry *get_page(const virtual_address address)
{
    // get page directory
    page_directory *pd = current_page_directory;
    // get page table in directory
    pd_entry *entry = &pd->entries[PD_INDEX(address)];
    page_table *table = (page_table *)PAGE_PHYSIC_ADDRESS(entry);

    // get page in table
    pt_entry *page = &table->entries[PT_INDEX(address)];

    // return page
    return page;
}

void *allocate_page(pt_entry *page)
{
    void *block = allocate_blocks(1);
    if (block)
    {
        // map page to block
        SET_FRAME(page, (physical_address)block);
        SET_ATTRIBUTE(page, PTE_PRESENT);
    }
    return block;
}

void free_page(pt_entry *page)
{
    void *address = (void *)PAGE_PHYSIC_ADDRESS(page);
    if (address)
        free_blocks((uint64_t)address, 1);
    CLEAR_ATTRIBUTE(page, PTE_PRESENT);
}

extern "C" void load_cr3(void *);
extern "C" void load_cr0(uint64_t);
bool set_page_directory(page_directory *pd)
{
    if (!pd)
        return false;
    current_page_directory = pd;
    // CR3 (control register 3) holds address of the current page directory
    load_cr3(current_page_directory);
    return true;
}

void flush_tlb_entry(virtual_address address)
{
    __asm__ __volatile__("cli; invlpg (%0); sti" : : "r"(address));
}

// Map a page
bool map_page(void *phys_address, void *virt_address)
{
    // Get page
    page_directory *pd = current_page_directory;

    // Get page table
    pd_entry *entry = &pd->entries[PD_INDEX((uint64_t)virt_address)];

    // TODO: Use TEST_ATTRIBUTE for this check?
    if ((*entry & PTE_PRESENT) != PTE_PRESENT)
    {
        // Page table not present allocate it
        page_table *table = (page_table *)allocate_blocks(1);
        if (!table)
            return false; // Out of memory

        // Clear page table
        memset(table, 0, sizeof(page_table));

        // Create new entry
        pd_entry *entry = &pd->entries[PD_INDEX((uint64_t)virt_address)];

        // Map in the table & enable attributes
        SET_ATTRIBUTE(entry, PDE_PRESENT);
        SET_ATTRIBUTE(entry, PDE_READ_WRITE);
        SET_FRAME(entry, (physical_address)table);
    }

    // Get table
    page_table *table = (page_table *)PAGE_PHYSIC_ADDRESS(entry);

    // Get page in table
    pt_entry *page = &table->entries[PT_INDEX((uint64_t)virt_address)];

    // Map in page
    SET_ATTRIBUTE(page, PTE_PRESENT);
    SET_FRAME(page, (uint64_t)phys_address);

    return true;
}

// Unmap a page
void unmap_page(void *virt_address)
{
    pt_entry *page = get_page((uint64_t)virt_address);

    SET_FRAME(page, 0);                 // Set physical address to 0 (effectively this is now a null pointer)
    CLEAR_ATTRIBUTE(page, PTE_PRESENT); // Set as not present, will trigger a #PF
}

bool initialize_virtual_memory_manager(void)
{
    page_directory *dir = (page_directory *)allocate_blocks(3);

    if (!dir)
        return false; // Out of memory

    // Clear page directory and set as current
    memset(dir, 0, sizeof(page_directory));
    for (uint64_t i = 0; i < 1024; i++)
        dir->entries[i] = 0x02; // Supervisor, read/write, not present

    // Allocate page table for 0-4MB
    page_table *table = (page_table *)allocate_blocks(1);

    if (!table)
        return false; // Out of memory

    // Allocate a 3GB page table
    page_table *table3G = (page_table *)allocate_blocks(1);

    if (!table3G)
        return false; // Out of memory

    // Clear page tables
    memset(table, 0, sizeof(page_table));
    memset(table3G, 0, sizeof(page_table));

    // Identity map 1st 4MB of memory
    for (uint64_t i = 0, frame = 0x0, virt = 0x0; i < 1024; i++, frame += PAGE_SIZE, virt += PAGE_SIZE)
    {
        // Create new page
        pt_entry page = 0;
        SET_ATTRIBUTE(&page, PTE_PRESENT);
        SET_ATTRIBUTE(&page, PTE_READ_WRITE);
        SET_FRAME(&page, frame);

        // Add page to 3GB page table
        table3G->entries[PT_INDEX(virt)] = page;
    }

    // Map kernel to 3GB+ addresses (higher half kernel)
    for (uint64_t i = 0, frame = 0x200000, virt = 0xC0000000; i < 1024; i++, frame += PAGE_SIZE, virt += PAGE_SIZE)
    {
        // Create new page
        pt_entry page = 0;
        SET_ATTRIBUTE(&page, PTE_PRESENT);
        SET_ATTRIBUTE(&page, PTE_READ_WRITE);
        SET_FRAME(&page, frame);

        // Add page to 0-4MB page table
        table->entries[PT_INDEX(virt)] = page;
    }

    pd_entry *entry = &dir->entries[PD_INDEX(0xC0000000)];
    SET_ATTRIBUTE(entry, PDE_PRESENT);
    SET_ATTRIBUTE(entry, PDE_READ_WRITE);
    SET_FRAME(entry, (physical_address)table); // 3GB directory entry points to default page table

    pd_entry *entry2 = &dir->entries[PD_INDEX(0x00000000)];
    SET_ATTRIBUTE(entry2, PDE_PRESENT);
    SET_ATTRIBUTE(entry2, PDE_READ_WRITE);
    SET_FRAME(entry2, (physical_address)table3G); // Default dir entry points to 3GB page table

    // Switch to page directory
    set_page_directory(dir);

    // Enable paging: Set PG (paging) bit 31 and PE (protection enable) bit 0 of CR0
    load_cr0(0xffff800000000000);

    return true;
}