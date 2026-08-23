#ifndef FB_H
#define FB_H

#include <stdint.h>

#define FB_WIDTH   128 
#define FB_HEIGHT  128 /* Change this to "64" if you're using a 128x64 display. Might or might not work. */

int fb_init(void); // Initializes the framebuffer. Returns -1 if failed
void fb_close(void); // Closes the frambuffer.

void fb_flush(void); // To "push" changes to FB.

void fb_clear(uint16_t color); // Clear the screen with a specific color
void fb_set_pixel(int x, int y, uint16_t color); // Color a specific pixel
void fb_fill_rect(int x, int y, int w, int h, uint16_t color); // Draw a rectangle thats filled
void fb_draw_rect(int x, int y, int w, int h, uint16_t color); // Draw a non-filled rectangle
void fb_draw_line(int x1,int y1, int x2, uint16_t color); // Draw a line from {x1,y1} to {x2,y1}
/* -- Explanation for scale int
scale=1 -> glyph 5x7 px. scale=2 -> 10x14 px, etc. */
void fb_draw_char(int x, int y, char c, uint16_t fg, uint16_t bg, int scale); // Draws a single char.
void fb_draw_text(int x, int y, const char *text, uint16_t fg, uint16_t bg, int scale); // Draw multiple chars

uint16_t fb_rgb(uint8_t r, uint8_t g, uint8_t b); // Very useful helper to convert R,G,B into uint16_t rgb, thats used in every function.

#endif