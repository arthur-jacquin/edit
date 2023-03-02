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

    edit filename       # edit the file filename
    edit --help         # display the help message
    edit -h             # display the help message
    edit --version      # display the executable version
    edit -v             # display the executable version

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

Most of the editing commands acts on a list of selections. A selection is
located on a given line, starts at a given character and has a given length.
They are represented on the screen by a subtle highlight. There are two lists:
the running selections, and the saved selections.

By default, the running selections list only contains the cursor: one selection,
on the cursor line, starting at the cursor column and of null length. This way,
the editor behaves as one could expect with a standard editor.

It is also possible to drop an anchor below the cursor. The running selections
list is not anymore reduced to a single, null-length selection under the cursor,
but contains all characters between the anchor and the cursor. As a selection
can only span on a given line, the list contains multiple selections if the
anchor and the cursor are not on the same line.

For a unified understanding of the running selections list, you can imagine that
the anchor follows the cursor when not anchored.

For more elaborate editing, one can save selections in the "saved" list. This
list is always prefered to the running selections list, and commands will act on
the latter only if the former is empty. The saved selections emulates a
multiple-cursor editing. There are various ways to create saved selections,
which will be described later on.

Finally, it makes sense for some actions to repeat them a given number of times.
Enter a multiplier <m> before these actions to get the expected result. If no
multiplier is given, the default value 1 is assumed.


## Commands

This section covers all the commands available in normal mode and their default
keybind.

### Basic commands, switching mode, managing file, changing settings

Here are the basic commands, with their default keybinds:

            <ESC>   return to normal mode
                ?   display the help message
              q/Q   quit/force quit
              w/W   write/write as
                R   reload
                i   get in insert mode
                s   change a setting

If you change a setting, you will be prompted a modification. This modification
must be formatted like "<name>=<value>" where <name> is in the table below and
<value> is of the according type. Any integer value is considered a boolean, 0
meaning FALSE and any other value meaning TRUE.

    SETTING                 NAME    TYPE    DEFAULT VALUE
    syntax highlight        sh      bool    TRUE
    highlight selections    h       bool    TRUE
    case sensitive          c       bool    TRUE
    field separator         fs      char    ,
    tab width               tw      int     4
    language                l       string  (from extension)

At the cost of emptying the saved selections list and unanchoring the anchor,
these commands get you in insert mode at a specific location:

              I/A   at start/end of line
              o/O   on an empty line created below/above cursor line

### Moving around

For efficient editing, `edit` provides commands for fast movements. You can
move to:

                m   matching bracket
            0/^/$   start/first non-blank character/end of line
              g/G   line <m>/last line of the file
      arrows, l/h   <m> next/previous character
      arrows, j/k   <m> next/previous line
              t/T   <m> next/previous word
              }/{   <m> next/previous block
              n/N   <m> next/previous match

### Managing selections

Here is how you can manage selections:

            <ESC>   forget saved selections, unanchor
                c   display number of saved selections
                .   select cursor line
            %/b/:   select all lines of file/<m> following blocks/custom range
             f, /   search for pattern
                v   anchor/unanchor
                a   save running selections; unanchor
                z   duplicate the current selection on the <m> following lines

When selecting a custom range, the user is prompted a range that must be
formatted like "<min>,<max>" where <min> and <max> are either empty, a dot or an
integer. If needed, empty is converted in 1 (for <min>) or the line number of
the last line (for <max>), and dots are converted in the cursor line number, in
order to select any line whose number is between min and max.

Searching for pattern will create a new saved selections list containing all
substrings of previously saved selections matching the given pattern. Note that
searching is incremental: the potential results will be displayed as you type
your search pattern. See the last section for more details on searching.

The command associated to keybind `z` was designed for easy column editing.

### Acting on selections

Here is how you can act on selections:

    [insert mode]   insert a character before selections
    >/<, TAB/^TAB   <m> decrease/increase line indent
                K   comment/uncomment line
          x/<DEL>   suppress selection if non-null, else before/after selection
                r   replace with pattern elements and fields
              u/U   switch to lowercase/uppercase

Some of these commands are executed at most once per line, so that even if there
are two saved selections on a given line, running `>` will only indent that line
once.

See the last section for more details on replacing.

### Managing lines

`edit` has a line-centric clipboard. The following commands does not act on
selections but directly on lines:

              y/Y   yank <m> lines/blocks
              d/D   delete <m> lines/blocks
              p/P   paste after/before <m> times
     SHIFT+arrows   move cursor line <m> lines up/down


## The search and replace engine

Search and replace with field support. Describe the model and the syntax.
line ranges: like sed, so that its use is straigthforward when search and replace
