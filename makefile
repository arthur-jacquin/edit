binary: *.c *.h
	gcc -o binary edit.c

config.h: config.def.h
	cp config.def.h config.h

test: binary
	./binary ~/input.c

count: *.c
	cloc actions.c edit.c file.c graphical.c interaction.c lines.c movements.c \
		search_and_replace.c selections.c utils.c
	wc actions.c edit.c file.c graphical.c interaction.c lines.c movements.c \
		search_and_replace.c selections.c utils.c
