# edit

Simple, featured, modal text editor.

[TODO](asciinema video)

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
* modal editor: edit, insert, dialog modes
* some syntax highlight
* advanced navigation
* mouse support
* advanced line management
* search and replace engine with field support

Maybe in a far future:
* read-only mode ?
* scrolloff ?
* works with tab ?
* [langage] auto-indenting ?
* visual wrap of long lines ??
* tab-completion ??
* inline clipboard ??
* macros ???

## Non-runtime-modifiable parameters

You can modify settings in config.h, such as:
- mouse support, scroll line number...
- default values for parameters
- 8 or 256 colors modes, colorscheme
- keybinds
- ...

If there is no config.h at compile time, a default configuration is loaded
from config.def.h.


## Thanks

- my brother for beta-testing and comments
- [termbox2](https://github.com/termbox/termbox2) terminal rendering library



## Related projects

Inspiration
- [suckless tools](https://suckless.org/) for the development philosophy
- [vim](https://www.vim.org/) for the interface and most keybinds
- [kakoune](https://kakoune.org/) for the *selection then action* philosohy


## The story/rationale

When I started programming, I found the ubitiquous vim. I fell in love with its logic, efficiency and power. But as I used it, I started to want something more minimal, something closer to the suckless philosophy. While there is no text editor on suckless.org, there are plenty that follows a similar philosophy :
- [kibi](https://github.com/ilai-deutel/kibi)
- [kilo](https://github.com/antirez/kilo)
- [mle](https://github.com/adsr/mle)
- [iota](https://github.com/gchp/iota)
- [micro](https://github.com/zyedidia/micro)
- [aretex](https://github.com/aretext/aretext)
- [de](https://github.com/driusan/de)
- [smith](https://github.com/IGI-111/Smith)

So why writing a new one ? Mainly for the learning experience. After reading "The C programming language" from Dennis Ritchie, I was looking for a project to try my early C skills. edit is written from scratch, without looking at any other editor code. I had so much fun designing a syntax highligt system, or a search and replace engine with regular expression and fields support. My goal has never been completeness; and the editor intentionnally lacks some features.

Before publishing edit, I used for several months to polish it to my needs. It is now quite feature-stable, I use it every day. However the limitations might bothers one. If you ever try it, you should have no expectations, but I would greatly appreciate a feedback, wether it's on the user side or the code.
