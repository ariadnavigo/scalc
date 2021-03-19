# See LICENSE file for copyright and license details.

.POSIX:

include config.mk

SRC = op.c rpn.c scalc.c
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

rpn.o: op.h
scalc.o: rpn.h

${OBJ}: config.mk

scalc: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -f scalc ${OBJ} scalc-${VERSION}.tar.gz

dist: clean
	mkdir -p scalc-${VERSION}
	cp -R LICENSE Makefile README.md config.mk op.h rpn.h scalc.1 ${SRC} \
		scalc-${VERSION}
	tar -cf scalc-${VERSION}.tar scalc-${VERSION}
	gzip scalc-${VERSION}.tar
	rm -rf scalc-${VERSION}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f scalc ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/scalc
	mkdir -p ${DESTDIR}${MANPREFIX}/man1
	sed "s/VERSION/${VERSION}/g" scalc.1\
		> ${DESTDIR}${MANPREFIX}/man1/scalc.1
	chmod 644 ${DESTDIR}${MANPREFIX}/man1/scalc.1

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/scalc ${DESTDIR}${MANPREFIX}/man1/scalc.1

.PHONY: all options clean dist install uninstall
