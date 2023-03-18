## TODO

engine:
    review the process, test everything (classes, last element of group)
    rewrite syntax in cheatsheet.md (| can't follow REP...)
move to a single file ?
manipulating hex values: when are casts needed ?


### Final review

Read, understand, check its correctness, chase unstated assumptions (ASCII only...)

Be critical
    good order of variables ?
    is the goal/use of the function/variable explained enough ?
    is the process clear and non-obvious ? else commented accordingly ?
    is the code is written well-enough ? <= 80 chars ?
    use standard functions with n: strncpy, strncat, strn?cmp
    should this be a get_str_index stuff ?
    should that be moved elsewhere, or replaced by a macro, or e = +- 1 ?
    should this feature be optionnal as a compile-time setting ?
    is this feature/strange behaviour covered in the tutorial and philosophy ?

Extensive testing. Known source of failure:
    check cursor/anchor/selections movements

edit.c
lines.c
movements.c
marks.c
selections.c


### Publication

* finalise readme.md, tutor.def.md, philosophy.md
* finalise config.def.h, including colorscheme
* add languages and finalise languages.md
* final check for each file
* script, do and upload the screencast
* better colors.html
* suppress notes.md, clean makefile
* tarball and upload it all
* publish on github
