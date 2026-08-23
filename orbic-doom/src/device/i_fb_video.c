#include "i_video.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <stdint.h>
#include "v_video.h"
#include "orbic-hw/orbic_fb.h"

static int fbfd = 0;
static struct fb_var_screeninfo vinfo;
static struct fb_fix_screeninfo finfo;
static long int screensize = 0;
static char *fbp = 0;
static uint8_t* gameScreen;


void I_InitGraphics (void)
{
    /*
    fbfd = open("/dev/graphics/fb0", O_RDWR); // Path for modem is /dev/graphics/fb0, not directly /dev/fb0
    if (!fbfd) {
            printf("Error: cannot open framebuffer device.\n");
            exit(1);
    }
    printf("The framebuffer device was opened successfully.\n");

    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
        printf("Error reading fixed information.\n");
            exit(2);
    }

        if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
                printf("Error reading variable information.\n");
                exit(3);
        }

    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8; // On E5577-321s, should be 128x64x?
    printf("Screen size is %d\n",screensize);
    printf("Vinfo.bpp = %d\n",vinfo.bits_per_pixel);

    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,fbfd, 0);
    if ((int64_t)fbp == -1) {
            printf("Error: failed to map framebuffer device to memory.\n");
            exit(4);
    }
    printf("The framebuffer device was mapped to memory successfully.\n");
        */
       
    fb_init(); // ha
}


void I_ShutdownGraphics(void)
{
    fb_close();
}

void I_StartFrame (void)
{

}
__attribute__((packed))
struct Color
{
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;
};

union ColorInt
{
    struct Color col;
    uint32_t raw;
};

static union ColorInt colors[256];

// Takes full 8 bit values.
void I_SetPalette (byte* palette)
{
    byte c;
    // set the X colormap entries
    for (int i=0 ; i<256 ; i++)
    {
        c = gammatable[usegamma][*palette++];
        colors[i].col.r = (c<<8) + c;
        c = gammatable[usegamma][*palette++];
        colors[i].col.g = (c<<8) + c;
        c = gammatable[usegamma][*palette++];
        colors[i].col.b = (c<<8) + c;
    }
}

void I_UpdateNoBlit (void)
{

}
int location(int x, int y)
{
    return (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) * finfo.line_length;
}
uint16_t colorTo16bit(struct Color col)
{
    return fb_rgb(col.r, col.g, col.b);
}

void I_FinishUpdate (void)
{ 
    // Adapted to use orbic-hw lib
    for (int gy = 0; gy < 128; gy++)
    {
        /* Patched to scale things */
        int doom_y = (gy * 200) / 128;

        for (int gx = 0; gx < 128; gx++)
        {
            int doom_x = (gx * 320) / 128;

            byte doom_pixel = *(screens[0] + doom_y * 320 + doom_x);

            uint16_t color16 = colorTo16bit(colors[doom_pixel].col);

            //lets use my lib ha
            fb_set_pixel(gx, gy, color16);
        }
    }

    fb_flush();
}

void I_ReadScreen (byte* scr)
{
    memcpy(scr, screens[0], SCREENWIDTH*SCREENHEIGHT);
}