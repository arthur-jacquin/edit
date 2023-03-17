## TODO

engine:
    review the process, test everything (classes, last element of group)
    rewrite syntax in cheatsheet.md (| can't follow REP...)


### Final review

Read
    build a dependency graph of functions
    exhibit which globals each functions modifies
    understand the process
    check its correctness, chase unstated assumptions (ASCII only...)

Be critical
    is the goal/use of the function/variable explained enough ?
    is the process clear and non-obvious ? else commented accordingly ?
    is the code is written well-enough ? <= 80 chars ?
    is this feature covered in the tutorial and the philosophy ?
    use standard functions with n: strncpy, strncat, strn?cmp
    should this be a get_str_index stuff ?
    should that be moved elsewhere, or replaced by a macro, or e = +- 1 ?
    should this feature be optionnal as a compile-time setting ?
    should that be warned in philosophy.md and tutorial ?

Extensive testing. Known source of failure:
    file access error not detected/badly handled ?
    is_word_char is only ASCII
    check cursor/anchor/selections movements
    manipulating hex values: cast needed ?


### Publication

* strong config.def.h, including colorscheme
* add languages
* [asciinema](https://asciinema.org)
* finalise readme.md, tutor.def.md, philosophy.md
* suppress notes.md
* website
    * index.html
    * tarball
    * color palette
