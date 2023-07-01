# Philosophy

## Simplicity as the utmost goal

The main goal of the editor is simplicity. I believe it helps for the secondary
goals:

* making it efficient, intuitive and predictable
* writing small, well-structured, easily-hackable code
* shipping a sane default configuration, as well as a clear configuration file
* making sure all commands are accessed with a single keypress


## Old-school development

edit development is inspired by the suckless philosophy.

### Written in less than 3000 lines of C, with no dependencies

The stability (and the technical qualities) of the C language explains its
longevity and its wide avaibility. Combined with a small codebase with no
dependencies (except standard library), choosing C ensures great portability.

### Produces a freestanding, statically linked executable

It eliminates some potential runtime bugs, increases its portability, and avoids
the need for a dynamic linker.

### Textual, compile-time configuration, with sane defaults

On vim, a `.vimrc` is needed to get a sensible editor. With edit, a sane
default configuration is provided in `config.def.h`. This file contains all the
settings one could want to modify, which eases discoverability.

The fact that the configuration is textual makes it simple to save the
configuration for the future. Editing the configuration file and compiling are
considered to be a part of the editor experience.

### Keyboard-centric, terminal-based editor

Everything can be done with the keyboard, but mouse is supported (scrolling,
moving the cursor). One may disable this compile-time option.

### Only edits text

edit is text editor. It edits text, not file archives or compressed files.

It is not an IDE: it has no built-in terminal, file manager, git integration or
syntax/compiler warnings and errors.

It is neither a multiplexer nor a window manager: it has no splits or tabs.

It is not a word processor: it has no styling or spell checking.

### Feature-stable

edit is not meant to grow indefinitely. It has a finite number of carefully
chosen features, that are all explained in the tutorial. It avoids the feeling
of being overwhelmed by the number of features, which can happen when starting
to use vim.


## User experience design choices

### Modal edition

In insert mode, most keys insert their characters before the selections, as most
basic editors do. But the (default) normal mode associates completely different
meaning to each key: quit, reload the file, move through the file, manage
selections, act on selections, insert the clipboard... That way the editor is
not biaised towards insertion (which is not the main editing task).

### Selection-centric, selection then action philosophy

The following points were inspired by kakoune, which has done a great job at
explaining its philosophy. Refer to their documentation for more details[^1].

[^1]: https://kakoune.org/why-kakoune/why-kakoune.html

edit is selection-centric, as most editing commands act on a list of selections.
This approach easily enables multi-cursor, visual block and column editing.

Selecting happens before action like in kakoune, and not the other way around
like in vim. It provides more interactivity, as one can see what edit will act
on before action, enabling correcting the selections without having to undo the
action (something that is not possible in edit).

### No edition language

Following the goal of simplicity, commands behaves individually, and while some
sequences of commands are more used than others, commands do not explicitely
combine together. I think it helps lowering the complexity, for the user and for
the code, without reducing functionnality. As edit does not provide a proper
"edition language", there is no macro support.

### Home-grown search and replace engine

The search and replace engine is home-grown. While it avoids the need for
dependencies, it is not to be seen as a weaker, spare replacement. The search is
incremental and supports regular expressions, and the replace supports the reuse
of subpatterns and fields from the to-be-replaced string. The engine is also
better integrated in the editor that a sed call would have. Please refer to the
tutorial for a more precise description.


## Code-related design choices

The main goal being simplicity for both code and usability, many features that
might be considered interesting but that brings much complexity have
intentionnally been left aside.

edit has limited abilities. Reading the following may help in deciding if edit
is capable enough for your workflow. If not, others editors are listed at the
end of this file.

### No support for every terminal

Even if edit has no dependencies, it won't run everywhere, as the terminal
drawing library (termbox[^2]) does not support all terminals. For example, it
won't run on Windows, unless one use WSL or a similar solution.

[^2]: https://github.com/termbox/termbox2

All the interaction between edit and its environment happens through
`termbox.h`. Therefore if one want to embed the editor in its own (GPLv3)
software, make it work on Windows, or build a Graphical User Interface, all
there is to do is to replace `termbox.h` with a file adapted to the targetted
environment and providing the same API.

### No elaborated data structures for storing text

Some editors provides elaborated data structures for storing text: conflict-free
replicated data type in xi[^3], database in sam...). This upfront complexity
might be useful or even crucial with collaborative editing or really, really big
files, but this is not what edit strives for.

[^3]: https://xi-editor.io/docs/crdt-details.html

### No undo/redo

When I was using vim, I found out I was saving the file so often that undoing
was most of the time equivalent to reloading the file. Frequent saves and
occasionnal reloads avoid the need for autosave and undo/redo system.

### Wrong syntax highlight for multiline elements

Contrary to sam and its structural regular expressions[^4], the line abstraction
is central in edit. The syntax highlighting system operates on each line
independently. Therefore, multiline strings and comments are not highlighted as
one could expect. However, the commenting command use single line comments: the
user is encouraged to use it over ranges of lines instead of using multiline
comments.

[^4]: https://doc.cat-v.org/bell_labs/structural_regexps/se.pdf

### No plugins support

As edit is considered mostly complete feature-wise, I have not seen the
justification for designing a plugin support. One can patch or fork edit to add
functionnality, but an interface for plugins is not something edit will have.

### Not a client-server architecture

Many editors do that, and in some cases it might be useful, but I don't really
see the *need* in most use cases, and as it adds some complexity, I left it
aside.

### Others limitations

* only works with UTF-8 encoding
* transforms tabs to spaces (but saves tabs for makefiles)
* does not visually wrap lines


## Interesting alternatives

Crazy small editors:
* [kilo](https://github.com/antirez/kilo) (C)
* [kibi](https://github.com/ilai-deutel/kibi) (Rust)

Innovative editors:
* [kakoune](https://kakoune.org) (C++)
* [sam](http://doc.cat-v.org/plan_9/4th_edition/papers/sam/) (C)
* [vis](https://github.com/martanne/vis) (C, Lua)
* [de](https://github.com/driusan/de) (Go)

Others:
* [micro](https://github.com/zyedidia/micro) (Go)
* [aretex](https://github.com/aretext/aretext) (Go)
* [smith](https://github.com/IGI-111/Smith) (Rust)
* [iota](https://github.com/gchp/iota) (Rust)
* [mle](https://github.com/adsr/mle) (C)
