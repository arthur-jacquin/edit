# edit

Simple, featured, modal text editor.

## Goals

* produce a standalone executable
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

* does not highlight multi lines strings/comments.
* does not work well with very big files.
* does not visually wrap lines.
* does not work with tabs.

## Features

* UTF-8 support
* read-only mode (can act as a pager)
* modal editor: edit, insert, dialog modes
* some syntax highlight
* advanced navigation
* mouse support
* advanced line management
* search and replace engine

Maybe in a far future:
* scrolloff ?
* works with tab ?
* [langage] auto-indenting ?
* visual wrap of long lines ??
* tab-completion ??
* inline clipboard ??
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
     SHIFT+arrows   move cursor line <n> lines up/down

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
                b   select all lines of <n> block
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


## Search and replace

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
    rt          replace_tabs            bool    yes
    fs          field_separator         char    ,
    tw          tab_width               int     4
    l           language                string  (from extension)


## Non-runtime-modifiable parameters

You can modify settings in config.h, such as:
- mouse support, scroll line number...
- default values for parameters
- 8 or 256 colors modes, colorscheme
- keybinds
- ...

If there is no config.h at compile time, a default configuration is loaded
from config.def.h


## Thanks

* my brother for beta-testing and comments
* [termbox2](https://github.com/termbox/termbox2) terminal rendering library


## Ressources

Features/bindings and philosophy:
* [vi](http://www.ungerhu.com/jxh/vi.html)
* [vim](https://vimhelp.org/vi_diff.txt.html)
* [notepad](https://jsimlo.sk/notepad/features.php)
* [ted](http://www.kpdus.com/ted.html)
* [kakoune philosophy](https://kakoune.org/why-kakoune/why-kakoune.html)
* [kakoune](https://github.com/mawww/kakoune#advanced-topics)
* [xi](https://xi-editor.io/docs.html)
* [xi ropes tree](https://xi-editor.io/docs/crdt-details.html)
* [notepad++](https://github.com/notepad-plus-plus/notepad-plus-plus/wiki/Features)
* [mle](https://github.com/adsr/mle)
* [sam](http://doc.cat-v.org/plan_9/4th_edition/papers/sam/)
* [sam man](http://man.cat-v.org/plan_9/1/sam)
* [sam refcard](http://sam.cat-v.org/cheatsheet/sam-refcard.pdf)

Regex search:
* [sd](https://github.com/chmln/sd)
* [sed](https://www.gnu.org/software/sed/manual/sed.html)

Documentation:
* [UTF-8](https://en.wikipedia.org/wiki/UTF-8)
