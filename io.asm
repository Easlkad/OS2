bits 32
global inb
global outb

inb:
    push ebp
    mov ebp,esp
    mov edx,[ebp+8]
    xor eax,eax
    in al,dx

    pop ebp

    ret
outb:
    push ebp
    mov ebp,esp
    mov edx,[ebp+8]
    mov eax,[ebp+12]
    out dx,al

    pop ebp

    ret