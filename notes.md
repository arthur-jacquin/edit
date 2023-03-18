## TODO

engine:
    review the process, test everything (classes, last element of group)
    rewrite syntax in cheatsheet.md (| can't follow REP...)
add jump to to *.md


### Final review

Read, understand, check its correctness, chase unstated assumptions (ASCII only...)

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

actions.c
config.def.h
edit.c
file.c
globals.h
print_line in graphical.c
interaction.c
languages.h
lines.c
movements.c
search_and_replace.c
selections.c
termbox.h
utils.c

### Publication

* finalise readme.md, tutor.def.md, philosophy.md
* strong config.def.h, including colorscheme
* add languages
* final check for each file
* script, do and upload the screencast
* better colors.html
* suppress notes.md, clean makefile
* tarball and upload it all
* publish on github
