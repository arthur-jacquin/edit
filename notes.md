## TODO

correct syntax: | can't follow REP


### Final rewrite

Progressively integrate each part. At each step:

1. Read
    exhibit which globals each functions modifies
    understand the process
    check its correctness, chase unstated assumptions (ASCII only...)

2. Treat TODO and XXX

3. Be critical
    is the name and goal of the file/function/variable explained enough ?
    is the process clear and non-obvious ? else commented accordingly ?
    is the code is written well-enough ? <= 80 chars ?
    is this feature covered in the manual, the philosophy or the documentation ?
    is echoing interface is well exploited ?
    use standard functions with n: strncpy, strncat, strn?cmp
    sould this be a get_str_index stuff ?
    should that be moved elsewhere, or replaced by a macro, or e = +- 1 ?
    should this feature be optionnal as a compile-time setting ?

4. Extensive testing. Known source of failure:
    file access error not detected/badly handled ?
    attributing x without flagging attribute_x
    is_word_char is only ASCII
    break_line
    concatenate_line
    manipulating hex values: cast needed ?

5. Potentially append to the following lists:
    "special behaviour: users should be warned of it and given an explanation"
    "hot stuff to promote"
    "not perfect stuff"

Build a dependency graph of functions. Correlate that with list of modified
globals.


### Warnings

tabs are converted in spaces on loading the document
regex engine is not perfect: please send erroneous behaviours

### Hot stuff

### Not perfect stuff

move colorscheme to other file ? Real colors support ?
gruvbox, dracula, one light/dark, solarized, nord, monokai


### Publication

* new colorscheme
* strong defaults
* add languages
* .md files
    * make tutorial
* tarball
* [asciinema](https://asciinema.org)
* website
    * .md files
    * tarball
    * color palette
