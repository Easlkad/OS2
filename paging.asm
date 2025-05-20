; paging.asm
bits 32

global enable_paging

; void enable_paging(uint32_t* page_directory)
enable_paging:
    push ebp
    mov ebp, esp
    mov eax, [ebp+8]    ; page_directory adresini al
    mov cr3, eax         ; CR3'e yükle
    mov eax, cr0
    or eax, 0x80000000   ; Paging bitini set et
    mov cr0, eax
    pop ebp
    ret