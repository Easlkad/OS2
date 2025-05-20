[bits 16]
[org 0x7C00]

start:
    xor ax,ax
    mov ds,ax
    mov es,ax 
    mov ss,ax 
    mov sp,[0x7C00]
    cli
    
    ; Initial debug message
    mov si, boot_message
    call print_string
    
    ; Debug message before disk read
    mov si, reading_disk_msg
    call print_string

    ; Load kernel sectors first while in real mode
    mov ah, 0x02      ; BIOS read sector function
    mov al, 30        ; Number of sectors to read (increased from 5)
    mov ch, 0         ; Cylinder 0
    mov cl, 2         ; Start from sector 2
    mov dh, 0         ; Head 0
    mov dl, 0x00      ; Drive (try 0x00 for floppy in QEMU)
    mov bx, 0x1000    ; Load to 0x1000 memory address
    int 0x13          ; BIOS interrupt
    jc disk_error     ; Jump if error (carry flag set)
    
    ; Debug message after successful disk read
    mov si, disk_success_msg
    call print_string
    
    ; Debug message before protected mode switch
    mov si, switching_mode_msg
    call print_string

    ; Switch to protected mode
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    
    jmp CODE_SEGMENT:protected_mode_entry ; far jump to flush the pipeline

disk_error:
    mov si, disk_error_msg
    call print_string
    jmp $             ; Hang on error

print_string:
    mov ah, 0x0E
.print_char:
    lodsb
    cmp al, 0
    je .done
    int 0x10
    jmp .print_char
.done:
    ret

boot_message db 'Started Real mode', 0x0D, 0x0A, 0
reading_disk_msg db 'Reading kernel from disk...  ', 0x0D, 0x0A, 0
disk_success_msg db 'Kernel loaded successfully!', 0x0D, 0x0A, 0
switching_mode_msg db 'Switching to protected mode...', 0x0D, 0x0A, 0
disk_error_msg db 'ERROR: Failed to read disk!', 0x0D, 0x0A, 0

gdt_start:
    dd 0x0
    dd 0x0

gdt_code:
    dw 0xFFFF
    dw 0x0
    db 0x0
    ;db 10011010b
    db 0x9A
    db 11001111b
    db 0x0

gdt_data:
    dw 0xFFFF
    dw 0x0
    db 0x0
    ;db 10010010b
    db 0x92
    db 11001111b
    db 0x0

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEGMENT equ gdt_code - gdt_start  
DATA_SEGMENT equ gdt_data - gdt_start   

[bits 32]
protected_mode_entry:
    ; Set up segment registers
    mov ax, DATA_SEGMENT
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax  

    ; Initialize stack pointer
    mov esp, 0x90000
    
    ; Display success message
    mov esi, msg2
    call print_string_pm
    
    ; Jump to kernel entry point (start function)
    cli
    jmp CODE_SEGMENT:0x1000

print_char_pm:
    mov ebx, 0xB8000
    mov [ebx + edi*2], ax
    ret
    
print_string_pm:
    xor edi, edi
.next_char_pm:
    lodsb
    test al, al
    jz .done_pm
    mov ah, 0x07
    call print_char_pm
    inc edi
    jmp .next_char_pm
.done_pm:
    ret

msg2 db "Protected Mode Active! Jumping to kernel...", 0

times 510 - ($ - $$) db 0
dw 0xAA55