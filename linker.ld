ENTRY(main)
SECTIONS {
    . = 0x1000; // Kodun başlangıç adresi
    .text : {
        *(.text)
    }
    
}