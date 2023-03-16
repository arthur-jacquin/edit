## TODO

engine:
    review the process, test everything (classes, last element of group)
    rewrite syntax (| can't follow REP...)

add autoindent in break_line and insert_line
delete automatically chooses to untab


### Final rewrite

Read
    exhibit which globals each functions modifies
    understand the process
    check its correctness, chase unstated assumptions (ASCII only...)

Be critical
    is the goal/use of the function/variable explained enough ?
    is the process clear and non-obvious ? else commented accordingly ?
    is the code is written well-enough ? <= 80 chars ?
    is this feature covered in the tutorial and the philosophy ?
    is echoing interface is well exploited ? should attribute_x ?
    use standard functions with n: strncpy, strncat, strn?cmp
    should this be a get_str_index stuff ?
    should that be moved elsewhere, or replaced by a macro, or e = +- 1 ?
    should this feature be optionnal as a compile-time setting ?

Extensive testing. Known source of failure:
    file access error not detected/badly handled ?
    is_word_char is only ASCII
    check cursor/anchor/selections movements
    manipulating hex values: cast needed ?

Potentially append to warnings in philosophy.md

Build a dependency graph of functions. Correlate that with list of modified
globals.


### Publication

* new colorscheme
* strong defaults
* add languages
* suppress notes.md
* [asciinema](https://asciinema.org)
* website
    * index.html
    * tarball
    * color palette
