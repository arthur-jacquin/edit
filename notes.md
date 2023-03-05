## TODO

### Adding/rethinking

* store optimal cursor column ?
* `e`, command_int, background ? default command by language ? always 'make' ?
* best way to store strings ?
* search and replace
    names of variables in engine
    detect word boundaries, start/end of selection
* `*` command


### Final rewrite

Progressively integrate each part. At each step:

    exhibit which globals each functions modifies
    use standard functions with n: strncpy, strncat, strn?cmp
    should that be moved elsewhere, or replaced by a macro, or e = +- 1 ?
    should this feature be optionnal as a compile-time setting ?
    is this feature covered in manual/... ?
    are tabs and UTF-8 errors detected and treated (exit, fprintf) ?
    check correctness by hand
    look at unstated assumptions
        ASCII only ?
    document
        code cleanness, <= 80 chars
        comments coverage
        variables names, use (are they even used ?)
        explain non-obvious behaviours, assumptions
    look at existing TODO, XXX and any comment
    extensive testing. Known source of failure:
        inserting '
        break_line
        concatenate_line
        manipulating hex values: cast needed ?
        \d matches d
        multiple slash on same line
    potentially append to the "hot stuff to promote" list
    potentially append to the "this isn't perfect" list

Build a dependency graph of functions. Correlate that with list of modified
globals.


### Publication

* new colorscheme
* add languages
* .md files
* tarball
* [asciinema](https://asciinema.org)
* website
    * .md files
    * tarball
    * color palette
