TODO:
* integrate search and replace engine
* concatenate lines on SUPPR/BACKSPACE
* shift selections when move line, move to clip, insert from clip, insert line,
    split lines, concatenate lines...
* {split, concatenate} lines
* suppress selections when move to clip
* int resize_to_be_treated ?
* fix insert/delete in dialog

- syntax highlight rules
- case sensitive
- read-only mode
- gathers variables in good order in globals.h

internalize termbox
better move when an index is already known
Remove all the go_to(pos_of(..., just attribute new asked y and x, and go_to(pos_of(x, y)) at start of main loop
regex: choose a known syntax, link official documentation
move get_str_index to char * so that it can be used elsewhere (in interaction.c) ?
suppress settings.language ?
move all hexadecimal values to global with a description ?

CHECK:
* actions
* edit interf->current in interaction.c
* field detection: backslash resistant ?
* {insert, delete} characters
* where best to put variables ? extern/const...
* correctness of search engine (extensivity)
* unstated assumptions
* has_been_changes, anchor, cursor, selections
* possibility to make compile-time arguments
* possibility to make "const" args

MISSING KEYBINDS:
* search for pattern
* replace with pattern elements and fields

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
