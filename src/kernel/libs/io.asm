section .text
global outb
global inb
global outw
global inw
global outd
global ind
global load_rdi

outb:
    mov dx, di
    mov al, sil
    out dx, al
    ret

inb:
    xor rax, rax
    mov dx, di
    in  al, dx
    ret

outw:
    mov dx, di
    mov ax, si
    out dx, ax
    ret

inw:
    xor rax, rax
    mov dx, di
    in  ax, dx
    ret

outd:
    mov dx, di
    mov eax, esi
    out dx, eax
    ret

ind:
    mov dx, di
    xor rax, rax
    in  eax, dx
    ret

load_rdi:
    ret
