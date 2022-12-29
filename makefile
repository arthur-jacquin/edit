edit: edit.c termbox.h
	gcc -o edit edit.c

test: edit
	./edit input > log.txt
