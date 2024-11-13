#ifndef __VIRTUAL_MEMORY
#define __VIRTUAL_MEMORY

#include "physic_memory.h"

#define PAGES_PER_TABLE 1024
#define TABLES_PER_DIRECTORY 1024
#define PAGE_SIZE 4096

#define PD_INDEX(address) ((address) >> 22)
#define PT_INDEX(address) (((address) >> 12) & 0x3ff)
#define PAGE_PHYSIC_ADDRESS(dir_entry) ((*dir_entry) & ~0xfff)
#define SET_ATTRIBUTE(entry, attr) (*entry |= attr)
#define CLEAR_ATTRIBUTE(entry, attr) (*entry &= ~attr)
#define TEST_ATTRIBUTE(entry, attr) (*entru & attr)
#define SET_FRAME(entry, address) (*entry = (*entry & ~0x7ffff000) | address)

typedef uint64_t pt_entry;
typedef uint64_t pd_entry;
typedef uint64_t physical_address;
typedef uint64_t virtual_address;

typedef enum
{
    PTE_PRESENT = 0x01,
    PTE_READ_WRITE = 0x02,
    PTE_USER = 0x04,
    PTE_WRITE_THROUGH = 0x08,
    PTE_CACHE_DISABLE = 0x10,
    PTE_ACCESSED = 0x20,
    PTE_DIRTY = 0x40,
    PTE_PAT = 0x80,
    PTE_GLOBAL = 0x100,
    PTE_FRAME = 0xfffff000
} PAGE_TABLE_FLAGS;

typedef enum
{
    PDE_PRESENT = 0x01,
    PDE_READ_WRITE = 0x02,
    PDE_USER = 0x04,
    PDE_WRITE_THROUGH = 0x08,
    PDE_CACHE_DISABLE = 0x10,
    PDE_ACCESSED = 0x20,
    PDE_DIRTY = 0x40,     // 4MB paging only
    PDE_PAGE_SIZE = 0x80, // 0 = 4KB page, 1 = 4MB page
    PDE_GLOBAL = 0x100,   // 4MB entry only
    PDE_PAT = 0x200,      // 4MB entry only
    PDE_FRAME = 0xfffff000
} PAGE_DIR_FLAGS;

typedef struct
{
    pt_entry entries[PAGES_PER_TABLE];
} page_table;

typedef struct
{
    pd_entry entries[TABLES_PER_DIRECTORY];
} page_directory;

pt_entry *get_pt_entry(page_table *pt, virtual_address address);
pd_entry *get_pd_entry(page_table *pd, virtual_address address);
pt_entry *get_page(const virtual_address address);
void *allocate_page(pt_entry *page);
void free_page(pt_entry *page);
bool map_page(void *phys_address, void *virt_address);
void unmap_page(void *virt_address);
bool initialize_virtual_memory_manager(void);

#endif // !__VIRTUAL_MEMORY