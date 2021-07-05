/* See LICENSE for copyright and license details. */

#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "sline.h"
#include "strlcpy.h"

#define SEQ_SIZE 3

enum {
	VT_DEF,
	VT_UP,
	VT_DWN,
	VT_LFT,
	VT_RGHT,
	VT_BKSPC,
	VT_RET
};

static int term_key(void);
static int term_esc(char *seq);

static size_t key_bkspc(char *dest, size_t pos);
static size_t key_default(char *dest, size_t pos, size_t size, char key);

static int
term_esc(char *seq)
{
	if (read(STDIN_FILENO, &seq[0], 1) != 1)
		return -1;
	if (read(STDIN_FILENO, &seq[1], 1) != 1)
		return -1;

	if (seq[0] != '[')
		return -1;

	if (seq[1] >= '0' && seq[1] <= '9') {
		if (read(STDIN_FILENO, &seq[2], 1) != 1)
			return -1;
	}

	return 0;
}

static int
term_key(void)
{
	char key;
	char seq[SEQ_SIZE];
	int nread;

	while ((nread = read(STDIN_FILENO, &key, 1)) != 1) {
		if (nread == -1)
			return -1;
	}

	if (key == '\x1b') {
		if (term_esc(seq) < 0)
			return VT_DEF;

		switch (seq[1]) {
		case 'A':
			return VT_UP;
		case 'B':
			return VT_DWN;
		case 'C':
			return VT_RGHT;
		case 'D':
			return VT_LFT;
		default:
			return VT_DEF;
		}
	} else if (key == '\x7f') {
		return VT_BKSPC;
	} else if (key == '\x0a') {
		return VT_RET;
	} else {
		/* Not an escaped or control key */
		return key;
	}
}

static size_t
key_bkspc(char *dest, size_t pos)
{
	if (pos > 0) {
		--pos;
		dest[pos] = '\0';
		write(STDOUT_FILENO, "\b \b", 3);
	}

	return pos;
}

static size_t
key_default(char *dest, size_t pos, size_t size, char key)
{
	if (pos < size) {
		dest[pos] = key;
		++pos;
	}
	write(STDOUT_FILENO, &key, 1);

	return pos;
}

int
sline_setup(struct termios *term)
{
	term->c_lflag &= ~(ICANON | ECHO);
	term->c_cc[VMIN] = 0;
	term->c_cc[VTIME] = 1;
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, term) < 0)
		return -1;

	return 0;
}

int
sline(char *dest, size_t size)
{
	/* Work in progress */

	char key;
	size_t pos;

	memset(dest, 0, size);

	pos = 0;
	while ((key = term_key()) != -1) {
		switch (key) {
		/* Arrow keys not implemented yet. */
		case VT_UP:
			continue;
		case VT_DWN:
			continue;
		case VT_LFT:
			continue;
		case VT_RGHT:
			continue;
		case VT_RET:
			write(STDOUT_FILENO, "\n", 1);
			return pos;
		case VT_BKSPC:
			pos = key_bkspc(dest, pos);
			break;
		case VT_DEF:
			continue;
		default:
			pos = key_default(dest, pos, size, key);
			break;
		}

	}

	return -1;
}
