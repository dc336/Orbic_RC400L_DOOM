#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>

#include "doomstat.h"
#include "doomdef.h"
#include "m_argv.h"
#include "d_main.h"
#include "m_menu.h"

/* Orbic RC400L input mappings */
#define ORBIC_RESET  9
#define ORBIC_MENU   10
#define ORBIC_POWER  110

static int menu_fd = -1;
static int power_fd = -1;

static int menu_down = 0;
static int power_down = 0;

static int left_sent = 0;
static int right_sent = 0;
static int forward_sent = 0;

static void post_key(int type, int key)
{
    event_t event = {0};

    event.type = type;
    event.data1 = key;

    D_PostEvent(&event);
}

static void update_movement(void)
{
    int want_forward = menu_down && power_down;
    int want_left = menu_down && !power_down;
    int want_right = power_down && !menu_down;

    if (want_left != left_sent) {
        post_key(
            want_left ? ev_keydown : ev_keyup,
            KEY_LEFTARROW
        );
        left_sent = want_left;
    }

    if (want_right != right_sent) {
        post_key(
            want_right ? ev_keydown : ev_keyup,
            KEY_RIGHTARROW
        );
        right_sent = want_right;
    }

    if (want_forward != forward_sent) {
        post_key(
            want_forward ? ev_keydown : ev_keyup,
            KEY_UPARROW
        );
        forward_sent = want_forward;
    }
}

void I_InitInput(void)
{
    menu_fd = open(
        "/dev/input/event1",
        O_RDONLY | O_NONBLOCK
    );

    if (menu_fd < 0) {
        perror("Cannot open menu/reset buttons");
    }

    power_fd = open(
        "/dev/input/event2",
        O_RDONLY | O_NONBLOCK
    );

    if (power_fd < 0) {
        perror("Cannot open power button");
    }
}

static void read_menu_events(void)
{
    struct input_event ev;

    while (read(menu_fd, &ev, sizeof(ev)) == sizeof(ev)) {
        if (ev.type != EV_KEY)
            continue;

        if (ev.value != 0 && ev.value != 1)
            continue;

		if (ev.code == ORBIC_MENU) {
			if (menuactive) {
				/* Menu button moves down through menu choices. */
				if (ev.value == 1)
					post_key(ev_keydown, KEY_DOWNARROW);

				menu_down = 0;
			} else if (gamestate != GS_LEVEL) {
				/* Open the menu from the title/demo screen. */
				if (ev.value == 1)
					post_key(ev_keydown, KEY_ESCAPE);

				menu_down = 0;
			} else {
				menu_down = ev.value;
			}
		}

        else if (ev.code == ORBIC_RESET) {
            if (gamestate != GS_LEVEL) {
                post_key(
                    ev.value ? ev_keydown : ev_keyup,
                    13
                );
            }

            else if (menu_down) {
                /*
                 * Hold menu and tap reset:
                 * use switches and open doors.
                 */
                post_key(
                    ev.value ? ev_keydown : ev_keyup,
                    ' '
                );
            }

            else {
                /* Tap reset to fire. */
                post_key(
                    ev.value ? ev_keydown : ev_keyup,
                    KEY_RCTRL
                );
            }
        }
    }
}

static void read_power_events(void)
{
    struct input_event ev;

    while (read(power_fd, &ev, sizeof(ev)) == sizeof(ev)) {
        if (ev.type != EV_KEY ||
            ev.code != ORBIC_POWER ||
            (ev.value != 0 && ev.value != 1)) {
            continue;
        }

        if (menuactive) {
            /* Only keydown is needed for a menu selection. */
            if (ev.value == 1)
                post_key(ev_keydown, 13);

            power_down = 0;
        } else if (gamestate != GS_LEVEL) {
            /*
             * At the title/demo screen, Escape opens the menu.
             * Press Power again afterward to select.
             */
            if (ev.value == 1)
                post_key(ev_keydown, KEY_ESCAPE);

            power_down = 0;
        } else {
            power_down = ev.value;
        }
    }
}

void I_StartTic(void)
{
    if (menu_fd >= 0)
        read_menu_events();

    if (power_fd >= 0)
        read_power_events();

    update_movement();
}

int main(int argc, char **argv)
{
    myargc = argc;
    myargv = argv;

    I_InitInput();
    D_DoomMain();

    return 0;
}