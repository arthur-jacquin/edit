Remove all the go_to(pos_of(..., just attribute new asked y and x, and go_to(pos_of(x, y)) at start of main loop
check regex.h
internalize termbox
initiate interfaces ?



Pour l'instant:
1. FILE (supposedly UTF-8)
2. LINES BUFFER (double linked list of UTF-8 strings)
3. TERMBOX BUFFER
4. SCREEN

    1>2: file.c: load_file
    1<2: file.c: write_file
    2>3: graphical.c: print_all
    3>4: termbox.h: tb_present

Maybe in the future
1. FILE (supposedly UTF-8)
2. LINES BUFFER (double linked list of UTF-8 strings)
3. GRAPHICAL BUFFER (array of (Unicode, fg, bg) arrays) (as doubled linked list ?)
4. SCREEN

dialog: unicode
