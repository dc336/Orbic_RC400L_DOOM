/**
 * orbic-hw Lib (Keypad code)
 * By: @FrenchPythonLover
 * Remade for Orbic: @dc336
 */
 
#include "orbic_keypad.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
//Returns: either fd or -1
int keypad_init(const char *device) {
    int fd = open(device, O_RDONLY);
    if (fd == -1) {
        return -1;
    }
    return fd;
}
// returns btn code, -1 or 0 (if its a un-press)
int keypad_get_pressed(int fd) {
    struct input_event ev;
    
    ssize_t n = read(fd, &ev, sizeof(struct input_event));
    
    if (n < (ssize_t)sizeof(struct input_event)) {
        return -1;  
    }
    if (ev.type == EV_KEY) {
        if (ev.value == 1) {
            return ev.code; 
        }
    }
    
    return 0; 
}
// Close.
void keypad_close(int fd) {
    if (fd >= 0) {
        close(fd);
    }
}