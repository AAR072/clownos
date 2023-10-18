#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
extern int cursorXPos;
extern int cursorYPos;
void terminal_initialize(void);
void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
void terminal_clear_screen(int fgcolor);
uint16_t get_cursor_position(void);
void update_cursor(int x, int y);
void disable_cursor();
void terminal_printat(const char* str, uint8_t color, size_t x, size_t y);
void terminal_print_at_cursor(const char* str, uint8_t color, _Bool printNewLineFirst);
#endif
