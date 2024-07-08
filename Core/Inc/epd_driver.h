#ifndef EPD_DRIVER_H
#define EPD_DRIVER_H

#include <stdint.h>

// Initialize the e-ink display
void epd_initialize(void);

// Display a full image on the e-ink display
void epd_display_image(const uint8_t* image);

// Display a single line of text at the specified coordinates
void epd_display_line(uint8_t x, uint8_t y, const char* line);

// Display a text in a TTY-like manner
void epd_tty_demo(const char* text);

// Test function to demonstrate the e-ink display functionality
void epd_test(void);

#endif // EPD_DRIVER_H
