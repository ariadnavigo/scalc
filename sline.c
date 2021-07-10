/* See LICENSE for copyright and license details. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "sline.h"
#include "strlcpy.h"

#define CURSOR_BUF_SIZE 16 /* Used for cursor movement directives */
#define HISTORY_SIZE 50

enum {
	VT_DEF,
	VT_UP,
	VT_DWN,
	VT_LFT,
	VT_RGHT,
	VT_BKSPC,
	VT_RET,
	VT_HOME,
	VT_END,
	VT_DEL
};

enum {
	SLINE_ERR_DEF,
	SLINE_ERR_TERMIOS_GET,
	SLINE_ERR_TERMIOS_SET
};

static char *buf_slice(char *src, int pivot);

static int term_key(void);
static int term_esc(char *seq);

static size_t key_bkspc(char *buf, size_t pos);
static size_t key_up(char *buf, size_t size, int *hist_num, size_t pos);
static size_t key_down(char *buf, size_t size, int *hist_num, size_t pos);
static size_t key_left(size_t pos);
static size_t key_right(char *buf, size_t pos);
static size_t key_home(size_t pos);
static size_t key_end(char *buf, size_t pos);
static size_t key_insert(char *buf, size_t pos, size_t size, char key);

static int history_add(const char *input);
static const char *history_get(int pos);
static int history_rotate(void);

static int sline_errno = SLINE_ERR_DEF;
static struct termios old, term;
static char *history[HISTORY_SIZE];
static int hist_last = -1;

static char *
buf_slice(char *src, int pivot)
{
	char *suff;
	size_t len;

	len = strlen(src);

	if ((suff = calloc(len + 1, sizeof(char))) == NULL)
		return NULL;

	strlcpy(suff, src + pivot, len - pivot + 1);
	memset(src + pivot, 0, len - pivot);

	return suff;
}

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
	char seq[3];
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
		case 'H':
			return VT_HOME;
		case 'F':
			return VT_END;
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
key_bkspc(char *buf, size_t pos)
{
	if (pos > 0) {
		--pos;
		buf[pos] = '\0';
		write(STDOUT_FILENO, "\b \b", 3);
	}

	return pos;
}

static size_t
key_up(char *buf, size_t size, int *hist_num, size_t pos)
{
	const char *hist;
	size_t len;

	if (*hist_num > 0)
		--(*hist_num);

	if ((hist = history_get(*hist_num)) == NULL)
		return pos;

	strlcpy(buf, hist, size);

	pos = key_home(pos);
	len = strlen(hist);
	write(STDOUT_FILENO, "\x1b[0K", 4);
	write(STDOUT_FILENO, hist, len);

	return len;
}

static size_t
key_down(char *buf, size_t size, int *hist_num, size_t pos)
{
	const char *hist;
	size_t len;

	++(*hist_num);

	/* Return to blank prompt if we navigate after last history item. */
	if (*hist_num > hist_last) {
		memset(buf, 0, size);
		*hist_num = hist_last + 1; /* To avoid hist_num growing */
		pos = key_home(pos);
		write(STDOUT_FILENO, "\x1b[0K", 4);
		return pos;
	}

	if ((hist = history_get(*hist_num)) == NULL)
		return pos;

	strlcpy(buf, hist, size);

	pos = key_home(pos);
	len = strlen(hist);
	write(STDOUT_FILENO, "\x1b[0K", 4);
	write(STDOUT_FILENO, hist, len);
	
	return len; /* len is the new pos we output */
}

static size_t
key_left(size_t pos)
{
	if (pos > 0) {
		--pos;
		write(STDOUT_FILENO, "\x1b[D", 3);
	}

	return pos;
}

static size_t
key_right(char *buf, size_t pos)
{
	if (pos < strlen(buf)) {
		++pos;
		write(STDOUT_FILENO, "\x1b[C", 3);
	}
	
	return pos;
}

static size_t
key_home(size_t pos)
{
	char cmd[CURSOR_BUF_SIZE];

	if (pos > 0) {
		snprintf(cmd, CURSOR_BUF_SIZE, "\x1b[%zdD", pos);
		write(STDOUT_FILENO, cmd, strlen(cmd));
	}

	return 0;
}


