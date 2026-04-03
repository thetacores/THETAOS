// SPDX-License-Identifier: GPL-2.0
/*
 * input.c - Generic input event subsystem
 *
 * Provides a ring buffer of struct input_event that input drivers
 * (keyboard, mouse, etc.) push into and consumers read from.
 */

#include <input/input.h>

static struct input_event buf[INPUT_BUF_SIZE];
static uint16_t buf_head;
static uint16_t buf_tail;

static void buf_put(const struct input_event *ev)
{
	uint16_t next = (buf_head + 1) % INPUT_BUF_SIZE;

	if (next == buf_tail)
		return;		/* full, drop */
	buf[buf_head] = *ev;
	buf_head = next;
}

void input_report_key(uint16_t code, int32_t value)
{
	struct input_event ev = {
		.type  = EV_KEY,
		.code  = code,
		.value = value,
	};

	buf_put(&ev);
}

void input_sync(void)
{
	struct input_event ev = {
		.type  = EV_SYN,
		.code  = SYN_REPORT,
		.value = 0,
	};

	buf_put(&ev);
}

int input_read_event(struct input_event *ev)
{
	if (buf_head == buf_tail)
		return 0;

	*ev = buf[buf_tail];
	buf_tail = (buf_tail + 1) % INPUT_BUF_SIZE;
	return 1;
}

void input_init(void)
{
	buf_head = 0;
	buf_tail = 0;
}
