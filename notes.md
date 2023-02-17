## TODO

### New features

* parse command line arguments
* integrate replace engine
* shift selections when move line, move to clip, insert from clip, insert line, split lines, concatenate lines...
* {split, concatenate} lines
* suppress selections when move to clip
* syntax highlight rules
* manage resizing while in dialog mode
* subpatterns, fields: store bytes length
* delete_character: specify behaviour on start of selection

### Restructurations

* (characters, bytes) struct ?
* gathers variables in good order in globals.h
* internalize termbox ?
* remove all the `go_to(pos_of(...`, just attribute new asked y and x, and execute `go_to(pos_of(x, y))` at start of main loop
* regex: choose a known syntax, link official documentation
* move `get_str_index` to char * so that it can be used elsewhere ?
* suppress settings.language ?
* move all hexadecimal values to `global.h` with a description ?
* put variables where they sould be, and how they should be (extern, const...)
* improve error management, assure safe and graceful fails

### Check

* missing TODO and comments
* correctness, especially
    * search and replace engine (extensivity)
    * {insert, delete} characters    
* look for unstated assumptions
* has_been_changes, anchor, cursor, selections, ... all globals
* possibility to make compile-time arguments
* possibility to make macros
* possibility to make "const" args
* test other compilers
* code cleanness and comments coverage

### Publication

* new colorscheme
* .md files
    * readme
    * philosophy
    * user_guide
    * hacker_guide
* website
    * .md files
    * tarball
    * color palette
* asciinema teaser
* asciinema long presentation
* add some languages ?

### Potential features

* [e] execute command ?
* read-only mode ?
* regex: add word boundaries ?
* store optimal x ?
* scrolloff ?
* [langage] auto-indenting ?

* display line numbers ??
* tab-completion ??

* visual wrap of long lines ???
* inline clipboard ???
* macros ???


## Ressources

### Features/bindings and philosophy

* [xi](https://xi-editor.io/docs.html)
* [vim](https://vimhelp.org/vi_diff.txt.html)
* [notepad](https://jsimlo.sk/notepad/features.php)
* [ted](http://www.kpdus.com/ted.html)
* [kakoune philosophy](https://kakoune.org/why-kakoune/why-kakoune.html)
* [kakoune](https://github.com/mawww/kakoune#advanced-topics)
* [notepad++](https://github.com/notepad-plus-plus/notepad-plus-plus/wiki/Features)
* [sam](http://doc.cat-v.org/plan_9/4th_edition/papers/sam/)
* [sam man](http://man.cat-v.org/plan_9/1/sam)
* [sam refcard](http://sam.cat-v.org/cheatsheet/sam-refcard.pdf)
* [neovim](https://neovim.io/charter/)
* [list](https://github.com/topics/text-editor) (explored for >= 150 stars)

### Regex search

* [regex ECMA](https://262.ecma-international.org/6.0/#sec-patterns)
* [PCRE](https://www.pcre.org/)
* [regexp](https://man.cat-v.org/plan_9/6/regexp)
* [standard](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap09.html)
* [sd](https://github.com/chmln/sd)
* [sed](https://www.gnu.org/software/sed/manual/sed.html)
* [pcre](https://github.com/niklongstone/regular-expression-cheat-sheet)

### Others

* [record screen](https://asciinema.org/)
* [UTF-8](https://en.wikipedia.org/wiki/UTF-8)
* [tree-sitter](https://tree-sitter.github.io/tree-sitter/)
* [style](https://suckless.org/coding_style/)

### Syntax

* [kibi](https://github.com/ilai-deutel/kibi/tree/master/syntax.d)
* [vim](https://github.com/vim/vim/tree/master/runtime/syntax)
* [micro](https://github.com/zyedidia/micro/tree/master/runtime/syntax)
