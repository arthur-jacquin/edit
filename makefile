binary: *.c *.h
	tcc -static -o binary edit.c

config.h: config.def.h
	cp config.def.h config.h

test: binary
	./binary ~/input.c

tutor: binary
	cp tutor.def.md tutor.md
	./binary tutor.md

count:
	cloc *.h *.c
	file ./binary
	du -h ./binary
