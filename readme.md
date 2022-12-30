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

At start: matches are initialised at lines.

SELECTION:
* search for pattern (mark pattern elements, narrow matches)
* keep containing pattern (mark pattern elements)
* ignore (remove match but outputs) or exclude (suppress) containing pattern

ACTIONS:
* change field separator
* display number of matches
* replace with pattern elements and fields
* switch to uppercase/lowercase
* insert character at given position
* remove character at given position

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

## Roadmap

* add edition engine (1 cursor, LIFO of selections)
* add UTF-8 support

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

echo "file %% opened (q to exit)"

FILE MANAGEMENT:
[q] quit; if unsaved changes: DIALOG single "qw"
    [q] quit without save
    [w] save and quit
[w] write; if truncations: DIALOG single "Ww"
    [W] write as: see below
    [w] write
[W] write as: if (DIALOG prompt "File name: ") {change name; write;}
[R] reload

PARAMETER MANAGEMENT:
[S] set parameter: if (DIALOG prompt "Parameter modification: ") modif (echoes if not successful)

CURSOR MANAGEMENT:
[ ] associated brackets
[ / ] start/end of line
[g] go to line: DIALOG prompt "Go to line: " (g -> start of file)
[G] go to end of file
[n/N] <n> next/previous match
[ / ] <n> next/previous character
[ / ] <n> next/previous word
[ / ] <n> next/previous line
[ / ] <n> next/previous block

GET IN INSERT MODE:
[i] before cursor
TODO ...

IN INSERT:
    echo "INSERT (ESC to exit)"
    [ESC] get out of INSERT
    [else] write char

LINE MANAGEMENT:
[ ] yank <n> lines
[ ] delete <n> lines
[ / ] paste after/before <n> times
[ ] duplicate (<n>) lines
[ / ] move (<n>) lines up/down (<m>) lines

''' EDITION ENGINE RELATED

SELECTIONS MANAGEMENT:
[ESC] select only 0 chars at cursor
    mark all lines of custom range (like sed)
[%] select all lines of files                       (no filter)
[b] select all lines of paragraph                   (no filter)
TODO
    what is deselected ?
    create/delete selections
    selections on search
    selections by movement

As a filter: select all lines at start

SELECTION:
[s] search for pattern (mark pattern elements, narrow matches)
[ ] keep containing pattern (mark pattern elements)
[ ] ignore (remove match but outputs) containing pattern
[ ] exclude (suppress) containing pattern

ACTIONS:
[ ] change field separator
[ ] display number of matches
[ ] replace with pattern elements and fields (MATCH)
[u/U] switch to lowercase/lowercase (MATCH) 
[ ] insert character(s) before (MATCH)
[ ] remove <n> first characters (MATCH)
[ / ] increase/decrease indent (LINE)
[ / ] comment/uncomment (LINE)

for LINE actions, extends selections to lines before acting
'''

IN DIALOG single(const char * prompt, const char * specifiedchars):
    echo prompt
    [ESC] return 0
    [specifiedchars] return char

IN DIALOG prompt(const char * prompt, char **buf, int nbcharmax):
    echo prompt
    [ESC] return 0
    [ENTER] return 1
    [else] write to buf, print after prompt


### Registers (old)

OPTIONS:
* autoindent
* syntax_highlight
* highlight_matches
* case_sensitive
* use_tabs
* cursor_line
* is_anchored

VARIABLES:
* field_separator
* tab_width
* scrolloff (number of screen lines to keep above and below the cursor)
* langage
* state (with precedent)
* anchor, cursor (with precedent) position
* window size
* search pattern (with precedent)
* clipboard (history ?)
* macro (with precedent)
* command (with precedent)
* 9 registers for subpattern in regex

MATCHES LIST
