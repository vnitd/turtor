#include "physic_memory.h"
#include <debug.h>
#include <print.h>

uint64_t memory_end;
static uint32_t *memory_map = 0;
static uint32_t max_blocks = 0;
static uint32_t used_blocks = 0;

void init_physic_memory()
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
            total_mem += mem_map[i].length;
        }
        printk("%x   %uKB   %u\n", mem_map[i].address, mem_map[i].length / 1024, (uint64_t)mem_map[i].type);
    }

    printk("Total memory is %uMB\n", total_mem / 1024 / 1024);
}

void set_block(uint32_t bit)
{
    memory_map[bit / 32] |= (1 << (bit % 32));
}

void unset_block(uint32_t bit)
{
    memory_map[bit / 32] &= ~(1 << (bit % 32));
}

uint8_t test_block(uint32_t bit)
{
    return memory_map[bit / 32] & (1 << (bit % 32));
}
