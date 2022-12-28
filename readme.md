https://github.com/termbox/termbox2


(Too) complete list of features that one could want
> https://github.com/adsr/mle
    sed integration OR search n replace with field support, multi level of keep/ignore/suppress including pattern
    suckless philosophy
        full UTF-8 support
        no deps
        small, easy codebase
        no runtime
    Syntax highlighting
    Large file support
    Incremental search
    Linear undo and redo
    Multiple cursors
    Auto indent
    mouse support

Representation of text as ropes tree
> https://xi-editor.io/docs/crdt-details.html

Inspiration:
> http://www.ungerhu.com/jxh/vi.html
> https://vimhelp.org/vi_diff.txt.html
> https://jsimlo.sk/notepad/features.php
> http://www.kpdus.com/ted.html
> https://kakoune.org/why-kakoune/why-kakoune.html
> https://github.com/mawww/kakoune#advanced-topics
> https://xi-editor.io/docs.html
> https://github.com/chmln/sd
notepad++
> http://doc.cat-v.org/plan_9/4th_edition/papers/sam/
> http://man.cat-v.org/plan_9/1/sam
> http://sam.cat-v.org/cheatsheet/sam-refcard.pdf

sed documentation:
> https://www.gnu.org/software/sed/manual/sed.html
> https://pubs.opengroup.org/onlinepubs/007904975/utilities/sed.html
> https://pubs.opengroup.org/onlinepubs/9699919799/
> https://pubs.opengroup.org/onlinepubs/9699919799/utilities/sed.html

Cleanly separate text editing engine and client

simple visual editor with
    client-server transactionnal architecture
        1-level undo/redo
        multi-cursor/collaborative editing
    some commands
        copy/cut/paste on internal/system clipboard
        upper/lowercase
        increase/decrease indent
        move up/down lines
    advanced movements

    !syntax highlight through system library
    !structural completions, automatic indenting, reformatting, [un]comment
    fuzzy completion through system library
    subselections mechanisms
    incremental, optionally-visually-previewed call to external manipulation tools on anything/subselections
        sed & easy_awk (regex+field search and replace)
        grep
        wc
        ...


# Reading and editing textual files

[edit][Simple, featured, modal text editor.]
[view][Pager.]

STATES:
* selection (default)
* matches (multiple selections, with highlights)
* insertion (only in interactive)
* dialog (only in interactive)

simple, predictable, dependable, fast

*selection then action* philosophy.

Where to focus on multiple selections ? > On next matches (virtual cursor)
which features works in interactive/in place/filter use ?
which features works when in reader mode ?


## view

A read-only file viewer that shares most of the code with edit.

* display file
* incremental search with regex/field recognition
* matches, lines, words and characters count
* modal editor: default, search, insert, dialog
* quick navigation (movements)
* langage detection and syntax highlight


## Features

advanced navigation, search
search and replace

* can be used interactively, in place or as a filter
* a client-server architecture ?
* action on all matches
* search and replace with regex/field recognition
    [sed] basic search and replace ([%] s/<pattern>/r/<pattern>/) 
    [grep] filter out not containing ([%l] k/<pattern>/)
    [awk] keep field <n> ([%l] r/$<n>/)
* matches, lines, words and characters count
    [wc] (%c)
* upper/lowercase
INTERACTIVE-ONLY:
* modal editor: selection, matches, insert, dialog
* quick navigation
* langage detection (based on file extension)
* syntax highlight (based on langage)
* comment/uncomment (based on langage)
* automatic indenting, reformatting (based on langage)
* incremental search and replace
* increase/decrease indent
* 1-level undo, repeat
* macro

edit <file[s]>                  Interactive editor
edit -i <command> <file[s]>     In place edit
<...> | edit -f <command>       Used as a filter
edit -p <command> <file[s]>     Read and filter to stdout

When used as a filter
* lines are considered independent, as separate files to operate on
* only prints matches or print everything ? 

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


REGISTERS:
    OPTIONS:
        autowrite
        autoindent
        syntax_highlight
        highlight_matches
        case_sensitive
        use_tabs
        cursor_line
        is_anchored
    VARIABLES:
        field_separator
        tab_width
        scrolloff (number of screen lines to keep above and below the cursor)
        langage
        state (with precedent)
        anchor, cursor (with precedent) position
        window size
        search pattern (with precedent)
        clipboard (history ?)
        macro (with precedent)
        command (with precedent)
        9 registers for subpattern in regex
    pile de match

\\ (backslash), \$ (dollar sign)
\0 (whole pattern), \1 .. \9 (subpattern in regex)
$0 (whole line), $1 .. $9 (fields)
$c (clipboard)
$s (field_separator)

INTERFACE:
    Text
    Command line, cursor position


## Links


# Examples

e.g. revert second field around eventual first space:

    s\2
    k\(.*\) \(.*\)
    r\2 \1

e.g. grep replacement:

    xpomme
    d

e.g. sed replacement:

    s<pattern>
    r<pattern>


# Tutoriel

* Launching
    how to launch
    what you get 
