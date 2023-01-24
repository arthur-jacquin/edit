FIX:
* problem with selections
* move cursor and selections on actions/line management
* new line at end of file

BIG STUFF:
* change to GPL
* restructure everything
* unified struct for settings with previous saved
* integrate search and replace engine
* add shift selections
* add read-only mode
* add UTF-8 support
* better help
* find matching bracket
* highlight matching bracket 
* find next/previous word
* find next/previous selection
* duplicates selections 
* concatenate lines on SUPPR/BACKSPACE

CHECK:
* correctness of search engine
* unstated assumptions
* n-extensivity
* has_been_changes
* possibility to make "const" args

MISSING KEYBINDS:
* [m] move to matching bracket
* [t/T] move to <n> next/previous word
* [n/N] move to <n> next/previous selection
* [z] duplicate the current selection on the next <n> lines
* [f,/] search for pattern
* [R] replace with pattern elements and fields

LAST:
* improve error management, assure safe and graceful fails
* restructuring, cleaning, commenting code
* documentation
* publish

LATER:
* [e] execute command ? make ?
* regex: add word boundaries ?
* store optimal x ?
* scrolloff ?
* display line number ?
* [langage] auto-indenting ?
* visual wrap of long lines ??
* tab-completion ??
* inline clipboard ??
* macros ???
