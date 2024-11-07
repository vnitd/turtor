#include "physic_memory.h"
#include <debug.h>
#include <print.h>
#include <libs/lib.h>

uint64_t memory_end;
static uint64_t *memory_map = 0;
static uint64_t max_blocks = 0;
static uint64_t used_blocks = 0;

void print_physic_memory()
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
    memory_map[bit / 64] |= (1 << (bit % 32));
}

void unset_block(uint32_t bit)
{
    memory_map[bit / 64] &= ~(1 << (bit % 32));
}

uint8_t test_block(uint32_t bit)
{
    return memory_map[bit / 64] & (1 << (bit % 32));
}

int32_t find_first_free_blocks(uint32_t num_blocks)
{
    if (num_blocks == 0)
        return -1;

    for (uint32_t i = 0; i < max_blocks / 64; i++)
    {
        if (memory_map[i] != 0xffffffff)
        {
            for (int32_t j = 0; j < 32; j++)
            {
                int32_t bit = 1 << j;
                if (!(memory_map[i] & bit))
                {
                    int32_t start_bit = i * 32 + bit;
                    uint32_t free_blocks = 0;
                    for (uint32_t count = 0; count <= num_blocks; count++)
                    {
                        if (!test_block(start_bit + count))
                            free_blocks++;
                        if (free_blocks == num_blocks)
                            return i * 32 + j;
                    }
                }
            }
        }
    }
    return -1;
}

void initalize_memory_manager(uint64_t size, uint64_t start_address)
{
    memory_map = (uint64_t *)start_address;
    max_blocks = size / BLOCK_SIZE;
    used_blocks = max_blocks;

    memset(memory_map, 0xff, max_blocks / BLOCKS_PER_BYTE);
}

void initialize_memory_region(uint64_t base_address, uint64_t size)
{
    int64_t align = base_address / BLOCK_SIZE;
    int64_t num_blocks = size / BLOCK_SIZE;

    for (; num_blocks > 0; num_blocks--)
    {
        unset_block(align++);
        used_blocks--;
    }
    set_block(0);
}

void deinitialize_memory_region(uint64_t base_address, uint64_t size)
{
    uint32_t align = base_address / BLOCK_SIZE;
    uint32_t num_blocks = size / BLOCK_SIZE;
    for (; num_blocks > 0; num_blocks--)
    {
        set_block(align++);
        used_blocks--;
    }
}

uint32_t *allocate_blocks(uint32_t num_blocks)
{
    if ((max_blocks - used_blocks) <= num_blocks)
        return 0;

    int32_t starting_block = find_first_free_blocks(num_blocks);
    if (starting_block == -1)
        return 0;

    for (uint32_t i = 0; i < num_blocks; i++)
        set_block(starting_block + i);

    used_blocks += num_blocks;

    uint32_t address = starting_block * BLOCK_SIZE;

    return (uint32_t *)address;
}

void free_blocks(uint32_t *address, uint32_t num_blocks)
{
    uint64_t starting_blocks = (uint64_t)address / BLOCK_SIZE; // Convert address to blocks
    for (uint64_t i = 0; i < num_blocks; i++)
        unset_block(starting_blocks + i); // Unset bits/blocks in memory map, to free
    used_blocks -= num_blocks;            // Decrease used block count
}