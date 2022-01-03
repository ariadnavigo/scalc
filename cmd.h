/* See LICENSE for copyright and license details. */

#define CMD_ID_SIZE 32

typedef struct {
	char id[CMD_ID_SIZE];
	int (*func)(const char *args);
} CmdReg;

const CmdReg *cmd(const char *name);
int cmd_valid(const CmdReg *ptr);

