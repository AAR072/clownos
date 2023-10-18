// Include necessary libraries and headers
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

// Include custom headers
#include <kernel/tty.h>
#include <kernel/util.h>
#include "vga.h"

// Define global variables
int cursorXPos;
int cursorYPos;

// Input/Output functions for port access
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("in %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t data) {
    asm volatile("out %0, %1" :: "a"(data), "Nd"(port));
}

// Constants for VGA display
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;

// Variables for text mode display
static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

// Initialize the terminal display
void terminal_initialize(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    terminal_buffer = VGA_MEMORY;

    // Clear the screen by filling it with spaces
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
}

// Set the text color
void terminal_setcolor(uint8_t color) {
    terminal_color = color;
}

// Write a character to the terminal display
void terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}

// Handle a single character for terminal output
void terminal_putchar(char c) {
    unsigned char uc = c;
    
    if (uc == '\n') {
        // Handle newline character
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT)
            terminal_column = 0;
        return;
    }
    
    // Print the character and advance the cursor
    terminal_putentryat(uc, terminal_color, terminal_column, terminal_row);
    if (++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT)
            terminal_row = 0;
    }
}

// Write a string to the terminal display
void terminal_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++)
        terminal_putchar(data[i]);
}

// Write a null-terminated string to the terminal display
void terminal_writestring(const char* data) {
    terminal_write(data, strlen(data));
}

// Clear the screen with a specified foreground color
void terminal_clear_screen(int fgcolor) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_buffer = VGA_MEMORY;
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', vga_entry_color(fgcolor, VGA_COLOR_BLACK));
        }
    }
}

// Get the current cursor position
uint16_t get_cursor_position(void) {
    uint16_t pos = 0;
    outb(0x3D4, 0x0F);
    pos |= inb(0x3D5);
    outb(0x3D4, 0x0E);
    pos |= ((uint16_t)inb(0x3D5)) << 8;
    return pos;
}

// Disable the text cursor
void disable_cursor() {
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

// Update the text cursor position
void update_cursor(int x, int y) {
    uint16_t pos = y * VGA_WIDTH + x;
 
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t) (pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

// Print text with "$>" at a specified position
void terminal_printat(const char* str, uint8_t color, size_t x, size_t y) {
    size_t index = y * VGA_WIDTH + x;
    size_t str_len = strlen(str);

    // Print "$>"
    terminal_buffer[index] = vga_entry('$', color);
    x++;
    index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry('>', color);
    x++;

    for (size_t i = 0; i < str_len; i++) {
        if (x >= VGA_WIDTH) {
            x = 0;
            y++;
        }

        if (y >= VGA_HEIGHT) {
            y = 0;
        }

        if (str[i] == '\n') {
            // Handle newline character
            x = 0;
            y++;
        } else {
            index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(str[i], color);
            x++;
        }
    }

    // Update the cursor position to the end of the string
    update_cursor(x, y);
}

// Print text at the current cursor position with an optional newline
void terminal_print_at_cursor(const char* str, uint8_t color, _Bool printNewLineFirst) {
    uint16_t posNumber = get_cursor_position();
    size_t y = posNumber / VGA_WIDTH;
    size_t x = posNumber % VGA_WIDTH;

    if (printNewLineFirst) {
        y++;  // Move to a new line before printing "$>"
        x = 0;
    }

    size_t index = y * VGA_WIDTH + x;
    size_t str_len = strlen(str);

    // Print "$>"
    terminal_buffer[index] = vga_entry('$', color);
    x++;
    index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry('>', color);
    x++;

    for (size_t i = 0; i < str_len; i++) {
        if (x >= VGA_WIDTH) {
            x = 0;
            y++;
        }

        if (y >= VGA_HEIGHT) {
            y = 0;
        }

        if (str[i] == '\n') {
            // Handle newline character
            x = 0;
            y++;
        } else {
            index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(str[i], color);
            x++;
        }
    }

    // Update the cursor position to the end of the string
    update_cursor(x, y);
}

void terminal_scroll_down_screen(_Bool scrollCursor) {
    // Loop through each row, except the last one
    uint16_t posNumber = get_cursor_position();
    size_t y = posNumber / VGA_WIDTH;
    size_t x = posNumber % VGA_WIDTH;
    for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            // Copy each character from the row below to the current row
            size_t current_index = y * VGA_WIDTH + x;
            size_t next_row_index = (y + 1) * VGA_WIDTH + x;
            terminal_buffer[current_index] = terminal_buffer[next_row_index];
        }
    }

    // Clear the last row with null characters
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        size_t last_row_index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
        terminal_buffer[last_row_index] = vga_entry(' ', vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    }
    if(scrollCursor){
    update_cursor(x, y-1);
    }
}

