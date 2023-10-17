#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

void terminal_initialize(void);
void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
void terminal_clear_screen(int fgcolor);
uint16_t get_cursor_position(void);
void update_cursor(int x, int y);
void disable_cursor();
#endif
