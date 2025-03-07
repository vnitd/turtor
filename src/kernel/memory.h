#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"

struct E820
{
    uint64_t address;
    uint64_t length;
    uint32_t type;
} __attribute__((packed));

struct FreeMemRegion
{
    uint64_t address;
    uint64_t length;
};

struct Page
{
    struct Page *next;
};

typedef uint64_t PDE;
typedef PDE *PD;
typedef PD *PDPTR;

#define PTE_P 1
#define PTE_W 2
#define PTE_U 4
#define PTE_ENTRY 0x80
#define KERNEL_BASE 0xffff800000000000
#define PAGE_SIZE (2 * 1024 * 1024)

#define PA_UP(v) ((((uint64_t)(v) + PAGE_SIZE - 1) >> 21) << 21)
#define PA_DOWN(v) ((((uint64_t)(v)) >> 21) << 21)
#define P2V(p) ((uint64_t)(p) + KERNEL_BASE)
#define V2P(v) ((uint64_t)(v) - KERNEL_BASE)
#define PDE_ADDR(p) (((uint64_t)p >> 12) << 12)
#define PTE_ADDR(p) (((uint64_t)p >> 21) << 21)

#ifdef __cplusplus
extern "C"
{
#endif
    void init_memory(void);
    void init_kvm(void);
    void switch_vm(uint64_t map);
    void *kalloc(void);
    void kfree(uint64_t v);
    bool map_pages(uint64_t map, uint64_t v, uint64_t e, uint64_t pa, uint32_t attribute);
    void load_cr3(uint64_t map);
#ifdef __cplusplus
}
#endif

#endif // !_MEMORY_H_