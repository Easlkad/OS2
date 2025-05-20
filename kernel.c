#include <stdint.h>
#define VIDEO_MEMORY 0xb8000
#define ROWS 25
#define COLS 80

//keyboard port 
#define KEYBOARD_PORT 0x60

//keyboard interrupt handler
#define SC_ENTER 0x1C
#define SC_BACKSPACE 0x0E

//void print_string(const char* str);
void print_string(const char* str);
void clear_screen();
char read_scancode();
char scancode_to_ASCII(unsigned char scancode);
void read_line(char* buffer, int max_length);
void execute_command(const char* command);
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, int n);
void scroll_screen();
static int cursor_position;
extern unsigned char inb(unsigned short port);
extern void outb(unsigned short port, unsigned char data);
extern void enable_paging(uint32_t * page_directory);


#define KERNEL_HEAP_START 0x100000 // 1MB sonrası (örnek)
#define KERNEL_HEAP_SIZE  0x10000  // 64KB heap (örnek)

static uint32_t heap_ptr = KERNEL_HEAP_START;

void* kmalloc(uint32_t size) {
    void* addr = (void*)heap_ptr;
    heap_ptr += size;
    if (heap_ptr >= (KERNEL_HEAP_START + KERNEL_HEAP_SIZE)) {
        // Heap doldu, NULL döndür
        return 0;
    }
    return addr;
}


#define MAX_FILES 16
#define MAX_FILENAME 16
#define MAX_FILE_SIZE 256

typedef struct{
    char name[MAX_FILENAME];
    char data[MAX_FILE_SIZE];
    int size;
    int used;
} File;
File file_table[MAX_FILES];

#define PAGE_SIZE 4096
#define PAGE_TABLE_ENTRIES 1024
#define PAGE_PRESENT 0x1
#define PAGE_RW      0x2
#define PAGE_USER    0x4

uint32_t* page_directory;
uint32_t* first_page_table;

void init_paging() {
    // Page directory ve ilk page table için bellek ayır
    page_directory = (uint32_t*)kmalloc(PAGE_SIZE);
    first_page_table = (uint32_t*)kmalloc(PAGE_SIZE);

    // Page directory ve page table'ı sıfırla
    for (int i = 0; i < PAGE_TABLE_ENTRIES; i++) {
        page_directory[i] = 0x00000002; // not present, R/W
        first_page_table[i] = (i * PAGE_SIZE) | PAGE_PRESENT | PAGE_RW; // identity map
    }
    // İlk 4MB'ı identity map'le
    page_directory[0] = ((uint32_t)first_page_table) | PAGE_PRESENT | PAGE_RW;

    // Paging'i etkinleştir
    enable_paging(page_directory);
}

void main(){
    init_paging();

    char command_buffer[256];
    
    clear_screen(); // Clear the screen
    
 
    print_string("Simple Shell v0.1\n"); // Added newline for better formatting
    
    
    print_string("Type exit to quit\n"); // Added newline
    print_string("Type help for a list of commands\n"); // Added newlines
    
while (1)
{
    print_string("shell> "); // Prompt for input
    read_line(command_buffer, 255);
    execute_command(command_buffer);
}

       
    
       
    
}

//clear screen
void clear_screen(){
    uint8_t* video_memory = (uint8_t*)VIDEO_MEMORY;
    for(int i = 0; i < ROWS * COLS * 2; i+=2){
        video_memory[i] = ' '; // boşluk karakteri, görünür ve üzerine yazılabilir
        video_memory[i + 1] = 0x07; // light gray on black
    }
    cursor_position = 0; // Reset cursor position
    
    
    //cursor_position = 0; // Reset cursor to the beginning
    
}

void scroll_screen() {
    uint8_t* video_memory = (uint8_t*)VIDEO_MEMORY;
    // Bir satır yukarı kaydır
    for (int row = 1; row < ROWS; row++) {
        for (int col = 0; col < COLS * 2; col++) {
            video_memory[(row - 1) * COLS * 2 + col] = video_memory[row * COLS * 2 + col];
        }
    }
    // Son satırı temizle
    for (int col = 0; col < COLS * 2; col += 2) {
        video_memory[(ROWS - 1) * COLS * 2 + col] = ' ';
        video_memory[(ROWS - 1) * COLS * 2 + col + 1] = 0x07;
    }
    cursor_position = (ROWS - 1) * COLS * 2;
}

void print_string(const char* str){
    uint8_t *video_memory = (uint8_t*)VIDEO_MEMORY;
    while(*str != '\0'){
        if(*str == '\n'){
            // Satır sonuna git
            cursor_position += (COLS * 2) - (cursor_position % (COLS * 2));
        } else {
            video_memory[cursor_position] = *str;
            video_memory[cursor_position + 1] = 0x02;
            cursor_position += 2;
        }
        // Satır sonunu geçerse bir alt satıra in
        if(cursor_position >= ROWS * COLS * 2){
            scroll_screen();
        }
        str++;
    }
}


// Implement scroll_screen function


char read_scancode(){
    char c;
    while(1){
        if((inb(0x64)& 1) != 0){
            c = inb(KEYBOARD_PORT);
            if(c>0){
                return c;
            }
        }
    }
}

char scancode_to_ascii(unsigned char scancode) {
    const char scancode_to_ascii[] = {
        0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,
        0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0,
        0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
        0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
        '*', 0, ' '
    };
    
    if (scancode < sizeof(scancode_to_ascii)) {
        return scancode_to_ascii[scancode];
    }
    return 0;
}

