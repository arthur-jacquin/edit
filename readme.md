# edit

Simple, featured, modal text editor.

## TODO

* change data representation

* load, save file
* display file, basic movements
* add dialog mode, with click, arrows, return

* add advanced movements
* add selections-oriented engine
* add insert mode
* add clipboard management
* add read-only mode
* add advanced modification tools
* add syntax highlight
* add UTF-8 support
* add long line support
* add search and replace engine

## Goals

* simplicity, predictability, efficiency, orthogonality
* sane defaults, in config.def.h
* suckless (small, idiomatic codebase, no runtime parameter...)
* <= 3000 SLOC
* no deps

## Opiniated choices

* *selection then action* philosophy.
* no undo/redo: the save/reload/quit philosophy...
* does not visually wrap lines.

## Limitations

* does not work well with very big files.
* does not visually wrap lines.

## Features

* UTF-8 support
* read-only mode (can act as a pager)
* modal editor: edit, insert, dialog modes
* syntax highlight (for some langages)
* advanced navigation
* mouse support
* advanced line management
* search and replace engine

Maybe in a far future:
* scrolloff ?
* [langage] auto-indenting ?
* load other file in current file ?
* tab-completion ??
* inline clipboard ??
* visual wrap of long lines ???
* macros ???

## Default keybinds

    GENERAL
            <ESC>   return to default mode
                ?   help
                q   quit
                w   write
                W   write as
                r   reload
                i   get in insert mode
                s   change a parameter

    INSERT MODE (after removing selections)
              I/A   at start/end of line
              o/O   on an empty line created below/above cursor line

    LINES
              y/Y   yank <n> lines/blocks
              d/D   delete <n> lines/blocks
              p/P   paste after/before <n> times
    CTRL + arrows   move cursor line <n> lines up/down

    MOVEMENTS
                m   matching bracket
            0/^/$   start/first non-blank character/end of line
              g/G   go to line <n>/end of file
      arrows, l/h   <n> next/previous character
      arrows, j/k   <n> next/previous line
              t/T   <n> next/previous word
              {/}   <n> next/previous block
              n/N   <n> next/previous match

    SELECTIONS
                c   display number of selections
            <ESC>   reset to only one selection, at cursor, of length 0
                .   select cursor line
                :   select all lines of custom range
                %   select all lines of files
                b   select all lines of current block
             f, /   search for pattern
                v   anchor/unanchor
                a   push running selection to pile; unanchor
                z   duplicate the current selection on the next <n> lines

    ACTIONS ON SELECTIONS
              </>   <n> decrease/increase line indent
                K   comment/uncomment line
                x   suppress
                R   replace with pattern elements and fields
              u/U   switch to lowercase/uppercase


## TODO Search and replace specials
    
    [-a-z#A-Z0-9], [^...]
    \( ... \) mark a subpattern

    \\ (backslash), \$ (dollar sign)
    \0 (whole pattern), \1 .. \9 (subpattern in regex)
    $1 .. $9 (fields)

## Runtime-modifiable parameters

    SHORT NAME  LONG NAME               TYPE    DEFAULT
    i           autoindent              bool    yes
    sh          syntax_highlight        bool    yes
    h           highlight_selections    bool    yes
    c           case_sensitive          bool    yes
    rt          replace_tabs            bool    yes
    fs          field_separator         char    ,
    tw          tab_width               int     4
    l           language                string  (from extension)

## Non-runtime-modifiable parameters

You can modify settings in config.h, such as:
- mouse support, scroll line number...
- default values for parameters
- colors
- keybinds
- ...

If there is no config.h at compile time, a default configuration is loaded
from config.def.h

## Thanks

* [termbox2](https://github.com/termbox/termbox2) terminal rendering library




---

## Ressources

Features/bindings and philosophy:
* [vi](http://www.ungerhu.com/jxh/vi.html)
* [vim](https://vimhelp.org/vi_diff.txt.html)
* [notepad](https://jsimlo.sk/notepad/features.php)
* [ted](http://www.kpdus.com/ted.html)
* [kakoune philosophy](https://kakoune.org/why-kakoune/why-kakoune.html)
* [kakoune](https://github.com/mawww/kakoune#advanced-topics)
* [xi](https://xi-editor.io/docs.html)
* [notepad++](https://github.com/notepad-plus-plus/notepad-plus-plus/wiki/Features)
* [mle](https://github.com/adsr/mle)
* [sam](http://doc.cat-v.org/plan_9/4th_edition/papers/sam/)
* [sam man](http://man.cat-v.org/plan_9/1/sam)
* [sam refcard](http://sam.cat-v.org/cheatsheet/sam-refcard.pdf)

File representation:
* [xi ropes tree](https://xi-editor.io/docs/crdt-details.html)

Edition engine:
* [sd](https://github.com/chmln/sd)
* [sed 1](https://www.gnu.org/software/sed/manual/sed.html)
* [sed 2](https://pubs.opengroup.org/onlinepubs/007904975/utilities/sed.html)
* [sed 3](https://pubs.opengroup.org/onlinepubs/9699919799/)
* [sed 4](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/sed.html)
* [regex](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Guide/Regular_Expressions/Cheatsheet)

Syntax highlighting:
* [latex](https://denbeke.be/blog/programming/syntax-highlighting-in-latex/)
* [src-highlite](https://www.gnu.org/software/src-highlite/)

Documentation:
* [UTF-8](https://en.wikipedia.org/wiki/UTF-8)
* [words delimitation](https://en.wikipedia.org/wiki/Delimiter)

## Infinite loop

* add features
* check correctness of all line fields and variables in any circumstances
* chase unstated assumptions, possibility of failure
* improve error management, assure safe and graceful fails
* restructuring, cleaning, commenting code
* documentation
* publish
