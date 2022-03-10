# See LICENSE file for copyright and license details.

.POSIX:

include config.mk

SRC = cmd.c mem.c op.c scalc.c stack.c strlcpy.c utils.c
OBJ = ${SRC:.c=.o}

all: options scalc

options:
	@echo Build options:
	@echo "CPPFLAGS = ${CPPFLAGS}"
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"
	@echo

${OBJ}: config.h config.mk

config.h:
	cp config.def.h $@

scalc: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS} ${LIBS}

clean:
	rm -f scalc ${OBJ}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f scalc ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/scalc
	mkdir -p ${DESTDIR}${MANPREFIX}/man1
	sed "s/VERSION/${VERSION}/g" scalc.1 \
	    > ${DESTDIR}${MANPREFIX}/man1/scalc.1
	chmod 644 ${DESTDIR}${MANPREFIX}/man1/scalc.1

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/scalc ${DESTDIR}${MANPREFIX}/man1/scalc.1

.PHONY: all options clean install uninstall
