#include "stdint.h"
#include "stddef.h"

#include "trap.h"
#include "debug.h"
#include "core/physic_memory.h"
#include "core/virtual_memory.h"

#include "drivers/graphics/vesa.h"
#include "print.h"

extern "C" void KMain(void)
{
    vbe_mode_info *mode_info = (vbe_mode_info *)0x8400;

    init_idt();
    uint64_t total_mem = print_physic_memory();

    initalize_memory_manager(MEMMAP, total_mem);

    uint64_t count = *(uint64_t *)0x9000;
    struct E820 *mem_map = (struct E820 *)0x9008;
    for (uint64_t i = 0; i < count; i++, mem_map++)
        if (mem_map->type == 1)
            initialize_memory_region(mem_map->address, mem_map->length);

    deinitialize_memory_region(0x200000, 0xc800);
    deinitialize_memory_region(MEMMAP, max_blocks / BLOCKS_PER_BYTE);

    initialize_virtual_memory_manager();

    const uint64_t fb_size_in_bytes = (mode_info->height * mode_info->linear_bytes_per_scan_line);
    uint32_t fb_size_in_pages = fb_size_in_bytes / PAGE_SIZE;
    if (fb_size_in_bytes % PAGE_SIZE > 0)
        fb_size_in_pages++;

    fb_size_in_pages *= 2;
    for (uint64_t i = 0, fb_start = mode_info->framebuffer; i < fb_size_in_pages; i++, fb_start += PAGE_SIZE)
        map_page((void *)fb_start, (void *)fb_start);

    deinitialize_memory_region(mode_info->framebuffer, fb_size_in_pages * BLOCK_SIZE);

    uint32_t *buffer = (uint32_t *)(uint64_t)mode_info->framebuffer;
    printk("\n%x", buffer);
    buffer[10] = 0xffffff;
}
