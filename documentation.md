# Documentation

## Default keybinds

### General

            <ESC>   return to default mode
                ?   help
              q/Q   quit/force quit
              w/W   write/write as
                r   reload
                i   get in insert mode
                s   change a parameter

### Other ways to get insert mode (after removing selections)

              I/A   at start/end of line
              o/O   on an empty line created below/above cursor line

### Manage lines

              y/Y   yank <n> lines/blocks
              d/D   delete <n> lines/blocks
              p/P   paste after/before <n> times
     SHIFT+arrows   move cursor line <n> lines up/down

### Move around

                m   matching bracket
            0/^/$   start/first non-blank character/end of line
              g/G   go to line <n>/end of file
      arrows, l/h   <n> next/previous character
      arrows, j/k   <n> next/previous line
              t/T   <n> next/previous word
              {/}   <n> next/previous block
              n/N   <n> next/previous match

### Manage selections

                c   display number of saved selections
            <ESC>   forget saved selections, unanchor
                .   select cursor line
            %/b/:   select all lines of file/<n> following blocks/custom range
             f, /   search for pattern
                v   anchor/unanchor
                a   push running selection to pile; unanchor
                z   duplicate the current selection on the <n> following lines

### Act on selections

              </>   <n> decrease/increase line indent
                K   comment/uncomment line
                x   suppress
                R   replace with pattern elements and fields
              u/U   switch to lowercase/uppercase


## Search and replace

TODO: choose a known syntax, link official documentation

    SEARCH
        characters
        predefined classes: . (any), \w, \W (word, non word), \d, \D (digit, non digit)
        markers: ^, $ (start, end of selection)
        quantifiers: *, +, ?, {n}, {n,}, {,n}, {n,m}
        delimiters: \(...\)
        custom classes: [abc], [^abc], [0-9], [-6], [^-a-z#A-Z0-9], ...
        logic: |

    REPLACE
        chars
        \\ (backslash), \$ (dollar sign)
        \0 (whole pattern), \1 .. \9 (subpatterns in regex)
        $0 (whole selection), $1 .. $9 (fields)


## Runtime-modifiable parameters

    SHORT NAME  LONG NAME               TYPE    DEFAULT
    i           autoindent              bool    yes
    sh          syntax_highlight        bool    yes
    h           highlight_selections    bool    yes
    c           case_sensitive          bool    yes
    fs          field_separator         char    ,
    tw          tab_width               int     4
    l           language                string  (from extension)


## File organisation

    readme.md           Introduction of the editor
    manual.md           A user guide
    documentation.md    This file
    license             License of the project (GPLv3)
    makefile            Compilation instructions

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

3 selections queues
saved and temps contains actual selections, displayed is just a reference to it
never call forget_sel_list on temp
always forget_sel_list before resetting saved and temp
