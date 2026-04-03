// SPDX-License-Identifier: GPL-2.0
/*
 * keyboard.c - AT keyboard driver (scan code set 1)
 *
 * Translates raw scancodes into keycodes via scancode_to_keycode[],
 * reports events through the input subsystem, and provides a keymap
 * layer for ASCII conversion.
 */

#include <drivers/keyboard.h>
#include <klib/kprintf.h>

static inline uint8_t inb(uint16_t port)
{
	uint8_t ret;
	__asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

/*
 * Scan-code set 1 -> keycode translation table.
 * Index = raw scancode (make code), value = KEY_* constant.
 * Mirrors the Linux atkbd scancode-to-keycode mapping for set 1.
 */
static const uint16_t scancode_to_keycode[128] = {
	[0x00] = KEY_RESERVED,
	[0x01] = KEY_ESC,
	[0x02] = KEY_1,       [0x03] = KEY_2,       [0x04] = KEY_3,
	[0x05] = KEY_4,       [0x06] = KEY_5,       [0x07] = KEY_6,
	[0x08] = KEY_7,       [0x09] = KEY_8,       [0x0A] = KEY_9,
	[0x0B] = KEY_0,       [0x0C] = KEY_MINUS,   [0x0D] = KEY_EQUAL,
	[0x0E] = KEY_BACKSPACE,
	[0x0F] = KEY_TAB,
	[0x10] = KEY_Q,       [0x11] = KEY_W,       [0x12] = KEY_E,
	[0x13] = KEY_R,       [0x14] = KEY_T,       [0x15] = KEY_Y,
	[0x16] = KEY_U,       [0x17] = KEY_I,       [0x18] = KEY_O,
	[0x19] = KEY_P,       [0x1A] = KEY_LEFTBRACE, [0x1B] = KEY_RIGHTBRACE,
	[0x1C] = KEY_ENTER,
	[0x1D] = KEY_LEFTCTRL,
	[0x1E] = KEY_A,       [0x1F] = KEY_S,       [0x20] = KEY_D,
	[0x21] = KEY_F,       [0x22] = KEY_G,       [0x23] = KEY_H,
	[0x24] = KEY_J,       [0x25] = KEY_K,       [0x26] = KEY_L,
	[0x27] = KEY_SEMICOLON, [0x28] = KEY_APOSTROPHE, [0x29] = KEY_GRAVE,
	[0x2A] = KEY_LEFTSHIFT,
	[0x2B] = KEY_BACKSLASH,
	[0x2C] = KEY_Z,       [0x2D] = KEY_X,       [0x2E] = KEY_C,
	[0x2F] = KEY_V,       [0x30] = KEY_B,       [0x31] = KEY_N,
	[0x32] = KEY_M,       [0x33] = KEY_COMMA,   [0x34] = KEY_DOT,
	[0x35] = KEY_SLASH,
	[0x36] = KEY_RIGHTSHIFT,
	[0x37] = KEY_KPASTERISK,
	[0x38] = KEY_LEFTALT,
	[0x39] = KEY_SPACE,
	[0x3A] = KEY_CAPSLOCK,
	[0x3B] = KEY_F1,      [0x3C] = KEY_F2,      [0x3D] = KEY_F3,
	[0x3E] = KEY_F4,      [0x3F] = KEY_F5,      [0x40] = KEY_F6,
	[0x41] = KEY_F7,      [0x42] = KEY_F8,      [0x43] = KEY_F9,
	[0x44] = KEY_F10,
	[0x45] = KEY_NUMLOCK,
	[0x46] = KEY_SCROLLLOCK,
	[0x57] = KEY_F11,
	[0x58] = KEY_F12,
};

/*
 * Keymap layers: keycode -> ASCII.
 * Layer 0 = plain, layer 1 = shift.
 */
#define KEYMAP_LAYERS	2
#define KEYMAP_SIZE	KEY_MAX

static const char keymap[KEYMAP_LAYERS][KEYMAP_SIZE] = {
	[0] = {
		[KEY_ESC] = 27,
		[KEY_1] = '1', [KEY_2] = '2', [KEY_3] = '3', [KEY_4] = '4',
		[KEY_5] = '5', [KEY_6] = '6', [KEY_7] = '7', [KEY_8] = '8',
		[KEY_9] = '9', [KEY_0] = '0', [KEY_MINUS] = '-', [KEY_EQUAL] = '=',
		[KEY_BACKSPACE] = '\b',
		[KEY_TAB] = '\t',
		[KEY_Q] = 'q', [KEY_W] = 'w', [KEY_E] = 'e', [KEY_R] = 'r',
		[KEY_T] = 't', [KEY_Y] = 'y', [KEY_U] = 'u', [KEY_I] = 'i',
		[KEY_O] = 'o', [KEY_P] = 'p',
		[KEY_LEFTBRACE] = '[', [KEY_RIGHTBRACE] = ']',
		[KEY_ENTER] = '\n',
		[KEY_A] = 'a', [KEY_S] = 's', [KEY_D] = 'd', [KEY_F] = 'f',
		[KEY_G] = 'g', [KEY_H] = 'h', [KEY_J] = 'j', [KEY_K] = 'k',
		[KEY_L] = 'l',
		[KEY_SEMICOLON] = ';', [KEY_APOSTROPHE] = '\'', [KEY_GRAVE] = '`',
		[KEY_BACKSLASH] = '\\',
		[KEY_Z] = 'z', [KEY_X] = 'x', [KEY_C] = 'c', [KEY_V] = 'v',
		[KEY_B] = 'b', [KEY_N] = 'n', [KEY_M] = 'm',
		[KEY_COMMA] = ',', [KEY_DOT] = '.', [KEY_SLASH] = '/',
		[KEY_KPASTERISK] = '*',
		[KEY_SPACE] = ' ',
	},
	[1] = {
		[KEY_ESC] = 27,
		[KEY_1] = '!', [KEY_2] = '@', [KEY_3] = '#', [KEY_4] = '$',
		[KEY_5] = '%', [KEY_6] = '^', [KEY_7] = '&', [KEY_8] = '*',
		[KEY_9] = '(', [KEY_0] = ')', [KEY_MINUS] = '_', [KEY_EQUAL] = '+',
		[KEY_BACKSPACE] = '\b',
		[KEY_TAB] = '\t',
		[KEY_Q] = 'Q', [KEY_W] = 'W', [KEY_E] = 'E', [KEY_R] = 'R',
		[KEY_T] = 'T', [KEY_Y] = 'Y', [KEY_U] = 'U', [KEY_I] = 'I',
		[KEY_O] = 'O', [KEY_P] = 'P',
		[KEY_LEFTBRACE] = '{', [KEY_RIGHTBRACE] = '}',
		[KEY_ENTER] = '\n',
		[KEY_A] = 'A', [KEY_S] = 'S', [KEY_D] = 'D', [KEY_F] = 'F',
		[KEY_G] = 'G', [KEY_H] = 'H', [KEY_J] = 'J', [KEY_K] = 'K',
		[KEY_L] = 'L',
		[KEY_SEMICOLON] = ':', [KEY_APOSTROPHE] = '"', [KEY_GRAVE] = '~',
		[KEY_BACKSLASH] = '|',
		[KEY_Z] = 'Z', [KEY_X] = 'X', [KEY_C] = 'C', [KEY_V] = 'V',
		[KEY_B] = 'B', [KEY_N] = 'N', [KEY_M] = 'M',
		[KEY_COMMA] = '<', [KEY_DOT] = '>', [KEY_SLASH] = '?',
		[KEY_KPASTERISK] = '*',
		[KEY_SPACE] = ' ',
	},
};

/* Modifier state */
static uint8_t shift_l, shift_r, ctrl, alt, capslock;

static int get_layer(void)
{
	int shift = (shift_l | shift_r) != 0;
	int caps  = capslock != 0;

	return shift ^ caps;
}

void keyboard_irq(void)
{
	uint8_t sc = inb(KB_DATA_PORT);
	int release = (sc & 0x80) != 0;
	uint8_t make = sc & 0x7F;
	uint16_t kc;

	if (make >= 128)
		return;

	kc = scancode_to_keycode[make];
	if (kc == KEY_RESERVED)
		return;

	/* Track modifier state */
	switch (kc) {
	case KEY_LEFTSHIFT:  shift_l  = !release; break;
	case KEY_RIGHTSHIFT: shift_r  = !release; break;
	case KEY_LEFTCTRL:   ctrl     = !release; break;
	case KEY_LEFTALT:    alt      = !release; break;
	case KEY_CAPSLOCK:
		if (!release)
			capslock ^= 1;
		break;
	}

	/* Report the event into the input subsystem */
	input_report_key(kc, release ? KEY_RELEASE : KEY_PRESS);
	input_sync();
}

char keyboard_getc(void)
{
	struct input_event ev;

	while (input_read_event(&ev)) {
		if (ev.type != EV_KEY || ev.value != KEY_PRESS)
			continue;
		if (ev.code >= KEYMAP_SIZE)
			continue;

		char c = keymap[get_layer()][ev.code];
		if (c)
			return c;
	}
	return 0;
}

void keyboard_init(void)
{
	shift_l = shift_r = ctrl = alt = capslock = 0;
	input_init();
	kprintf("[KB] keyboard driver initialized (input subsystem)\n");
}
