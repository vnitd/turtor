#include "vbe.h"
#include <libs/io.h>
#include <print.h>
#include <debug.h>
#include <memory.h>

vbe_info *info;
vbe_mode_info *mode_info;

extern uint8_t init_vbe(vbe_info *info);

void init_graphics(void)
{
    info = (vbe_info *)P2V(0x8200);
    mode_info = (vbe_mode_info *)P2V(0x8400);
    printk("\nSIGNATURE: %x", mode_info->framebuffer);

    // uint32_t *buffer = (uint32_t *)P2V(mode_info->framebuffer);
    // for (int i = 0; i < 1920 * 1080; i++)
    //     buffer[i] = 0x00ffffff;
}
