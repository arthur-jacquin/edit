# Philosophy

## The story

When I started programming, I found the ubitiquous [vim](https://www.vim.org). I fell in love with its
logic, efficiency and power. But as I used it, I started to notice areas where
vim not fitted my needs the way I wanted, and I searched for alternatives.
Good news: there are plenty! Here is a curated list:

Some are very small:
* [kilo](https://github.com/antirez/kilo) (C)
* [kibi](https://github.com/ilai-deutel/kibi) (Rust)

Some are innovative:
* [kakoune](https://kakoune.org) (C++): cleaner editing model
* [sam](http://doc.cat-v.org/plan_9/4th_edition/papers/sam/) (C): bringing structural regular expressions
* [vis](https://github.com/martanne/vis) (C, Lua): smart combination of vim and sam
* [de](https://github.com/driusan/de) (Go): mix of vim and acme, integrates with p9p plumbing

Some are neither crazy small nor very innovative, but still look solid:
* [mle](https://github.com/adsr/mle) (C)
* [micro](https://github.com/zyedidia/micro) (Go)
* [aretex](https://github.com/aretext/aretext) (Go)
* [smith](https://github.com/IGI-111/Smith) (Rust)
* [iota](https://github.com/gchp/iota) (Rust)

So why writing a new one ? There are two main reasons.

The first one is the learning experience. After reading "The C programming
language" from Dennis Ritchie and Brian Kernighan, I was looking for a project
to try my early C skills. edit was a perfect fit. I learned a lot, and had so
much fun along the way.

The other reason is that I wanted a text editor written the [suckless](https://suckless.org) way.
I haven't found one combining a capable, smooth editing model and a simple,
small C codebase using only compile-time configuration. edit tries to do that.

Before publishing it, I used it for several months to polish it to my needs. It
should now be quite feature-stable. If you wonder if the editing model will work
for you, you can read about the design choices below. If it sounds good to you,
do not hesitate to try it! In that case, I would greatly appreciate a feedback,
wether it's on the user side or the code.


## Simplicity as the utmost goal

The main goal of the editor is simplicity. I believe it helps for the secondary
goals:

* making it efficient, intuitive and predictable
* writing small, well-structured, easily-hackable code
* shipping a sane default configuration, as well as a clear configuration file
* making sure all commands are accessed with a single keypress


## Old-school development

edit development is inspired by the suckless philosophy.

### Written in less than 2500 lines of C, with no dependencies

The stability (and the technical qualities) of the C language explains its
longevity and its wide avaibility. Combined with a small codebase with no
dependencies (except standard library), choosing C ensures great portability.

### Produces a standalone executable

It eliminates some potential runtime bugs, increase its portability, and avoid
the need for a dynamic linker.

### Textual, compile-time configuration, with sane defaults

On vim, a `.vimrc` is almost mandatory to get a sensible editor. With edit,
configuration is not needed: a default configuration `config.def.h` is provided.
This file contains all the settings one could want to modify, which eases
discoverability. It also makes it simple to save the configuration for the
future.

As editing this file and compiling are considered to be a part of the editor
experience, the editor is not to be packaged other than in a tarball.

### Keyboard-centric, terminal-based editor

While I prefer the terminal-keyboard combination, I think the user should not be
retained to use the mouse power. By default, edit supports scrolling and moving
the cursor with the mouse. If it bothers you, you may disable this compile-time
option.

### Only edits text

edit is text editor. It edits text, not file archives or compressed files.

It is not an IDE: it has no built-in terminal, file manager, git integration or
syntax/compiler warnings and errors.

It is neither a multiplexer nor a window manager: it has no splits, tabs or
buffers.

It is not a word processor: it has no styling or spell checking.

### Feature-stable

edit is not meant to grow indefinitely. It has a finite number of carefully
chosen features. These can be all discovered by reading the manual (`manual.md`,
10 minute read) or the configuration file, where a key is associated to each
command.

It avoids the feeling of being overwhelmed by the number of features, which can
happen when starting to use vim. If you're new to advanced text editing, no need
to master all of them at first read, but you know what exists and where to find
the information. The learning curve is therefore pretty gentle, unless you want
to dig into it faster than needed.

Today edit is considered mostly feature stable. The following features are not
excluded in the future:

* executing a command, with the possibilities of piping selections to it and
sending it in the background
* display line numbers
* tab-completion


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

[^1]: [kakoune philosophy](https://kakoune.org/why-kakoune/why-kakoune.html)

edit is selection-centric, as most editing commands act on a list of selections.
This approach easily enables multi-cursor, visual block and column editing.

Selecting happens before action like in kakoune, and not the other way around
like in vim. It provides more interactivity, as you can see what you will act on
before action, enabling you to correct the selections without having to undo the
action.

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
of subpatterns and fields from the to-be-replaced string: the power of sed and
awk combined! Please refer to `manual.md` for a more precise description.


## Code-related design choices

The main goal being simplicity for both code and usability, many features that
might be considered interesting but that brings too much complexity to my tastes
have intentionnally been left aside.

edit has limited abilities. It is a feature. Some lacks might be a breaker for
you: reading the following will help in deciding if edit is capable enough for
your workflow. If not, one of the editor listed at the top of this file might
better suits you ;)

### No elaborated data structures for storing text

Some editors provides elaborated data structures for storing text: conflict-free
replicated data type in xi[^2], database in sam...). This upfront complexity
might be useful or even crucial when editing really, really big files, but
this is not what edit strives for.

[^2]: [CRDT](https://xi-editor.io/docs/crdt-details.html)

### UTF-8 only

There exists plenty encodings, but UTF-8 makes the most sense. Obviously, plain
ASCII is also supported as it's a subset of UTF-8. For other encodings, you
might have to translate it first.

### No undo/redo

When I was using vim, I found out I was saving the file so often that undoing
was most of the time equivalent to reloading the file. Frequent saves and
occasionnal reloads avoids the need for autosave and undo/redo system.

### Wrong syntax highlight for multiline elements

Contrary to sam and its structural regular expressions[^3], the line abstraction
is central in edit. The syntax highlighting system operates on each line
independently. Therefore, multiline strings and comments are not highlighted as
one could expect. However, the commenting command use single line comments: the
user is encouraged to use it over ranges of lines instead of using multiline
comments.

[^3]: [structural regexp](https://doc.cat-v.org/bell_labs/structural_regexps/se.pdf)

Please refer to `syntax_highlight.md` for a more precise description of the
system.

### No plugins support

As edit is considered mostly complete feature-wise, I have not seen the
justification for designing a plugin support. If you want to add functionnality,
patch it, fork it, but an interface for plugins is not something edit will have.

### Not a client-server architecture

While the structure of the code is quite compatible with a client-server
architecture, I've decide to not do that. Many editors does, and in some cases
it might be useful, but I don't really see the *need* in most uses cases, and as
it adds some complexity, I left it aside. But I might pick it up some day.

### Others limitations

* does no work with tabs
* does not visually wrap lines
