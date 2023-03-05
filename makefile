binary: *.c *.h
	tcc -o binary edit.c

config.h: config.def.h
	cp config.def.h config.h

test: binary
	./binary ~/input.c

count:
	cloc *.h *.c
	du -h ./binary
