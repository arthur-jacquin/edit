include config.mk

edit: *.c config.h languages.h termbox.h
	${CC} -o edit edit.c

config.h: config.def.h
	cp config.def.h $@

clean:
	rm -f edit edit-*.tar.gz

dist: clean
	tar -cf edit-${VERSION}.tar LICENSE Makefile readme.md *.md \
		config.def.h config.mk languages.h termbox.h *.c
	gzip edit-${VERSION}.tar

install: edit
	mkdir -p ${PREFIX}/bin
	cp -f edit ${PREFIX}/bin
	chmod 755 ${PREFIX}/bin/edit

uninstall:
	rm -f ${PREFIX}/bin/edit

tutor: edit
	cp tutor.def.md tutor.md
	./edit tutor.md
	rm tutor.md

.PHONY: edit clean dist install uninstall tutor