static size_t
key_end(char *buf, size_t pos)
{
	size_t len;
	char cmd[CURSOR_BUF_SIZE];

	len = strlen(buf);
	if (pos < len) {
		snprintf(cmd, CURSOR_BUF_SIZE, "\x1b[%zdC", len - pos);
		write(STDOUT_FILENO, cmd, strlen(cmd));
	}

	return len;
}

static size_t
key_insert(char *buf, size_t pos, size_t size, char key)
{
	char *suff;
	size_t len;

	if (pos >= size)
		return pos;

	if ((suff = buf_slice(buf, pos)) == NULL)
		return pos;

	len = strlen(suff);
	buf[pos] = key;
	++pos;
	strlcpy(buf + pos, suff, len + 1);

	write(STDOUT_FILENO, &key, 1);
	write(STDOUT_FILENO, "\x1b[0K", 4);
	write(STDOUT_FILENO, "\x1b[s", 3);
	write(STDOUT_FILENO, suff, len);
	write(STDOUT_FILENO, "\x1b[u", 3);

	free(suff);
	
	return pos;
}

static int
history_add(const char *input)
{
	size_t hist_size;

	hist_size = strlen(input) + 1;
	++hist_last;
	if (hist_last >= HISTORY_SIZE)
		history_rotate();

	history[hist_last] = calloc(hist_size, sizeof(char));
	if (history[hist_last] == NULL) {
		--hist_last;
		return -1;
	}

	strlcpy(history[hist_last], input, hist_size);

	return 0;
}

static const char *
history_get(int pos)
{
	if (pos < 0 || pos > hist_last)
		return NULL;

	return history[pos];
}

static int
history_rotate(void)
{
	int i;

	free(history[0]);
	for (i = 1; i < HISTORY_SIZE; ++i)
		history[i - 1] = history[i];

	history[i - 1] = NULL;
	--hist_last;

	return 0;
}

int
sline_setup(void)
{
	if (tcgetattr(STDIN_FILENO, &old) < 0) {
		sline_errno = SLINE_ERR_TERMIOS_GET;
		return -1;
	}

	term = old;
	term.c_lflag &= ~(ICANON | ECHO);
	term.c_cc[VMIN] = 0;
	term.c_cc[VTIME] = 1;
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
		sline_errno = SLINE_ERR_TERMIOS_SET;
		return -1;
	}

	return 0;
}

void
sline_end(void)
{
	int i;

	if (hist_last < 0)
		goto termios;

	for (i = 0; i < HISTORY_SIZE; ++i) {
		if (history[i] != NULL)
			free(history[i]);
	}

termios:
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &old);
}

const char *
sline_errmsg(void)
{
	switch (sline_errno) {
	case SLINE_ERR_TERMIOS_GET:
		return "could not read attributes.";
	case SLINE_ERR_TERMIOS_SET:
		return "could not set attributes.";
	default:
		return "unknown error.";
	}
}

int
sline(char *buf, size_t size)
{
	/* Work in progress */

	char key;
	int hist_num;
	size_t pos;

	memset(buf, 0, size);

	pos = 0;
	hist_num = hist_last + 1;
	while ((key = term_key()) != -1) {
		switch (key) {
		/* Arrow keys not implemented yet. */
		case VT_UP:
			pos = key_up(buf, size, &hist_num, pos);
			break;
		case VT_DWN:
			pos = key_down(buf, size, &hist_num, pos);
			break;
		case VT_LFT:
			pos = key_left(pos);
			break;
		case VT_RGHT:
			pos = key_right(buf, pos);
			break;
		case VT_RET:
			write(STDOUT_FILENO, "\n", 1);
			history_add(buf);
			return pos;
		case VT_BKSPC:
			pos = key_bkspc(buf, pos);
			hist_num = hist_last;
			break;
		case VT_HOME:
			pos = key_home(pos);
			break;
		case VT_END:
			pos = key_end(buf, pos);
			break;
		case VT_DEF:
			continue;
		default:
			pos = key_insert(buf, pos, size, key);
			hist_num = hist_last;
			break;
		}

	}

	return -1;
}
