## TODO

### Restructurations

* remove deps from termbox structures
* verify if no tabs and UTF-8 compliant ?
* break_line, concatenate_line 
* clarify functions deps, restructurate in which file goes which functions
* use strncat and other stdlib functions (ctype.h, string.h...)
* (characters, bytes) struct ?
* gathers variables in good order in globals.h
* internalize termbox ?
* remove all the `go_to(pos_of(...`, just attribute new asked y and x, and execute `go_to(pos_of(x, y))` at start of main loop
* regex: choose a known syntax, link official documentation
* put variables where they sould be, and how they should be (extern, const...)
* improve error management, assure safe and graceful fails
* moving stuff (has_been_changes, reset_selections..) to load_file 
* use ifdef and macros

### Check

* missing TODO and comments
* correctness, especially
    * rules
    * resize in dialog mode
    * what happens when multiple slash on the same line
    * insert characters like ' on line created by `insert_line`
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

* piping ?
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

* [regexp](https://man.cat-v.org/plan_9/6/regexp)
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


# Others

How to add a language.

## File organisation

    readme.md           Introduction of the editor, entry point
    license             License of the project (GPLv3)
    makefile            Compilation instructions

    philosophy.md
    manual.md           A user guide
    cheatsheet.md       A quick reference
    syntax_highlight.md
    search_and replace.md
    structures.md
    ...

    config.def.h        Default configuration
    globals.h           Main header, where most of the declarations are
    termbox.h           A self-contained TUI library

    actions.c
    edit.c
    file.c
    graphical.c
    interaction.c
    lines.c
    movements.c
    search_and_replace.c
    selections.c
    utils.c

    tests.c             Unit testing for search and replace engine


## Notes

In the clipboard, line numbers are 0 to clipboard.nb_lines - 1

lines buffer: doubly linked lists:
- main one with pointers to first line and to first line on screen.
- clipboard

3 selections queues
saved and temps contains actual selections, displayed is just a reference to it
never call forget_sel_list on temp
always forget_sel_list before resetting saved and temp

1. FILE (supposedly UTF-8)
2. LINES BUFFER (double linked list of UTF-8 strings)
3. TERMBOX BUFFER
4. SCREEN

    1>2: file.c: load_file
    1<2: file.c: write_file
    2>3: graphical.c: print_all
    3>4: termbox.h: tb_present

