## TODO

Linear development, commits quite regularly, pushs on version bumps:

    bump to v0.1.0: initial commit, documentation
    bump to v0.2.0: support for more languages, better colorscheme

Better management of `scroll_offset` on start of file, by defining a potentially
negative `first_line_nb` variable ?

Ask Thomas about string storing


### Adding/rethinking

* make sure each file is explained (tests.h...)
* correct syntax: | can't follow REP


### Final rewrite

Progressively integrate each part. At each step:

    exhibit which globals each functions modifies
    use standard functions with n: strncpy, strncat, strn?cmp
    should that be moved elsewhere, or replaced by a macro, or e = +- 1 ?
    should this feature be optionnal as a compile-time setting ?
    is this feature covered in manual/... ?
    are tabs and UTF-8 errors detected and treated (exit, fprintf) ?
    is this line numbers compliant ?
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
        add line at end of file ?
        attributing x without flagging attribute_x
        is_word_char is only ASCII
        break_line
        concatenate_line
        manipulating hex values: cast needed ?
    is echoing interface is well exploited ?
    sould this be a get_str_index stuff ?
    potentially append to the "hot stuff to promote" list
    potentially append to the "this isn't perfect" list

Build a dependency graph of functions. Correlate that with list of modified
globals.


### Publication

* new colorscheme
* strong defaults
* add languages
* .md files
* tarball
* [asciinema](https://asciinema.org)
* website
    * .md files
    * tarball
    * color palette
