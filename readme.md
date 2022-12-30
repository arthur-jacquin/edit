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


### Commands/tutorial

* related to engine (lines to send, actual commands)
* cursor management (advanced movements, create/delete cursors...)
* lines management (yank/delete <n> lines, paste, move <n> lines up/down <m> lines, duplicate...)
* file management (reload, save, quit)
* built in-commands (increase/decrease indent, comment/uncomment)


### Keybinds (old)

INSERT:
[<ESCAPE>] >selection

DIALOG:
[<ENTER>] validation, ><previous_state>
[<ESCAPE>] annulation, ><previous_state>

MATCHES: (when no match: >selection)
[<ESCAPE>] >selection
[s] search in selection (prompt for pattern)
    ENTER: write regex to pattern, jump cursor to next, >matches
    ESCAPE: no write, >selection
[ ] keep/exclude matches containing pattern (prompt for pattern)
[...] extend/split matches to ... (see SPECIAL, same keybinds, when matches ongoing)

SELECTION:
[f] fix/free anchor
SPECIAL:
    [%] select all
    [l] select line
    [ ] select inner/outer element (can be field)
    [ ] select inner/outer bracket
    [ ] select block

ACTION: (on matches)
[x] delete (selected/unselected ?)
<quantifier>[p/P] paste after/before
[ ] switch to uppercase/lowercase
[r] replace with pattern
[.] repeat last command
[,] play macro
[>/<] increase/decrease indent
[c] count
[ ] comment/uncomment (based on langage)
[ ] reformat (based on langage)

ACTION: (on selection)
ACTION on matches
[y] copy
[d] cut
[ ] insert before/after selection [after going to the eol/inserting newline]

OTHER:
[ ] show syntax group ?
[q] start/stop macro recording
[u] undo
movements:
    <n>[G] go to specific line
    [ ] associated brackets
    [ ] start/end of line/file
    <quantifier>[n/N] next/previous match
    <quantifier>[ / ] next/previous character
    <quantifier>[ / ] next/previous word
    <quantifier>[ / ] next/previous line
    <quantifier>[ / ] next/previous screen
    <quantifier>[ / ] next/previous block
[w] write
[R] reload (prompt for confirmation if unsaved changes, else print "No change")
[Q] quit (prompt for confirmation if unsaved changes)
[W] writeas (prompt for name)
[S] set (prompt for variable/option and value)
[:] enter command (prompt for command)


### Registers (old)

OPTIONS:
* autowrite
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
