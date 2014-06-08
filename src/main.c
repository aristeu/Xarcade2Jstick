/* ======================================================================== */
/*  This program is free software; you can redistribute it and/or modify    */
/*  it under the terms of the GNU General Public License as published by    */
/*  the Free Software Foundation; either version 2 of the License, or       */
/*  (at your option) any later version.                                     */
/*                                                                          */
/*  This program is distributed in the hope that it will be useful,         */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of          */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       */
/*  General Public License for more details.                                */
/*                                                                          */
/*  You should have received a copy of the GNU General Public License       */
/*  along with this program; if not, write to the Free Software             */
/*  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.               */
/* ======================================================================== */
/*                 Copyright (c) 2014, Florian Müller                       */
/* ======================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <signal.h>
#include <time.h>

#include "uinput_gamepad.h"
#include "uinput_kbd.h"
#include "input_xarcade.h"

// TODO Extract all magic numbers and collect them as defines in at a central location

#define GPADSNUM 2

UINP_KBD_DEV uinp_kbd;
UINP_GPAD_DEV uinp_gpads[GPADSNUM];
INP_XARC_DEV xarcdev;

int main(int argc, char* argv[]) {
	int result = 0;
	int rd, ctr;
	char keyStates[256];
	
	printf("[Xarcade2Joystick] Getting exclusive access: ");
	result = uinput_gpad_open(&uinp_gpads[0], UINPUT_GPAD_TYPE_XARCADE);
	printf("%s\n", (result == 0) ? "SUCCESS" : "FAILURE");
	if (result != 0) {
		exit(-1);
	}

	uinput_gpad_open(&uinp_gpads[1], UINPUT_GPAD_TYPE_XARCADE);
	uinput_kbd_open(&uinp_kbd);
	input_xarcade_open(&xarcdev, INPUT_XARC_TYPE_TANKSTICK);

	while (1) {
		rd = input_xarcade_read(&xarcdev);
		if (rd == 0) {
			break;
		}

		for (ctr = 0; ctr < rd; ctr++) {
			if (xarcdev.ev[ctr].type == 0)
				continue;
			if (xarcdev.ev[ctr].type == EV_MSC)
				continue;
			if (EV_KEY == xarcdev.ev[ctr].type) {

				keyStates[xarcdev.ev[ctr].code] = xarcdev.ev[ctr].value;

				switch (xarcdev.ev[ctr].code) {

				/* ----------------  Player 1 controls ------------------- */
				/* buttons */
				case 29:
					uinput_gpad_write(&uinp_gpads[0], BTN_A, xarcdev.ev[ctr].value > 0,
							EV_KEY);
					break;
				case 56:
					uinput_gpad_write(&uinp_gpads[0], BTN_B, xarcdev.ev[ctr].value > 0,
							EV_KEY);
					break;
				case 57:
					uinput_gpad_write(&uinp_gpads[0], BTN_C, xarcdev.ev[ctr].value > 0,
							EV_KEY);
					break;
				case 42:
					uinput_gpad_write(&uinp_gpads[0], BTN_X, xarcdev.ev[ctr].value > 0,
							EV_KEY);
					break;
				case 44:
					uinput_gpad_write(&uinp_gpads[0], BTN_Y, xarcdev.ev[ctr].value > 0,
							EV_KEY);
					break;
				case 45:
					uinput_gpad_write(&uinp_gpads[0], BTN_Z, xarcdev.ev[ctr].value > 0,
							EV_KEY);
					break;
				case 46:
					uinput_gpad_write(&uinp_gpads[0], BTN_TL, xarcdev.ev[ctr].value > 0,
							EV_KEY);
					break;
				case 6:
					uinput_gpad_write(&uinp_gpads[0], BTN_TR, xarcdev.ev[ctr].value > 0,
							EV_KEY);
					break;
				case 2:
					uinput_gpad_write(&uinp_gpads[0], BTN_START,
							xarcdev.ev[ctr].value > 0, EV_KEY);
					break;
				case 4:
					uinput_gpad_write(&uinp_gpads[0], BTN_SELECT,
							xarcdev.ev[ctr].value > 0, EV_KEY);
					break;

					/* joystick */
				case 75:
					uinput_gpad_write(&uinp_gpads[0], ABS_X,
							xarcdev.ev[ctr].value == 0 ? 2 : 0, EV_ABS); // center or left
					break;
				case 77:
					uinput_gpad_write(&uinp_gpads[0], ABS_X,
							xarcdev.ev[ctr].value == 0 ? 2 : 4, EV_ABS); // center or right
					break;
				case 72:
					uinput_gpad_write(&uinp_gpads[0], ABS_Y,
							xarcdev.ev[ctr].value == 0 ? 2 : 0, EV_ABS); // center or up
					break;
				case 80:
					uinput_gpad_write(&uinp_gpads[0], ABS_Y,
							xarcdev.ev[ctr].value == 0 ? 2 : 4, EV_ABS); // center or down
					break;

					/* ----------------  Player 2 controls ------------------- */
					/* buttons */
				case 30:
					uinput_gpad_write(&uinp_gpads[1], BTN_A, xarcdev.ev[ctr].value > 0,
							EV_KEY);
					break;
				case 31:
					uinput_gpad_write(&uinp_gpads[1], BTN_B, xarcdev.ev[ctr].value > 0,
							EV_KEY);
					break;
				case 16:
					uinput_gpad_write(&uinp_gpads[1], BTN_C, xarcdev.ev[ctr].value > 0,
							EV_KEY);
					break;
				case 17:
					uinput_gpad_write(&uinp_gpads[1], BTN_X, xarcdev.ev[ctr].value > 0,
							EV_KEY);
					break;
				case 18:
					uinput_gpad_write(&uinp_gpads[1], BTN_Y, xarcdev.ev[ctr].value > 0,
							EV_KEY);
					break;
				case 26:
					uinput_gpad_write(&uinp_gpads[1], BTN_Z, xarcdev.ev[ctr].value > 0,
							EV_KEY);
					break;
				case 27:
					uinput_gpad_write(&uinp_gpads[1], BTN_TL, xarcdev.ev[ctr].value > 0,
							EV_KEY);
					break;
				case 7:
					uinput_gpad_write(&uinp_gpads[1], BTN_TR, xarcdev.ev[ctr].value > 0,
							EV_KEY);
					break;
				case 3:
					uinput_gpad_write(&uinp_gpads[1], BTN_START,
							xarcdev.ev[ctr].value > 0, EV_KEY);
					break;
				case 5:
					uinput_gpad_write(&uinp_gpads[1], BTN_SELECT,
							xarcdev.ev[ctr].value > 0, EV_KEY);
					break;

					/* joystick */
				case 32:
					uinput_gpad_write(&uinp_gpads[1], ABS_X,
							xarcdev.ev[ctr].value == 0 ? 2 : 0, EV_ABS); // center or left
					break;
				case 34:
					uinput_gpad_write(&uinp_gpads[1], ABS_X,
							xarcdev.ev[ctr].value == 0 ? 2 : 4, EV_ABS); // center or right
					break;
				case 19:
					uinput_gpad_write(&uinp_gpads[1], ABS_Y,
							xarcdev.ev[ctr].value == 0 ? 2 : 0, EV_ABS); // center or up
					break;
				case 33:
					uinput_gpad_write(&uinp_gpads[1], ABS_Y,
							xarcdev.ev[ctr].value == 0 ? 2 : 4, EV_ABS); // center or down
					break;

				default:
					break;
				}

				/* button combinations */
				if (keyStates[2] == 2 && keyStates[4] > 0) { // TAB key
					uinput_kbd_write(&uinp_kbd, KEY_TAB, 1, EV_KEY);
				} else {
					uinput_kbd_write(&uinp_kbd, KEY_TAB, 0, EV_KEY);
				}
				if (keyStates[3] == 2 && keyStates[5] > 0) { // ESC key
					uinput_kbd_write(&uinp_kbd, KEY_ESC, 1, EV_KEY);
				} else {
					uinput_kbd_write(&uinp_kbd, KEY_ESC, 0, EV_KEY);
				}
			}
		}
	}

	printf("Exiting.\n");
	input_xarcade_close(&xarcdev);
	uinput_gpad_close(&uinp_gpads[0]);
	uinput_gpad_close(&uinp_gpads[1]);
	uinput_kbd_close(&uinp_kbd);
	return 0;
}
