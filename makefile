.DEFAULT: build

build: *.c *.h config.h
	tcc -static -o edit edit.c

install: build
	cp edit /usr/bin

config.h: config.def.h
	cp config.def.h config.h

tutor: build
	cp tutor.def.md tutor.md
	./edit tutor.md

# lol

test: build
	./edit ~/input.c

count:
	cloc *.h *.c
	file ./edit
	du -h ./edit
