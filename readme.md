# edit

A suckless, simple, featured, modal text editor.

[TODO](asciinema short video)

### Description

edit combines a simple and powerful editing model with a suckless approach:

- written in less than 2500 lines of C, with no dependencies
- produces a standalone executable
- textual, compile-time configuration, with sane defaults in config.def.h
- keyboard-centric, terminal-based editor
- only edits text: does not try to be an IDE or a multiplexer/window manager
- feature-stable: not meant to grow indefinitely

### Features

- vi-inspired default keybinds
- mouse support
- home-grown search and replace engine (regexp, subpatterns and fields support)
- home-grown syntax highlight system
- multicursor editing
- advanced lines management including line clipboard

### Opiniated choices

- selection-centric, selection then action philosophy
- frequent save/reload instead of undo/redo
- does not visually wrap lines
- does not highlight multiline elements (strings, comments)
- does not work with tabs
- only works with UTF-8 encoded text

### Documentation

Read `philosophy.md` if you're wondering if you should use the editor. You will
find:
- why I wrote it
- what it is and is not
- design choices and explanations
- a list of editors that might better suits you

Read `manual.md` if you want to know how to use the editor. You will find:
- build instructions
- an explanation of the editing model
- a complete walktrough of its features and mechanisms

Read `cheatsheet.md` if you want a quick overview of the default keybinds and
the name of the runtime options.

Others `*.md` files are technical notes covering specific points.

### Links

- [source code repository]()
- [source code tarball]()
- [website]()
- [complete asciinema walktrough]()

### Thanks

- my brother for beta-testing and comments
- [termbox2](https://github.com/termbox/termbox2) terminal rendering library

### License

edit is licensed under the GPLv3. See `license`.

### Feedback/contact information

Your feedback is greatly appreciated! If you have any comment on the editor,
wether it's on the user side or the code, please send me an email at
arthur@jaquin.xyz.
