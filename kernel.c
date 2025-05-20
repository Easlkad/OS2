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

// Simple delay function (busy-wait)


//static uint16_t* video_mem = (uint16_t*)VIDEO_MEMORY;
// Replace your current inb function with these declarations
extern unsigned char inb(unsigned short port);
extern void outb(unsigned short port, unsigned char data);


void main(){
     

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
            cursor_position = 0; // Ekran sonuna gelirse başa dön (veya scroll_screen() çağırılabilir)
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
        print_string("  help - Display this help message\n");
        print_string("  clear - Clear the screen\n");
        print_string("  echo [text] - Display the text\n");
    }
    else if (strcmp(command, "clear") == 0) {
        clear_screen();
    }
    else if (strncmp(command, "echo ", 5) == 0) {
        print_string(command + 5);
        print_string("\n");
    }
    else if (command[0] != '\0') {
        print_string("Unknown command: ");
        print_string(command);
        print_string("\n");
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
