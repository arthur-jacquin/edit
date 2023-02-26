# edit

A suckless, simple, featured, modal text editor.

[TODO](asciinema short video)

## Links

- [source code repository]()
- [source code tarball]()
- [website]()
- [complete walktrough]()

## Documentation

Read `philosphy.md` to see why I wrote `edit`, what it is and is not, the design
choices explanations, how it compares to other editors, other editors that might
better suits you. If you're wondering if you should use `edit`, read this.

Read `user_guide.md` to see build instructions, use guide, and discover features.
If you want to use `edit`, read this.

Read `cheatsheet.md` to see a quick summary of all runtime options, commands and
default keybinds.

Others documentation files are in-depth descriptions of some systems, like
syntax highlighting or search and replace engines; or technical notes.

## Goals

* produce a standalone executable
* simplicity, predictability, efficiency, orthogonality
* sane defaults, in config.def.h
* suckless (small, idiomatic codebase, no runtime parameter...)
* does not try to be an IDE, a multiplexer or a window manager
* <= 3000 SLOC
* no deps

## Opiniated choices

* *selection then action* philosophy.
* no undo/redo: the save/reload/quit philosophy...
* does not visually wrap lines.

## Limitations

* does not highlight multi lines strings/comments.
* does not work well with very big files.
* does not work with tabs.
* search: one level of \\( \\) maximum, no escapes in classes

## Features

* UTF-8 support
* modal editor: edit, insert, dialog modes
* some syntax highlight
* advanced navigation
* mouse support
* advanced line management
* search and replace engine with field support
* vi-inspired default keybinds

## Thanks

- my brother for beta-testing and comments
- [termbox2](https://github.com/termbox/termbox2) terminal rendering library

## License

`edit` is licensed under the GPLv3. See `license`.

## Feedback/contact information

Your feedback is greatly appreciated! If you have any comment on the editor,
wether it's on the user side or the code, please send me an email at arthur@jaquin.xyz
