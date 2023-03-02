## The story/rationale

When I started programming, I found the ubitiquous [vim](https://www.vim.org). I fell in love with its
logic, efficiency and power. But as I used it, I started to notice areas where
vim not fitted my needs the way I wanted, and I searched for alternatives.
Good news: there are plenty! Here is a curated list:

Some are very small:
- [kilo](https://github.com/antirez/kilo) (C)
- [kibi](https://github.com/ilai-deutel/kibi) (Rust)

Some are innovative:
- [kakoune](https://kakoune.org/why-kakoune/why-kakoune.html) (C++): cleaner editing model
- [sam](http://doc.cat-v.org/plan_9/4th_edition/papers/sam/) (C): bringing structural regular expressions
- [vis](https://github.com/martanne/vis) (C, Lua): smart combination of vim and sam
- [de](https://github.com/driusan/de) (Go): mix of vim and acme, integrates with p9p plumbing

Some are neither crazy small nor very innovative, but still look solid:
- [mle](https://github.com/adsr/mle) (C)
- [micro](https://github.com/zyedidia/micro) (Go)
- [aretex](https://github.com/aretext/aretext) (Go)
- [smith](https://github.com/IGI-111/Smith) (Rust)
- [iota](https://github.com/gchp/iota) (Rust)

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

### Style

* The suckless way
* old school development
* config.def.h
* feature-stable, not meant to grow indefinitely
* suckless (C, simple, small, idiomatic, no runtime parameter...)
* <= 3000 SLOC

simplicity, predictability, orthogonality, powerful, intuitive editing model


### Simplicity

single stroke commands
no command like in vim

predictable

fast

sensible defaults. Parameters that makes sense modyfing can be, cf config.def.h.

more hackable, small (<= 3000 SLOC) and hopefully idiomatic codebase

## Orthogonality, layers

#### UTF-8 encoding

#### Written in C, with no dependencies!

So stable, first language supported on most systems, therefore edit might be run
everywhere with no problem.
(uses the great, minimal, single-header termbox drawing library)
Ensure great protability.

#### Create a standalone executable!

Dynamic linking mess, Eliminates many runtime potentials bugs

#### Optionnal configuration!

Need no configuration! on vim, a .vimrc is almost mandatory, else not quite usable
Here, the editor is simple enough so that there are few choice to be made.
all settings the user could want to modify are in config.def.h, so discoverability
is awesome. You should really have a look at it. easy to save configuration for
future: it's a text file. strongly inspired by suckless.
As editing and compiling is considered to be a part of the editor experience, the
editor is not to be packaged other than in a tarball.

#### Terminal-based.

No GUI.

#### Modal editor

Like vi(m), kakoune, and others. In insert mode, most keys insert
their caracters in the beginning of the selections. In normal (default) mode,
keys have completely different effects: quit, reload the file, move through the
file, manage selections, act on selections, interact with the lines clipboard...
In dialog mode, the user is prompted a value for more elaborate tasks
(e.g. choosing a new file name). Not biaised towards insertion, as it is not the
main editing task

Mouse support! While the editor is quite keyboard-centric, I think the user sould
not be completely suppressed its right to use the mouse power.
Scrolling, moving the cursor.
(it is a compile time option, if it bothers you)

UTF-8 support! (only encoding supported)

Incremental, regexp search; replace with subpatterns and fields support.
The power of sed and awk combined.

Finite features. Small number of carefully chosen features. All discoverable by
either reading the user guide (10 minutes read) or the config.def.h, where a
key is defined for each possible operations. No feeling of being overwhelmed like
in vim where the user can found years later features that could have helped him.
If you're new to advanced text editing, no need to master all of them at first
read, but you know it exists and where to find the information.
therefore the learning curve is pretty gentle, unless you want to dig into it
faster than needed. 

If any of this sounds overfeatured, you might have a look at those editors: TODO

### Selections then action philophy

Like Kakoune
The editor is selections-centric. All editing actions, except the lines clipboard,
acts on a list of selections. Therefore it's similar to Kakoune in the sense of
it's selection then action philosophy, and not action then selection like in vi(m).
provide more interactivity.

This approach easily enables multi-cursor editing, visual block/column editing...

Benefits on search and replace, that are decoupled: searching is an operation on
selections, while replacing is an action on selections.
incremental, see what will be replaced, or different actions than replace
(e.g. switch to uppercase)
or replace with no search pattern previously, e.g. if you want to exchange fields


### What `edit` is not

> Most of the following what I intentionnally not implemented. The main goal being
simplicity, for both code and usability, many features that might be considered
interesting but that brings too much complexity have been left aside. Reading
this will help you in deciding if `edit` is enough featured for your workflow,
and if not, in finding an editor that better suits you. 

Considered feature complete: i'm not advertising you of the *extensive* plugin
support in any language you might think of or such. plugins are bloat. if you want
t add functionnality, patch it, fork it, but an API for plugins is not something i wish
in an editor. calls for asyncronous... all bloat. Not that much expandable.

Does not integrates UNIX tools. (considered complete, might leave the editor if
you want to do something). Quite not a great unix citizen after all, and that's
ok.

Not an IDE! No terminal, file manager, git integration, syntax/compilator warnings.
Not a multiplexer/window manager! no splits, tabs, buffers.
Not a word processor! No styling, no spell checking.
It's a text editor.

Not a client-server architecture. While the code could be very easily splitten
in a client and a server, I've decide to not do that. Many editors does: sam...
In some cases it might be useful. But I don't really see the *need* in most
uses cases, and as it adds some complexity, I left it aside. But I might pick it
up some day.

No undo-redo!

No line wrap, no folding.

Clipboard only works with whole lines!

No elaborated data structures! Some are quite elaborated (ropes in xi, database
in sam and others), and this upfront complexity might be useful or even crucial
when editing really, really big quantities.

No scripting language, nor macros. No action-movement like in vim. (e.g. dw for
delete word).
Quite difficult to wrap its head around, and
together produce complexity. If user is interested in that,you might Kakoune that
hace an interesting approach, or sam or vis or TODO.

Text completion ?

Search engine and syntax highlighting are not full fledged.

Support file archives, compressed or over-networks files

