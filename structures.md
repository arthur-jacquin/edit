## File organisation

    readme.md           Introduction of the editor, entry point
    license             License of the project (GPLv3)
    makefile            Compilation instructions

    cheatsheet.md       Quick reference
    manual.md           User guide
    philosophy.md       Rationale
    regexps.md          Guide on regular expressions
    syntax_highlight.md Description of the syntax highlighting system
    structures.md       Notes on

    config.def.h        Default configuration
    globals.h           Main header, where most of the declarations are
    termbox.h           A self-contained terminal drawing library

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


## Data structures

lines buffer: doubly linked lists:
- main one with pointers to first line and to first line on screen.
- clipboard

3 selections queues
saved and temps contains actual selections, displayed is just a reference to it
never call forget_sel_list on temp
always forget_sel_list before resetting saved and temp

## How to read the code

## Notes

In the clipboard, line numbers are 0 to clipboard.nb_lines - 1
