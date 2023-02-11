binary: *.c *.h
	gcc -o binary edit.c

config.h: config.def.h
	cp config.def.h config.h

test: binary
	./binary old/input.c
