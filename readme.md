# edit

A suckless, simple, featured text editor.

[TODO](asciinema screencast)


## Description

edit aims to provide a simple and powerful editing model, in a suckless-style
codebase. In less than 2500 lines of C (counted with cloc) with no dependencies,
it features:

* a textual, compile-time configuration, with sane defaults in `config.def.h`
* kakoune-inspired selection-centric, selection-then-action editing
* vi-inspired default keybinds
* mouse support
* home-grown search and replace engine (regexp, subpatterns and fields support)
* home-grown syntax highlighting system
* line numbers, visual column, scrolling offset
* multicursor editing
* advanced lines management including line clipboard

To learn more about the design choices and limitations, read `philosophy.md`.


## Usage

    edit file

To compile edit, run `make`, or `make install` with priviledges to put the
executable in `/usr/bin`. The default compiler is tcc, in static-linking mode.
You can modify that in `makefile`.

To understand the editing model and the features provided by edit, run
`make tutor`. After this exhaustive walkthrough, a `cheatsheet.md` is provided
for quick reference.

To configure edit, modify `config.h` to your tastes and recompile. If `config.h`
does not exist, start by copying `config.def.h` to `config.h`.


## License

edit is licensed under the GPLv3. See `license` for more information.


## Feedback/contact information

Your feedback is greatly appreciated! If you have any comment on the editor,
wether it's on the user side or the code, send an email at arthur@jaquin.xyz.
