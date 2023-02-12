TODO:
- continue migration
- better uppercase/lowercase
- move cursor line, highlight matching bracket... as compile-time settings
- syntax highlight rules
- case sensitive
- read-only mode
- gathers variables in good order in globals.h

internalize termbox
better move when an index is already known 
Remove all the go_to(pos_of(..., just attribute new asked y and x, and go_to(pos_of(x, y)) at start of main loop
TODO for move after indent: check difference of l->ml
search and replace with UTF 8 ?
UTF-8: move everything to a expand - process - compress model ?
4 * INTERFACE_WIDTH ?
int resize_to_be_treated ?
remove masks ?

For now
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


KEEP IN MIND:
* UTF-8
* move at good position

BIG STUFF:
* rewrite all the selections stuff
    * add shift selections
    * find next/previous selection
    * duplicates selections
* rewrite all the actions
* integrate search and replace engine
* make search and replace engine UTF-8 support
* concatenate lines on SUPPR/BACKSPACE

CHECK:
* where best to put variables ? extern/const...
* correctness of search engine
* unstated assumptions
* has_been_changes
* possibility to make "const" args

MISSING KEYBINDS:
* [n/N] move to <n> next/previous selection
* [z] duplicate the current selection on the next <n> lines
* [f,/] search for pattern
* [R] replace with pattern elements and fields

LAST:
* better help
* improve error management, assure safe and graceful fails
* restructuring, cleaning, commenting code
* documentation
* publish under GPL

LATER:
* [e] execute command ? make ?
* read-only mode ?
* regex: add word boundaries ?
* store optimal x ?
* scrolloff ?
* display line number ?
* [langage] auto-indenting ?
* visual wrap of long lines ??
* tab-completion ??
* inline clipboard ??
* macros ???
