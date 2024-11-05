section .text
global init_vbe

init_vbe:
    xor rax, rax
    mov es, ax

    mov dx, 0x1ce
    mov al, 0x4f

    out dx, al
    xor rax, rax
    in  al, dx
    ret
