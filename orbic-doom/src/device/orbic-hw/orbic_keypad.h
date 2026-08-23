#ifndef KEYPAD_H
#define KEYPAD_H

#include <linux/input.h>

/* KEYCODES */
#define KEY_ORBIC_RESET  9
#define KEY_ORBIC_MENU   10
#define KEY_ORBIC_POWER  110

#define ORBIC_MENU_DEVICE  "/dev/input/event1"
#define ORBIC_POWER_DEVICE "/dev/input/event2"

/*
Initializes the keypad for a specific event path. Most certainly "/dev/event/input0" on most devices.
Returns a file descriptor, used to reference it in other functions, like close or get_pressed
*/
int keypad_init(const char *device); 


int keypad_get_pressed(int fd); // returns the keycode if pressed, otherwise returns 0/-1 if it was an un-press


void keypad_close(int fd); // Properly close the specified fd.

#endif // KEYPAD_H