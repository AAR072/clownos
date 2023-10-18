#include <stdio.h>
#include <kernel/tty.h>
#include <kernel/util.h>
#include <string.h>

// Define VGA text mode colors

// 	VGA_COLOR_BLACK = 0,
// 	VGA_COLOR_BLUE = 1,
// 	VGA_COLOR_GREEN = 2,
// 	VGA_COLOR_CYAN = 3,
// 	VGA_COLOR_RED = 4,
// 	VGA_COLOR_MAGENTA = 5,
// 	VGA_COLOR_BROWN = 6,
// 	VGA_COLOR_LIGHT_GREY = 7,
// 	VGA_COLOR_DARK_GREY = 8,
// 	VGA_COLOR_LIGHT_BLUE = 9,
// 	VGA_COLOR_LIGHT_GREEN = 10,
// 	VGA_COLOR_LIGHT_CYAN = 11,
// 	VGA_COLOR_LIGHT_RED = 12,
// 	VGA_COLOR_LIGHT_MAGENTA = 13,
// 	VGA_COLOR_LIGHT_BROWN = 14,
// 	VGA_COLOR_WHITE = 15,

char *input;               // A variable for storing input from the user (currently unused).
char *terminalIcon = "$>"; // Icon or prompt displayed in the terminal.
unsigned char text = 'J';
char *term = "\n"; // Newline character, used to move to the next line.
char *longtext = "sahjdsalhlsaakjhfalkjshflkjdsahflksajdhflksajhflkjsafhsalkfhdsalkjfsadfhakjsdhfsalkjdhf";

int currentColor = 2; // Current text color for the terminal (e.g., VGA_COLOR_LIGHT_GREY).

// The main function for the kernel.
void kernel_main(void)
{
    // Initialize the terminal for text output.
    terminal_initialize();

    // Set the text color for the terminal.
    terminal_setcolor(currentColor);

    terminal_print_at_cursor("Hello World!", currentColor, false);
    terminal_print_at_cursor("H2", currentColor, true);

}