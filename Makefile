include config.mk

edit: *.c config.h languages.h termbox.h
	${CC} -o edit edit.c

config.h: config.def.h
	cp config.def.h $@

clean:
	rm -f edit edit-*.tar.gz

dist: clean
	tar -cf edit-${VERSION}.tar LICENSE Makefile readme.md *.md \
	    config.def.h config.mk languages.h termbox.h *.c edit.1
	gzip edit-${VERSION}.tar

install: edit
	mkdir -p ${PREFIX}/bin
	cp -f edit ${PREFIX}/bin
	chmod 755 ${PREFIX}/bin/edit
	mkdir -p ${MANPREFIX}/man1
	sed "s/VERSION/${VERSION}/g" < edit.1 > ${MANPREFIX}/man1/edit.1
	chmod 644 ${MANPREFIX}/man1/edit.1

uninstall:
	rm -f ${PREFIX}/bin/edit ${MANPREFIX}/man1/edit.1

tutor: edit
	cp tutor.def.md tutor.md
	./edit tutor.md
	rm tutor.md

.PHONY: edit clean dist install uninstall tutor
