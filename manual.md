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
free to copy `config.def.h` to `config.h` and modify the latter to fit your
tastes, or to bring your own configuration file from a previous build.

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
the right, a RULER gives the position of the cursor (formatted {line}:{column}).

### Modal editing

After startup, the editor loop indefinitely:

1. Refresh the elements on the screen
2. Wait for an user event (keypress, mouse or terminal resize)
3. Process the event

The way an event is processed heavily depends on the mode in which the editor
is. There are three modes: normal, insert and dialog modes.

At startup, the editor is in normal mode, in which most keys is associated with
an editing command. All the commands of this mode will be described later.

When in normal mode, you can press `i` to get in insert mode. There, most keys
will insert the associated character in the buffer. Press escape to get back to
normal mode.

Sometimes, the editor will need an user input, for example a search pattern. To
collect that input, the editor uses the dialog mode. In this mode, the user is
prompted something on the bottom line of the screen. The up arrow recovers the
last input associated with the prompt, and the down arrow clears the input.
Press enter to validate the input, or escape to cancel and get back to normal.

### Exiting

To exit the editor, make sure to be in normal mode by pressing escape, then
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
Enter a multiplier {m} before these actions to get the expected result. If no
multiplier is given, the default value 1 is assumed.


## Commands

This section covers all the commands available in normal mode and their default
keybind. All of these are condensed in `cheatsheet.md` for quick reference.

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
must be formatted like "{name}={value}" where {name} is in the table below and
{value} is of the according type. Any integer value is considered a boolean, 0
meaning FALSE and any other value meaning TRUE.

    SETTING                 NAME    TYPE    DEFAULT VALUE
    syntax highlight        sh      bool    TRUE
    highlight selections    h       bool    TRUE
    case sensitive          c       bool    TRUE
    field separator         fs      char    ,
    tab width               tw      int     4
    language                l       string  (from extension)

After emptying the saved selections list and unanchoring the anchor, these
commands get you in insert mode at a specific location:

              I/A   at start/end of line
              o/O   on an empty line created below/above cursor line

### Moving around

