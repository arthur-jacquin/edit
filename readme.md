# edit

Simple, featured, modal text editor.

## Uses

Ships with both text editing engine and visual client.

* can be used as a filter, using only the edition engine.
* modal visual client, with insert mode, edition mode and tight integration of the edition engine.
* read-only mode (act as a pager).

## Goals

* simplicity, predictability, efficiency
* suckless (small, idiomatic codebase, no runtime parameter...)
* <= 3000 SLOC
* no deps

## Opiniated choices

* *selection then action* philosophy
* no undo/redo: the save/reload/quit philosophy...

## Limitations

Most of these are not considered as a problem.

* does not work well with very big files.
* does not visually wrap lines.
* truncate each lines at MAX_CHARS characters.

## Features

### Edition engine

... see commands

\\ (backslash), \$ (dollar sign)
\0 (whole pattern), \1 .. \9 (subpattern in regex)
$0 (whole line), $1 .. $9 (fields)
$c (inline_clipboard) ??

### Client

* modal editor: edit, insert, dialog modes
* syntax highlight (for some langages)
* advanced navigation
* mouse support (can be disabled at compile time)
* advanced line management
* incremental view of what engine will do
* client-server transactionnal architecture: issues edition commands to engine

Maybe in a far future:
* multi-cursor/collaborative mode ?
* [langage] auto-indenting ?
* load other file in current file ?
* tab-completion ??
* inline clipboard ??
* macros ???
* [langage] structural completion, reformating ???
* visual wrap of long lines ???

## Commands

    FILE MANAGEMENT
    
        q   quit
        w   write
        W   write as
        r   reload

    MODES MANAGEMENT

    <ESC>   return to default mode
        i   get in insert mode

    PARAMETERS MANAGEMENTS
        S   change a parameter

## Roadmap

* add syntax highlight
* add edition engine (1 cursor, LIFO of selections)
* add UTF-8 support
* add long line support

## Thanks

* [termbox2](https://github.com/termbox/termbox2) terminal rendering library


---

## Ressources

### Inspiration

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

Syntax highlighting:
* [latex](https://denbeke.be/blog/programming/syntax-highlighting-in-latex/)
* [src-highlite](https://www.gnu.org/software/src-highlite/)

Documentation:
* [UTF-8](https://en.wikipedia.org/wiki/UTF-8)


### Keybinds

''' LINE MANAGEMENT:
[ ] yank <n> lines
[ ] delete <n> lines
[ / ] paste after/before <n> times
[ ] duplicate (<n>) lines
[ / ] move (<n>) lines up/down (<m>) lines
'''

''' EDITION ENGINE
When multiples matches: what appends on move ? keep selection ? move all cursors ?

CURSOR MANAGEMENT:
[ ] anchor
[ ] associated brackets (OK for multiples)
[ / ] start/end of line (OK for multiples)
[g] go to line: DIALOG prompt "Go to line: " (g -> start of file)
[G] go to end of file
[n/N] <n> next/previous match
[ / ] <n> next/previous character (OK for multiples)
[ / ] <n> next/previous word (OK for multiples)
[ / ] <n> next/previous line ()
[ / ] <n> next/previous block

SELECTION:
    mark all lines of custom range (like sed)       (OUTPUTS LINES)
[s] search for pattern                              (OUTPUTS MATCHES)
[ ] keep containing pattern                         (OUTPUTS LINES)
[ ] ignore (print to stdout) containing pattern     (OUTPUTS LINES)
[ ] suppress containing pattern                     (OUTPUTS LINES)
[.] MATCHES TO LINES MODES                          (OUTPUTS LINES)
(in interactive only)
[ ] select only 0 chars at cursor                   (OUTPUTS MATCHES)
    create/delete matches, anchor, column editing...(OUTPUTS MATCHES)
[%] select all lines of files                       (OUTPUTS LINES)
[b] select all lines of current paragraph           (OUTPUTS LINES)

ACTIONS:
[ ] change field separator
[ ] display number of matches
[ ] replace with pattern elements and fields (MATCH)
[u/U] switch to lowercase/uppercase (MATCH) 
[ / ] insert character before/after (MATCH)
[ / ] remove (/and move match) selection/first character (MATCH)
[ / ] increase/decrease indent (LINE)
[ / ] comment/uncomment (LINE)

for LINE actions, extends selections to lines before acting
'''


### Registers (in far future)

OPTIONS:
* autoindent
* syntax_highlight
* highlight_matches
* case_sensitive
* use_tabs
* is_anchored

VARIABLES:
* field_separator
* tab_width
* langage
* anchor, cursor (with precedent) position
* search pattern (with precedent)
* clipboard (history ?)
* macro (with precedent)
* command (with precedent)
* 9 registers for subpattern in regex

MATCHES LIST