// Read a line of input
void read_line(char* buffer, int max_length) {
    int i = 0;
    char scancode, ascii;
    
    while (i < max_length - 1) {
        scancode = read_scancode();
        
        // Handle key release (ignore it)
        if (scancode & 0x80) {
            continue;
        }
        
        // Enter key - finish input
        if (scancode == SC_ENTER) {
            print_string("\n");
            break;
        }
        
        // Backspace key
        if (scancode == SC_BACKSPACE && i > 0) {
            i--;
            cursor_position -=2;
            print_string(" "); // Erase the last character
             // Go back, erase, go back again
            cursor_position -=2;
            continue;
        }
        
        // Convert scancode to ASCII and add to buffer if it's printable
        ascii = scancode_to_ascii(scancode);
        if (ascii) {
            buffer[i++] = ascii;
            
            // Echo the character
            char echo[2] = {ascii, 0};
            print_string(echo);
        }
    }
    
    buffer[i] = '\0'; // Null-terminate the string
}

// Execute a command
void execute_command(const char* command) {
    if (strcmp(command, "help") == 0) {
        print_string("Available commands:\n");
        print_string("-help - Display this help message\n");
        print_string("-clear - Clear the screen\n");
        print_string("-echo [text] - Display the text\n");
        print_string("-malloc [size] - Allocate memory\n");
        print_string("-touch [filename] - Create a file\n");
        print_string("-write [filename] [data] - Write data to a file\n");
        print_string("-cat [filename] - Read a file\n");
        print_string("-rm [filename] - Delete a file\n");
        print_string("-ls - List files\n");
    }
    else if (strcmp(command, "clear") == 0) {
        clear_screen();
    }
    else if (strncmp(command, "echo ", 5) == 0) {
        print_string(command + 5);
        print_string("\n");
    }
    else if (strncmp(command, "malloc ", 7) == 0) {
        // Kullanıcıdan boyut al
        int size = 0;
        const char* numstr = command + 7;
        while (*numstr >= '0' && *numstr <= '9') {
            size = size * 10 + (*numstr - '0');
            numstr++;
        }
        void* ptr = kmalloc(size);
        if (ptr) {
            print_string("malloc OK\n");
        } else {
            print_string("malloc FAIL\n");
        }
    }
    else if (strncmp(command, "touch ", 6) == 0) {
        if (create_file(command + 6))
            print_string("File created\n");
        else
            print_string("File create FAIL\n");
    }
    else if (strncmp(command, "ls", 2) == 0) {
        list_files();
    }
    else if (strncmp(command, "cat ", 4) == 0) {
        char buf[MAX_FILE_SIZE];
        if (read_file(command + 4, buf) >= 0)
            print_string(buf);
        else
            print_string("File not found\n");
    }
    else if (strncmp(command, "rm ", 3) == 0) {
        if (delete_file(command + 3))
            print_string("File deleted\n");
        else
            print_string("File not found\n");
    }
    else if(strncmp(command, "write ",6) ==0){
        const char* rest = command + 6;
        int i= 0;
        char name[MAX_FILENAME];
        while (rest[i] && rest[i] != ' ' && i < MAX_FILENAME-1) {
        name[i] = rest[i];
        i++;
            }   
        name[i] = '\0';
        if(rest[i] == ' '){
            const char* data = rest + i + 1;
            if(write_file(name, data)){
                print_string("File written\n");
            } else {
                print_string("File write FAIL\n");
            }
        }
    }
    else if (command[0] != '\0') {
        print_string("Unknown command: ");
        print_string(command);
        print_string("\n");
    }
    
}

// Basit dosya sistemi fonksiyonları
int create_file(const char* name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (!file_table[i].used) {
            for (int j = 0; j < MAX_FILENAME - 1 && name[j]; j++)
                file_table[i].name[j] = name[j];
            file_table[i].name[MAX_FILENAME-1] = '\0';
            file_table[i].size = 0;
            file_table[i].used = 1;
            file_table[i].data[0] = '\0';
            return 1;
        }
    }
    return 0;
}

int write_file(const char* name, const char* data) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_table[i].used && strcmp(file_table[i].name, name) == 0) {
            int len = 0;
            while (data[len] && len < MAX_FILE_SIZE-1) {
                file_table[i].data[len] = data[len];
                len++;
            }
            file_table[i].data[len] = '\0';
            file_table[i].size = len;
            return 1;
        }
    }
    return 0;
}

int read_file(const char* name, char* buffer) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_table[i].used && strcmp(file_table[i].name, name) == 0) {
            int len = file_table[i].size;
            for (int j = 0; j < len; j++)
                buffer[j] = file_table[i].data[j];
            buffer[len] = '\0';
            return len;
        }
    }
    return -1;
}

int delete_file(const char* name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_table[i].used && strcmp(file_table[i].name, name) == 0) {
            file_table[i].used = 0;
            return 1;
        }
    }
    return 0;
}

void list_files() {
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_table[i].used) {
            print_string(file_table[i].name);
            print_string("\n");
        }
    }
}

// String comparison
int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

// Check if a string starts with another string
int strncmp(const char* s1, const char* s2, int n) {
    while (n && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) {
        return 0;
    }
    return *s1 - *s2;
}