For efficient editing, `edit` provides commands for fast movements. You can
move to:

                m   matching bracket
            0/^/$   start/first non-blank character/end of line
              g/G   line {m}/last line of the file
      arrows, l/h   {m} next/previous character
      arrows, j/k   {m} next/previous line
              t/T   {m} next/previous word
              }/{   {m} next/previous block
              n/N   {m} next/previous match

### Managing selections

Here is how you can manage selections:

            <ESC>   forget saved selections, unanchor, reset multiplier
                c   display number of saved selections
                .   select cursor line
            %/b/:   select all lines of file/{m} following blocks/custom range
             f, /   search for pattern
                *   search for the word that is currently under the cursor
                v   anchor/unanchor
                a   save running selections; unanchor
                z   duplicate the current selection on the {m} following lines

When selecting a custom range, the user is prompted a range that must be
formatted like "{min},{max}" where {min} and {max} are either empty, a dot or an
integer. If needed, empty is converted in 1 (for {min}) or the line number of
the last line (for {max}), and dots are converted in the cursor line number, in
order to select any line whose number is between {min} and {max}.

Searching for pattern will create a new saved selections list containing all
substrings of previously saved selections matching the given pattern. Note that
searching is incremental: the potential results will be displayed as you type
your search pattern. See the search and replace engine section for more details
on searching.

The command associated to keybind `z` was designed for easy column editing.

### Acting on selections

Here is how you can act on selections:

    [insert mode]   insert a character before selections
    >/<, TAB/^TAB   {m} increase/decrease line indent
                K   comment/uncomment line
          x/<DEL>   suppress selection if non-null, else before/after selection
                r   replace with pattern elements and fields
              u/U   switch to lowercase/uppercase

Some of these commands are executed at most once per line, so that even if there
are two saved selections on a given line, running `>` will only indent that line
once.

See the search and replace engine section for more details on replacing.

### Managing lines

`edit` has a line-centric clipboard. The following commands does not act on
selections but directly on lines:

              y/Y   yank {m} lines/blocks
              d/D   delete {m} lines/blocks
              p/P   paste after/before {m} times
     SHIFT+arrows   move cursor line {m} lines up/down


## The search and replace engine

The search and replace engine is home-grown, so it needs some explanations. You
can read about the reasons of designing one in `philosophy.md`.

### Invocation

Using the engine should feel pretty straightforward as it can be used in a
typical `sed` fashion:

1. Choosing a line range with `%`, `.`, `b` or a custom range with `:`
2. Giving a search pattern with `/` or `f`
3. Giving a replace pattern with `r`

However, all these steps makes sense individually. As normal mode is entered
between two steps, you can see at any moment if the result differs from what you
expected, for example if you entered a bad line number in the custom range
prompt. You can then restart the process with no consequences.

Moreover, it means that you can use any combination of these steps. For example,
you can use replace without searching first, if you don't need subpatterns. This
way it unifies the replace process, wether it's a small replace of a word you
selected with the running selection or big search-and-replace operation across
the whole file.

### Search pattern syntax

The search pattern uses regular expressions, whose syntax is inspired by
PCRE[^1] and described below in a Backus-Naur form style. If you're new to
regular expressions, you might want to have a look at `regexp.md` for a more
detailed guide.

    e2: ASSERTION
      | CHARACTER
      | CHARACTER REP                   # match a character with a repeater
      | '\(' e1 '\)'                    # group for subpatterns reuse
      | '\(' e1 '\)' REP                # match a group with a repeater

    e1: e2
      | e1 e2                           # match the concatenation of 2 patterns
      | e1 '|' e2                       # match one of the 2 patterns

    ASSERTION: '^'                      # match the beginning of the line
             | '$'                      # match the end of the line
             | '\A'                     # match the beginning of the selection
             | '\Z'                     # match the end of the selection
             | '\b'                     # match word boundaries
             | '\B'                     # match non word boundaries
    
    CHARACTER: LITERAL                  # match the LITERAL only
             | '\' ESCAPED_LITERAL      # match to-be-escaped ESCAPED_LITERAL
             | CHARCLASS                # match characters in CHARCLASS 
             | '.'                      # match any character

    CHARCLASS: '[' LIST ']'             # match characters in LIST
             | '[^' LIST ']'            # match characters not in LIST
             | '\d'                     # match digits
             | '\D'                     # match non digits
             | '\w'                     # match word characters
             | '\w'                     # match non word characters
    
    LIST: LIST_MINUS                    # match literals in LIST_MINUS
        | '-' LIST_MINUS                # match '-' and literals in LIST_MINUS

    LIST_MINUS: LITERAL                 # match the LITERAL only 
              | LITERAL1 '-' LITERAL2   # match literals between the 2 
              | l1 l2                   # match literals in l1 or l2

    REP: '*'                            # 0 or more (any number)
       | '+'                            # 1 or more (at least once)
       | '?'                            # 0 or 1 (at most once)
       | '{' INTEGER '}'                # exactly INTEGER
       | '{' INTEGER ',' '}'            # at least INTEGER
       | '{' ',' INTEGER '}'            # at most INTEGER
       | '{' INTEGER1 ',' INTEGER2 '}'  # between INTEGER1 and INTEGER2    

[^1]: [PCRE](https://www.pcre.org/)

### Using subpatterns and fields in the replace pattern

The replace pattern can contain special sequences:

- `$0` and `\0` will be expended to the whole initial string
- `${i}` (1 <= {i} <= 9) will be expended to the {i}-th field of the initial
    string, where fields are delimited by the runtime-modifiable field separator
- `\{i}` (1 <= {i} <= 9) will be expended to the {i}-th subpattern matched by
    the search pattern in the initial string
- `\\` and `\$` produces repectively `\` and `$` (escaping sequences)

Here are some examples:

    INITIAL STRING      SEARCH PATTERN  REPLACE PATTERN RESULT
    "vim"               none            "edit"          "edit"
    "edit"              none            "$0 \$ \0"      "edit $ edit"
    "field 1,field 2"   none            "$2: $1"        "field 2: field 1"
    "murcielago"        "mur\(.{4}\)"   "\1 ($1)"       "ciel (murcielago)"


## Porting `edit`

Even if `edit` has no dependencies, it won't run everywhere, as the default
terminal drawing library (termbox[^2]) does not support all terminals. For
example, it won't run on Windows, unless you use WSL or a similar solution.

[^2]: [termbox2](https://github.com/termbox/termbox2)

All the interaction between `edit` and its environment happens through
`termbox.h`. Therefore if you want to embed the editor in your own (GPLv3)
software, make it work on Windows, or build a Graphical User Interface, all you
have to do is to replace `termbox.h` with a file adapted to your targetted
environment and providing the same API.
