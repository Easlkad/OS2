# Makefile for your OS project

NASM = nasm
GCC = i686-elf-gcc
LD = i686-elf-ld
OBJCOPY = i686-elf-objcopy

NASMFLAGS_BIN = -f bin
NASMFLAGS_ELF = -f elf32
GCCFLAGS = -c -ffreestanding -O2 -Wall -Wextra
#LDFLAGS = -T linker.ld  -e _main
LDFLAGS = -Ttext 0x1000  -e main

BOOTLOADER_SRC = bootloader.asm
BOOTLOADER_BIN = bootloader.bin

IO_SRC = io.asm
IO_OBJ = io.o
PAGING_SRC = paging.asm
PAGING_OBJ = paging.o

KERNEL_SRC = kernel.c
KERNEL_OBJ = kernel.o
KERNEL_ELF = kernel.elf
KERNEL_BIN = kernel.bin
OS_IMAGE = os-image.bin

all: $(OS_IMAGE)

$(BOOTLOADER_BIN): $(BOOTLOADER_SRC)
	$(NASM) $(NASMFLAGS_BIN) $< -o $@

$(IO_OBJ): $(IO_SRC)
	$(NASM) $(NASMFLAGS_ELF) $< -o $@

$(PAGING_OBJ): $(PAGING_SRC)
	$(NASM) $(NASMFLAGS_ELF) $< -o $@

$(KERNEL_OBJ): $(KERNEL_SRC)
	$(GCC) $(GCCFLAGS) $< -o $@

$(KERNEL_ELF): $(KERNEL_OBJ) $(IO_OBJ) $(PAGING_OBJ)
	$(LD) $(LDFLAGS) -o $@ $(KERNEL_OBJ) $(IO_OBJ) $(PAGING_OBJ)

$(KERNEL_BIN): $(KERNEL_ELF)
	$(OBJCOPY) -O binary $< $@

$(OS_IMAGE): $(BOOTLOADER_BIN) $(KERNEL_BIN)
	copy /b $(BOOTLOADER_BIN)+$(KERNEL_BIN) $@

run: $(OS_IMAGE)
	qemu-system-x86_64 -drive format=raw,file=$(OS_IMAGE),if=floppy

clean:
	del $(BOOTLOADER_BIN) $(IO_OBJ) $(KERNEL_OBJ) $(KERNEL_ELF) $(KERNEL_BIN) $(OS_IMAGE) 2>nul || true

.PHONY: all clean
