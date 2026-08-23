/**
 * orbic-hw Lib (OLED Display code)
 * By: @FrenchPythonLover
 * Remade for Orbic: @dc336
 */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>

#include "orbic_fb.h"
#include "font5x7.h"

#define FB_DEVICE "/dev/fb0"
#define FB_STRIDE 256

static int fb_fd = -1;
static uint16_t *fb_mem = NULL;
static size_t fb_map_size = 0;
static struct fb_var_screeninfo fb_vinfo;

int fb_init(void) {
    /* Initializes the frambebuffer */
    fb_fd = open(FB_DEVICE, O_RDWR);
    if (fb_fd < 0) {
        perror("fb_init: open");
        return -1;
    }

    if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &fb_vinfo) < 0) {
        perror("fb_init: FBIOGET_VSCREENINFO");
        close(fb_fd);
        fb_fd = -1;
        return -1;
    }

    fb_map_size = (size_t)FB_STRIDE * fb_vinfo.yres_virtual;

    fb_mem = mmap(NULL, fb_map_size, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
    if (fb_mem == MAP_FAILED) {
        perror("fb_init: mmap");
        close(fb_fd);
        fb_fd = -1;
        fb_mem = NULL;
        return -1;
    }

    fb_vinfo.yoffset = 0;


    return 0;
}

void fb_close(void) {
    /* Close properly the framebufffer (i never do it tho) */
    if (fb_mem) {
        munmap(fb_mem, fb_map_size);
        fb_mem = NULL;
    }
    if (fb_fd >= 0) {
        close(fb_fd);
        fb_fd = -1;
    }
}

 void fb_flush(void) { /* The Orbic framebuffer is directly mmap()ed; no page flip is supported. */ }

uint16_t fb_rgb(uint8_t r, uint8_t g, uint8_t b) {
    /* Helper to do conversion between 8bit & 565 */
    return (uint16_t)(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}

void fb_set_pixel(int x, int y, uint16_t color) {
    /* Draw a singular pixel */
    if (!fb_mem) return;
    if (x < 0 || x >= FB_WIDTH || y < 0 || y >= FB_HEIGHT) return;
    fb_mem[y * FB_WIDTH + x] = color;
}

void fb_clear(uint16_t color) {
    /* Draw non filled rectangle */
    if (!fb_mem) return;
    for (int i = 0; i < FB_WIDTH * FB_HEIGHT; i++) {
        fb_mem[i] = color;
    }
}

void fb_fill_rect(int x, int y, int w, int h, uint16_t color) {
    /* Fill rectangle */
    for (int yy = y; yy < y + h; yy++) {
        for (int xx = x; xx < x + w; xx++) {
            fb_set_pixel(xx, yy, color);
        }
    }
}

void fb_draw_rect(int x, int y, int w, int h, uint16_t color) {
    /* Draw rectangle but not fill it */
    for (int xx = x; xx < x + w; xx++) {
        fb_set_pixel(xx, y, color);
        fb_set_pixel(xx, y + h - 1, color);
    }
    for (int yy = y; yy < y + h; yy++) {
        fb_set_pixel(x, yy, color);
        fb_set_pixel(x + w - 1, yy, color);
    }
}

void fb_draw_line(int x1,int y1, int x2, uint16_t color) {
    /* Draw a line across the screen */
    for (int i = 0; i < x2-x1; i++) {
        fb_set_pixel(x1+i, y1,color);
    }
}

void fb_draw_char(int x, int y, char c, uint16_t fg, uint16_t bg, int scale) {
    const uint8_t *glyph = font_get_glyph(c);
    if (scale < 1) scale = 1;

    for (int row = 0; row < FONT_HEIGHT; row++) {
        uint8_t bits = glyph[row];
        for (int col = 0; col < FONT_WIDTH; col++) {
            uint16_t color = (bits & (1 << col)) ? fg : bg;
            if (scale == 1) {
                fb_set_pixel(x + col, y + row, color);
            } else {
                fb_fill_rect(x + col * scale, y + row * scale, scale, scale, color);
            }
        }
    }
}

void fb_draw_text(int x, int y, const char *text, uint16_t fg, uint16_t bg, int scale) {
    if (scale < 1) scale = 1;
    int cx = x;
    for (const char *p = text; *p; p++) {
        if (*p == '\n') {
            cx = x;
            y += (FONT_HEIGHT + 1) * scale;
            continue;
        }
        fb_draw_char(cx, y, *p, fg, bg, scale);
        cx += (FONT_WIDTH + 1) * scale;
    }
}