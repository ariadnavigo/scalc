/* See LICENSE for copyright and license details. */

#define CMD_ID_SIZE 32

enum cmd_type {
	CMD_NULL,
	CMD_NOARGS,
	CMD_SETTER,
	CMD_STACK
};

typedef struct {
	char id[CMD_ID_SIZE];
	enum cmd_type type;
	union {
		int (*noargs)(void);
		int (*setter)(char, double);
		int (*stack)(Stack *);
	} func;
} CmdReg;

const CmdReg *cmd(const char *name);

