FIX:
* problem with selections
* move cursor and selections on actions/line management
* new line at end of file

BIG STUFF:
* rewrite all the selections stuff
* add shift selections
* find next/previous selection
* integrate search and replace engine
* add UTF-8 support
* duplicates selections
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
* < and > as brackets ?
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
