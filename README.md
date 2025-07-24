# Simple Operating System (OS2)

A basic 32-bit operating system written in C and Assembly, featuring a custom bootloader, memory management with paging, and a simple shell interface with file system operations.

## 🚀 Features

- **Custom Bootloader**: 16-bit real mode bootloader that transitions to 32-bit protected mode
- **Memory Management**: Virtual memory support with paging implementation
- **Interactive Shell**: Command-line interface with various built-in commands
- **File System**: Simple in-memory file system with basic CRUD operations
- **Keyboard Input**: Real-time keyboard input handling with scancode to ASCII conversion
- **Memory Allocation**: Basic heap management with kmalloc implementation

## 📁 Project Structure

```
OS2/
├── bootloader.asm      # 16-bit bootloader (real mode → protected mode)
├── kernel.c           # Main kernel code with shell and file system
├── paging.asm         # Memory paging implementation
├── io.asm            # Low-level I/O operations (inb/outb)
├── linker.ld         # Linker script for kernel
├── Makefile          # Build configuration
└── Binary files:
    ├── bootloader.bin
    ├── kernel.bin
    ├── os-image.bin   # Final bootable OS image
    └── *.o, *.elf     # Object and executable files
```

## 🛠️ Build Requirements

### Toolchain
- **NASM** - Netwide Assembler for assembly code
- **i686-elf-gcc** - Cross-compiler for 32-bit x86
- **i686-elf-ld** - Cross-linker
- **i686-elf-objcopy** - Object file converter

### Emulator
- **QEMU** - For testing and running the OS

### Installation (Windows)
1. Install NASM from [nasm.us](https://www.nasm.us/)
2. Install cross-compilation tools:
   - Download pre-built i686-elf toolchain or build from source
   - Add tools to your PATH environment variable
3. Install QEMU from [qemu.org](https://www.qemu.org/)

## 🔧 Building the OS

```bash
# Build all components
make all

# Or build individual components
make bootloader.bin
make kernel.bin
make os-image.bin
```

## 🚀 Running the OS

```bash
# Run in QEMU emulator
make run

# Or manually:
qemu-system-x86_64 -drive format=raw,file=os-image.bin,if=floppy
```

## 🖥️ Shell Commands

Once the OS boots, you'll see a simple shell interface. Available commands:

| Command | Description | Usage |
|---------|-------------|-------|
| `help` | Display available commands | `help` |
| `clear` | Clear the screen | `clear` |
| `echo` | Display text | `echo Hello World` |
| `malloc` | Allocate memory | `malloc 1024` |
| `touch` | Create a new file | `touch filename.txt` |
| `write` | Write data to a file | `write filename.txt Hello World` |
| `cat` | Display file contents | `cat filename.txt` |
| `rm` | Delete a file | `rm filename.txt` |
| `ls` | List all files | `ls` |

## 🏗️ Architecture Overview

### Boot Process
1. **BIOS** loads the bootloader from sector 0
2. **Bootloader** (`bootloader.asm`):
   - Initializes in 16-bit real mode
   - Loads kernel from disk sectors 2-31
   - Sets up Global Descriptor Table (GDT)
   - Transitions to 32-bit protected mode
   - Jumps to kernel entry point

### Kernel Components
1. **Memory Management**:
   - Page directory and page tables setup
   - Identity mapping for first 4MB
   - Virtual memory with 4KB pages

2. **I/O Operations**:
   - Port-based input/output functions
   - Keyboard input handling
   - VGA text mode display

3. **File System**:
   - In-memory file table (16 files max)
   - Files up to 256 bytes each
   - Basic CRUD operations

4. **Shell Interface**:
   - Command parsing and execution
   - Line editing with backspace support
   - Screen scrolling

## 🔍 Technical Details

### Memory Layout
- **0x7C00**: Bootloader load address
- **0x1000**: Kernel load address
- **0x100000**: Heap start (1MB)
- **0xB8000**: VGA text mode buffer

### Paging Configuration
- **Page Size**: 4KB (4096 bytes)
- **Page Tables**: 1024 entries each
- **Identity Mapping**: First 4MB of physical memory
- **Protection**: Present, Read/Write flags

### File System Limitations
- Maximum 16 files
- Maximum filename length: 16 characters
- Maximum file size: 256 bytes
- Files stored in kernel memory (non-persistent)

## 🐛 Debugging

### Common Issues
1. **Boot failure**: Check if QEMU is using the correct drive format
2. **Kernel not loading**: Verify disk read parameters in bootloader
3. **Protection faults**: Check GDT setup and segment registers

### Debug Messages
The bootloader includes debug messages:
- "Started Real mode"
- "Reading kernel from disk..."
- "Kernel loaded successfully!"
- "Switching to protected mode..."
- "Protected Mode Active! Jumping to kernel..."

## 🚧 Limitations & Future Improvements

### Current Limitations
- No persistent storage (files lost on reboot)
- Limited heap size (64KB)
- No multitasking/process management
- Basic interrupt handling
- No networking support

### Potential Enhancements
- [ ] Persistent file system (disk-based)
- [ ] Process scheduling and multitasking
- [ ] Advanced memory management
- [ ] Hardware interrupt handling
- [ ] Network stack implementation
- [ ] GUI interface
- [ ] Device drivers (USB, sound, etc.)

## 📚 Learning Resources

This project demonstrates fundamental OS concepts:
- **Bootloaders and BIOS interaction**
- **x86 assembly programming**
- **Memory management and virtual memory**
- **Low-level I/O operations**
- **Basic file system implementation**
- **Cross-compilation toolchains**

## 🤝 Contributing

Feel free to fork this project and submit improvements! Areas that need work:
- Better error handling
- More robust file system
- Additional shell commands
- Code documentation and comments

## 📄 License

This project is for educational purposes. Feel free to use and modify as needed.

---

**Note**: This is a learning project demonstrating basic OS development concepts. It's not intended for production use.
