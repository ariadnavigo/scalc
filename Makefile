# See LICENSE file for copyright and license details.

.POSIX:

include config.mk

SRC = rpn.c scalc.c
OBJ = ${SRC:%.c=%.o}

all: options scalc

options:
	@echo Build options:
	@echo "CFLAGS 	= ${CFLAGS}"
	@echo "LDFLAGS	= ${LDFLAGS}"
	@echo "CC	= ${CC}"
	@echo

.c.o:
	${CC} -c ${CFLAGS} $<

scalc.o: rpn.h

${OBJ}: config.mk

scalc: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -f scalc ${OBJ}

.PHONY: all options clean
