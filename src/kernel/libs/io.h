#ifndef __IO_H__
#define __IO_H__

#include "stdint.h"

#ifdef __cplusplus
extern "C"
{
#endif
    void outb(uint16_t port, uint8_t value);
    uint8_t inb(uint16_t port);

    void outw(uint16_t port, uint16_t value);
    uint16_t inw(uint16_t port);

    void outd(uint16_t port, uint32_t value);
    uint32_t ind(uint16_t port);

    void load_rdi(uint64_t value);
#ifdef __cplusplus
}
#endif

#endif // !__IO_H__
