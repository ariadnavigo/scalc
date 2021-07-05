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
	VT_BCKSP,
	VT_RET
};

static int term_key(void);

static int
term_key(void)
{
	/* 
	 * We want this to only act upon what we want to. We should discard all 
	 * we don't plan to implement or haven't implemented yet by returning
	 * VT_DEF.
	 */

	char key;
	char seq[SEQ_SIZE];
	int nread;

	while ((nread = read(STDIN_FILENO, &key, 1)) != 1) {
		if (nread == -1)
			return -1;
	}

	/* 
	 * This isn't fully implemented yet. Some 3-byte sequences are read
	 * partially still and leak the final '~'.
	 */
	if (key == '\x1b') { /* Maybe iscntrl() is better */
		if (read(STDIN_FILENO, &seq[0], 1) != 1)
			return key;
		if (read(STDIN_FILENO, &seq[1], 1) != 1)
			return key;

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
		return VT_BCKSP;
	} else if (key == '\x0a') {
		return VT_RET;
	} else {
		/* Not an escaped or control key */
		return key;
	}
}

int
sline_setup(struct termios *term)
{
	/* TODO: Probably wrong */

	/* term->c_iflag &= ~IXON; */
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

	int key;
	size_t i;

	memset(dest, 0, size);

	i = 0;
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
			return i;
		case VT_BCKSP:
			if (i > 0) {
				--i;
				dest[i] = '\0';
				write(STDOUT_FILENO, "\b \b", 3);
			}
			break;
		case VT_DEF:
			continue;
		default:
			if (i < size) {
				dest[i] = key;
				++i;
			}
			write(STDOUT_FILENO, &key, 1);
			break;
		}

	}

	return -1;
}
