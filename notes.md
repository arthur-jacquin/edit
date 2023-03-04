## TODO

* restructure error management
    * protect against tabs and UTF-8 errors
    * protect against malloc failings
* detect word boundaries, start/end of selection, `*` command
* add potential features ?

### Restructurations

* understand type casting
* use e = +- 1 for grouping commands
* break_line, concatenate_line
* clarify functions deps, restructurate in which file goes which functions
* use strncat and other stdlib functions (ctype.h, string.h...)
* (characters, bytes) struct ?
* gathers variables in good order in globals.h
* remove unwrap_pos
* put variables where they sould be, and how they should be (extern, const...)
* use macros

### Check

* missing TODO and comments
* correctness, especially
    * rules
    * resize in dialog mode
    * what happens when multiple slash on the same line
    * insert characters like ' at end of line
    * search and replace engine, memory indexes
    * `replace_characters`
* look for unstated assumptions
* selections, has_been_changes, anchor, cursor, selections, ... all globals
* recurrent expr that should be replaced with a macro
* possibility to make compile-time arguments
* possibility to make "const" args
* test other compilers
* code cleanness and comments coverage

### Publication

* new colorscheme
* .md files
* website
    * .md files
    * tarball
    * color palette
* asciinema teaser
* add some languages ?


## Ressources

* [regexp man page](https://man.cat-v.org/plan_9/6/regexp)
* [pcre](https://github.com/niklongstone/regular-expression-cheat-sheet)
* [UTF-8](https://en.wikipedia.org/wiki/UTF-8)
* [record screen](https://asciinema.org/)
* [style](https://suckless.org/coding_style/)
