# edit - simple text editor
# See LICENSE file for copyright and license details.

include config.mk

SRC = edit.c
OBJ = ${SRC:.c=.o}

all: options edit

options:
	@echo edit build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	${CC} -c ${CFLAGS} $<

${OBJ}: config.h config.mk languages.h termbox2.h

config.h:
	cp config.def.h $@

edit: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -f edit ${OBJ} edit-${VERSION}.tar.gz

dist: clean
	mkdir -p edit-${VERSION}
	cp -R LICENSE Makefile README config.def.h config.mk\
		edit.1 languages.h termbox2.h ${SRC} edit-${VERSION}
	tar -cf edit-${VERSION}.tar edit-${VERSION}
	gzip edit-${VERSION}.tar
	rm -rf edit-${VERSION}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f edit ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/edit
	mkdir -p ${DESTDIR}${MANPREFIX}/man1
	sed "s/VERSION/${VERSION}/g" < edit.1 > ${DESTDIR}${MANPREFIX}/man1/edit.1
	chmod 644 ${DESTDIR}${MANPREFIX}/man1/edit.1

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/edit\
		${DESTDIR}${MANPREFIX}/man1/edit.1

.PHONY: all options clean dist install uninstall
