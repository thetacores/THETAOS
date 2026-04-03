/* SPDX-License-Identifier: GPL-2.0 */
#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

/*
 * Input event subsystem.
 *
 * Drivers report raw events via input_report_key() / input_sync().
 * Consumers read cooked events via input_read_event().
 */

/* Event types */
#define EV_SYN			0x00
#define EV_KEY			0x01

/* EV_SYN codes */
#define SYN_REPORT		0

/* EV_KEY values */
#define KEY_RELEASE		0
#define KEY_PRESS		1
#define KEY_REPEAT		2

struct input_event {
	uint16_t type;
	uint16_t code;
	int32_t  value;
};

#define INPUT_BUF_SIZE		128

void input_init(void);

/* Driver-facing: report events into the subsystem */
void input_report_key(uint16_t code, int32_t value);
void input_sync(void);

/* Consumer-facing: read next event, returns 1 on success, 0 if empty */
int  input_read_event(struct input_event *ev);

#endif
