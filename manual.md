# Manual

## Distribution model and build instructions

To produce a standalone executable, the configuration can only be done at
compile time. Compile-time configuration also reduces the size of the
executable, and suppress some bugs. Only good news so far!

However, that means you have to compile `edit` yourself, which may be daunting
to you if you never compiled anything. Don't worry, it's as easy as running
`make` to build the executable, and `make install` with according
priviledges to add the executable to `/usr/bin`. As there is no dependencies,
that should work fine, and compile pretty quickly as the codebase is very small.

But what about the configuration? Everything happens in the `config.h` header.
If there is none, the default configuration `config.def.h` will be used. Feel
free to modify `config.h` to fit your tastes, or to bring your own config file
from a previous build.

Note that you don't need to know how to code for modifying the configuration
file. For example, you can easily:
- modify the keybinds
- choose between 8 or 256 colors modes and modify the colorscheme
- enable/disable mouse support
- modify editor messages
- choose the default values for runtime-modifiable settings
- add new languages for syntax highlighting (see `syntax_highlighting.md`)


## Interacting with the editor

Note: all the keybinds used here are the default ones. If you modify them in
`config.h`, don't forget to check which keybind does what.

### Invocation

Here are all the ways to run the editor:

'''sh
edit filename       # edit the file filename
edit --help         # display the help message
edit -h             # display the help message
edit --version      # display the executable version
edit -v             # display the executable version
'''

Please note that the file that you want to edit must exist. If you want to
create a new file, you can `touch filename` before running `edit filename`.

### How elements are laid out on the screen

When editing a file, three elements are displayed, in a pretty common way for
terminal text editors:

    +----------------------------------------------------------------------+
    |                                                                      |
    |                                                                      |
    |                                 FILE                                 |
    |                                                                      |
    |                                                                      |
    | INTERFACE                                                     RULER  |
    +----------------------------------------------------------------------+

All lines but the bottom line are used to display the FILE being edited. On the
bottom line of the screen, an INTERFACE displays messages for the user and at
the right, a RULER gives the position of the cursor (formatted <line>:<column>).

### Modal editing

After startup, the editor loop indefinitely:

1. Refresh the elements on the screen
2. Wait for an user event (keypress, mouse or terminal resize)
3. Process the event

The way the event will be processed heavily depends on the mode in which the
editor is. There are three modes: normal, insert and dialog modes.

At startup, the editor is in normal mode, in which most keys is associated with
an editing command. This mode will be described later.

When in normal mode, you can press `i` to get in insert mode. There, most keys
will insert the associated character in the buffer. Press escape to get back to
normal mode.

Sometimes, the editor will need an user input, for example a search pattern. To
collect that input, the editor uses the dialog mode. In this mode, the user is
prompted something on the bottom line of the screen. The up arrow recovers the
last input associated with the prompt, and the down arrow clears the input.
Press enter to validate the input, and escape to cancel and get back to normal.

### Exiting

To exit the editor, make sure to get in normal mode by pressing escape, then
press `q`. If there are unsaved changes, the editor will display a warning. If
you want to quit and lose your changes, press `Q`.


## The editing model

    selections, differents stacks, anchoring
        change selections, act on them
        multiplier
    cover all commands, in a logical order
        specify how to modify runtime settings
    TODO cover all abilities of the editor

selections. what it is. act on selections. behave like multiple cursors.

ways of saving selections.
line ranges: like sed, so that its use is straigthforward when search and replace

if no saved selections, act on the running selections.
cursor/anchor, running selection, when not anchored, anchor always follows the
cursor. anchoring.

possible actions
    [insert mode]   insert a character
              u/U   switch to lowercase/uppercase
    >/<, TAB/^TAB   indent/unindent
                K   comment/uncomment line
                x   delete
                r   replace

clipboard.

Any actions where repeat makes sense, there is the multiplier

Search and replace with field support. Describe the model and the syntax.

Changing settings: explanations
