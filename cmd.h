/* See LICENSE for copyright and license details. */

#define CMD_ID_SIZE 32

enum cmd_type {
	CMD_NULL,
	CMD_CMD,
	CMD_MEM,
	CMD_STK
};

typedef struct {
	char id[CMD_ID_SIZE];
	enum cmd_type type;
	union {
		int (*cmd)(void);
		int (*mem)(char, double);
		int (*stk)(Stack *);
	} func;
} CmdReg;

const CmdReg *cmd(const char *name);