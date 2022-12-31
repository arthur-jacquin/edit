binary: edit.c termbox.h
	gcc -o binary edit.c

test: binary
	./binary input > log.txt
