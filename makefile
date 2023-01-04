config.h: config.def.h
	cp config.def.h config.h

binary: edit.c config.h termbox.h
	gcc -o binary edit.c

test: binary
	./binary input > log.txt
