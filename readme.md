# edit

A suckless, simple, featured text editor.

[![asciicast](https://asciinema.org/a/575017.svg)](https://asciinema.org/a/575017)


## Description

edit aims to provide a simple and powerful editing model, in a suckless-style
codebase. In less than 3000 lines of C (counted with cloc) with no dependencies,
it features:

* a textual, compile-time configuration, with sane defaults in `config.def.h`
* kakoune-inspired selection-centric, selection-then-action editing
* vi-inspired default keybinds
* mouse support
* home-grown search and replace engine (regexp, subpatterns and fields support)
* home-grown syntax highlighting system
* multicursor, visual block and column editing
* advanced lines management including line clipboard
* line numbers, visual column, scrolling offset

To learn more about the design choices and limitations, read `philosophy.md`.


## Usage

To build and install, edit `config.mk` to match your local setup and run
`make install` (if necessary as root). While `cc` is the default compiler,
`tcc` is strongly advised.

To understand the editing model and the provided features, run `make tutor`.
After this exhaustive walkthrough, a `cheatsheet.md` is provided for quick
reference.

To run edit, just run `edit path/to/file`.

To configure edit, modify `config.h` to your tastes and recompile. If `config.h`
does not exist, start by copying `config.def.h` to `config.h`.


## Feedback/contact information

Your feedback is greatly appreciated! If you have any comment on the editor,
whether it's on the user side or the code, send an email at arthur@jaquin.xyz.
