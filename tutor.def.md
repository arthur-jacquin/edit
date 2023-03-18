


                      _ _ _       _         _             _       _
              ___  __| (_) |_    | |_ _   _| |_ ___  _ __(_) __ _| |
             / _ \/ _` | | __|   | __| | | | __/ _ \| '__| |/ _` | |
            |  __/ (_| | | |_    | |_| |_| | || (_) | |  | | (_| | |
             \___|\__,_|_|\__|    \__|\__,_|\__\___/|_|  |_|\__,_|_|



Hi and welcome in edit! This tutorial will explain all there is to know to use
edit. Despite it being quite exhaustive, it won't take more than 10 minutes, as
edit was designed to be simple to learn and use.

If you are reading this after running `make tutor`, this file is a copy of the
tutorial. That means you can (and should) try anything this file, you will find
an unmodified version of this tutorial each time you run `make tutor`.

Press j or the down arrow key until the first lesson fills the screen.



# PART 1: THE BASICS

## 1.1: visual elements

Run `edit path/to/file` to use edit. It will fill the terminal with three
elements, in a pretty common way for terminal text editors:

    +----------------------------------------------------------------------+
    |                                                                      |
    |                                                                      |
    |                                                                      |
    |                                                                      |
    |                                 FILE                                 |
    |                                                                      |
    |                                                                      |
    |                                                                      |
    |                                                                      |
    | INTERFACE                                                     RULER  |
    +----------------------------------------------------------------------+

All lines but the bottom line are used to display the FILE being edited. On the
bottom line of the screen, an INTERFACE displays messages for the user and at
the right, a RULER gives the position of the cursor (formatted {line}:{column}).

Press j or the down arrow key until the next lesson fills the screen.


## 1.2: basic movements

There are various ways to move the cursor. The most basic ones are the arrow
keys. Alternatively, the vi hjkl keys can be used:

    h/left arrow        move the cursor a character to the left
    j/down arrow        move the cursor to the following line
    k/up arrow          move the cursor to the previous line
    l/right arrow       move the cursor a character to the right

If the mouse is supported, a left click will move the cursor under the mouse.

Try to move the cursor through this lesson with the arrows, the hjkl keys and
the mouse.

When you are done with a lesson, move down in the file until the next one fills
the screen.


## 1.3: modal editing

After startup, the editor loop indefinitely:

1. Refresh the elements on the screen
2. Wait for an user event (keypress, mouse or terminal resize)
3. Process the event

The way an event is processed heavily depends on the mode in which the editor
is. There are three modes: normal, insert and dialog modes.

At startup, the editor is in normal mode, in which most keys are associated with
an editing command. The commands of this mode will be described later.

When in normal mode, you can press `i` to get in insert mode. There, most keys
will insert the associated character in the buffer. Press escape to get back to
normal mode.

Sometimes, the editor will need an user input, for example a search pattern. To
collect that input, the editor uses the dialog mode. In this mode, the user is
prompted something on the bottom line of the screen. The up arrow recovers the
last input associated with the prompt, and the down arrow clears the input.
Press enter to validate the input, or escape to cancel and get back to normal.


## 1.4: exiting

To exit the editor, make sure to be in normal mode by pressing escape, then
press `q`. If there are unsaved changes, the editor will display a warning and
abort.

Pressing `Q` skips the check for unsaved changes and exits in any case. Unsaved
changes are then lost.

Let's try this. Please read the entire process before starting.

1. Make sure you are in normal mode by pressing escape.
2. Place your cursor on this line and press d. It will delete the line.
3. Try to exit by pressing `q`. The unsaved deletion will abort exiting.
4. Press `Q` to ignore the warning and exit.
5. Run `make tutor` again and check that the deleted line reappeared.

Now you know the basics! Let's dive in the edition model.



# PART 2: THE EDITION MODEL

## 2.1: selections

A selection is a set of contiguous characters. It is located on a given line,
starts at a given character and has a given length.

Most of the editing commands acts on a list of selections. Selections are
therefore a central component, and are represented on the screen by a subtle
highlight.

Press `.` to select the cursor line. It creates a selection on the cursor line,
starting at the first character of the line and of the same length of the line.
Check that the cursor line is highlighted, then forget the selection by pressing
escape.

There are two selections lists: the running selections, and the saved
selections. The saved list is preferred to the running list, as commands will
act on the saved selections, unless there is none, in which case the commands
will act on the running selections.

For more elaborate editing, there are multiple ways to create saved selections,
but as there is no saved selections at startup, let's focus on the running
selections first.


## 2.2: simple editing

The fact that most commands act on a list of selection is powerful: it easily
enables multi-cursor and block/column editing, and integrates well with the
home-grown search and replace engine. But as of yet, it might look like a
complex abstraction that needlessly gets in the way for simple tasks.

Do not worry, it does not. By default, there is no saved selections and the
running selections list only contains the cursor: one selection, on the cursor
line, starting at the cursor column and of length zero. This way, commands only
act on the cursor and the editor behaves as one could expect with a standard
editor.

A basic command is character deletion. Pressing `x` deletes the character before
every selections. If you press `x` now, you will see it only deletes the
character before the cursor, as expected.


## 2.3: anchor

It is also possible to drop an anchor below the cursor. The running selections
list is then not reduced to a single, null-length selection under the cursor,
but contains all characters between the anchor and the cursor. As a selection
can only span on a given line, the list contains multiple selections if the
anchor and the cursor are not on the same line.

Press `v` to drop the anchor, then move the cursor to see the selected
characters. Press escape or `v` again to remove the anchor.

For a unified understanding of the running selections list, you can imagine that
the anchor follows the cursor when not anchored.


## 2.4: multiplier

It makes sense for some commands to repeat them a given number of times. Enter a
multiplier {m} before these commands to get the expected result. For those
commands, the default value of 1 is used when no multiplier were given.

Most of the movements commands combines well with a multiplier. Try to precede
hjkl with some multipliers to move faster.

You should be be ready to navigate by yourself in the commands walktrough!



# PART 3: COMMANDS WALKTHROUGH

## 3.1: introduction

This part covers all the commands available in normal mode and their default
keybind. Remember that you can get back to normal mode at any moment by pressing
escape. All of the following commands are condensed in `cheatsheet.md` for quick
reference.

Do not forget to test all of the commands while reading this file, as running
`make tutor` again will give you an unmodified version of the tutorial.

Some mechanisms need more in-depth explanations or custom training to master.
They are explored in the following parts.


## 3.2: interacting with the editor

                ?   display the help message
              q/Q   quit/force quit
                e   execute a shell command and get back to edit


## 3.3: managing the file

              w/W   write/write as
                R   reload

As there is no undo/redo mechanism (a choice explained in `philosophy.md`), the
user is advised to save frequently and reload the file when a set of actions is
to reverted. Warning: unsaved changes are lost when reloading.


## 3.4: switching to insert mode

                i   get in insert mode

Pressing `i` is the standard way to get in insert mode. For compatibility with
vi muscle memory, the following commands also get you in insert mode at a
specific location:

              I/A   at start/end of line
              o/O   on an empty line created below/above cursor line

However these commands do not combine well when applied to more than one
selection, therefore they empty the saved selections list and unanchor the
anchor before switching to insert mode.


## 3.4: changing a setting

                s   change a setting

Changing a setting prompts for a modification. This modification must be
formatted as "{name}={value}" where {name} is in the table below and {value} is
of the according type. Any integer value is considered a boolean, 0 meaning
FALSE and any other value meaning TRUE.

    SETTING                 NAME    TYPE    DEFAULT VALUE
    syntax highlight        sh      bool    TRUE
    highlight selections    h       bool    TRUE
    case sensitive          c       bool    TRUE
    field separator         fs      char    ,
    tab width               tw      int     4
    language                l       string  (from extension)


## 3.5: moving around

                m   matching bracket
            0/^/$   start/first non-blank character/end of line
              g/G   line {m}/last line of the file
      arrows, l/h   {m} next/previous character
      arrows, j/k   {m} next/previous line
              t/T   {m} next/previous word
              }/{   {m} next/previous block
              n/N   {m} next/previous selection
                J   jump to {m} next match for prompted pattern

Use `J` to navigate quickly through the file without changing selections.


## 3.6: managing selections

            <ESC>   forget saved selections, unanchor, reset multiplier
                v   anchor/unanchor
                a   add running selections to saved selections; unanchor
                z   duplicate the running selection on the {m} following lines

The command associated to keybind `z` was designed for easy block/column
editing. Given a single line running selection, it will add to the saved list
selections on the m following lines corresponding to same characters of the
line.

                c   display number of saved selections
                .   select cursor line
            %/b/:   select all lines of file/{m} following blocks/custom range

When selecting a custom range, the user is prompted a range that must be
formatted as "{min},{max}" where {min} and {max} are either empty, a dot or an
integer. If needed, empty is converted in 1 (for {min}) or the line number of
the last line (for {max}), and dots are converted in the cursor line number, in
order to select any line whose number is between {min} and {max}.

             f, /   search for pattern
                *   search for the word that is currently under the cursor

Searching will create a new saved selections list containing all substrings of
previously saved selections matching the given pattern. Note that searching is
incremental: the potential results will be displayed as you type your search
pattern. See the search and replace engine section for more details on
searching.


## 3.7: acting on selections

In insert mode, most keys will insert the associated character before
selections. But acting on selections in normal mode is also possible. Some of
the following actions are executed at most once per line, so that even if there
are two saved selections on a given line, running `>` will only indent that line
once.

          x/<DEL>   suppress selection content if any, else char. before/after 
              u/U   switch to lowercase/uppercase
    >/<, TAB/^TAB   {m} increase/decrease line indent
                K   comment/uncomment line
                r   replace with pattern elements and fields

See the search and replace engine section for more details on replacing.

           CTRL+A   autocomplete at the end of each selection

If enabled, autocompletion is a special action on selections, as it can be done
in both normal and insert mode. This is what explains the need for a modifier.
See the autocomplete section for more details.


## 3.8: managing lines

edit has a line-centric clipboard. The following commands does not act on
selections but directly on lines:

              y/Y   yank {m} lines/blocks, starting at cursor
              d/D   delete {m} lines/blocks, starting at cursor
              p/P   paste after/before cursor line {m} times
     SHIFT+arrows   move cursor line {m} lines up/down



# PART 4: AUTOCOMPLETION

Hitting CTRL + A performs autocompletion at the end of each selection. If the
end of a selection does not follow the start of a word, the completion is
aborted. Else the word will be completed by the first word in the file that
starts identically and is strictly longer. Let's try that.

Imagine you want to write the very_long_name_of_a_function defined somewhere in
the file. You already typed the start of the name. You can try to hit CTRL + A.
Move your cursor at the end of the following lines and try that:

    v
    very

Note that autocompleting on "v" (and "ve", and "ver") gives "version", as it is
the first match in the file. Sometimes autocompletion won't give you the word
you want if it is ambiguous.



# PART 5: THE SEARCH AND REPLACE ENGINE

The search and replace engine is home-grown, so it needs some explanations. You
can read about the reasons of designing one in `philosophy.md`.


## 5.1: use

Using the engine should feel pretty straightforward as it can be used in a
typical sed fashion:

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


## 5.2: search pattern syntax

The search pattern uses regular expressions, whose syntax is inspired by
PCRE[^1] and described in `cheatsheet.md` in a Backus-Naur form style.

[^1]: https://www.pcre.org/

If you're new to regular expressions, reading the syntax description might not
be sufficient. If you do not want to learn how to use regular expressions, you
just have to know that some characters needs to be escaped with an antislash (\)
in the search patterns: ., \, ^, $, |, *, +, ?, {, and [.

Please note that the regular expression engine is not perfect. For example,
repeaters always match as much as possible, potentially eating too much
characters and invalidating a valid match.


## 5.3: replace pattern syntax

The replace pattern can contain special sequences:

- `$0` and `\0` will be expended to the whole initial selection
- `${i}` (1 <= {i} <= 9) will be expended to the {i}-th field of the initial
    selection, where fields are delimited by the runtime-modifiable field
    separator
- `\{i}` (1 <= {i} <= 9) will be expended to the {i}-th subpattern matched by
    the search pattern in the initial selection
- `\\` and `\$` produces repectively `\` and `$` (escaping sequences)



# PART 6: REGULAR EXPRESSIONS

## 6.1: introduction

A regular expression is a pattern (string) describing a set of strings.

From now on, the shorter name "regex" will be used to refer to a regular
expression. A regex is said to "match" a string when the string is in the
set of strings described by the regex.

The simplest regexes, the ones that do not use any special feature, describe a
set reduced to a single string: themselve. For example, the regex "edit" only
matches the string "edit", and the regex "murcielago" only matches the string
"murcielago".

The whole point of regex expressions is to provide a syntax to easily describes
sets of structurally similar strings. Let's explore these different syntax
elements with examples. For each exemple, check what is asserted.


## 6.2: syntax elements

### 6.2.1: assertions

Some elements mark a specific position: `^` (resp. `$`) marks a line start
(resp. end), `\A` (resp. `\Z`) marks a selection start (resp. end). Let's
consider the following line:

    This is a file. This is the first line of this file.
              -----                                -----
                A                                    B

Substring B is matched by both "file." and "file.$" regexes, but only the former
matchs substring A.

Finally, `\b` (resp. `\B`) marks a word boundary (resp. not a word boundary). A
word boundary is the transition between a word character and a non-word
character.

This is particularly helpful for searching the occurrences of a specific
variable in code: searching "i" in "print(i)" will match both "i", while "\bi\b"
only matches the second one. In edit, the `*` command automatically delimits the
word under the cursor and search for it, padded with `\b` delimiters. Try to
select all the occurences of "the" in this paragraph with this method. Be
careful to not select this one: therefore.

### 6.2.2: characters

The simplest way to match a character is to use the character in the regex.

But some characters have a special meaning in a regex. We previously saw that
`$` marks an end of line. These characters needs to be "escaped" with an
antislash. For example, you can use the regex `\$` to match `$`, and `\\` to
match `\`. The characters to escape are the following:

    \ ^ $ | * + ? { [ .

Some syntax elements refers to classes of characters. For example, `\d` will
match any digit: `0`, `1` and all the way to `9`. `\D` matches non digits, and
`\w` (resp. `\W`) matches word characters (resp. non-word characters).

For more specific needs, you can create custom classes of characters. These
classes are characters lists encapsulated in square bracket. The potential `^`
symbol at the beginning of the list indicates that the class matches characters
that are NOT in the rest of the list; else the class matches any characters in
the list. Let's see some examples:

    "[abc]" matches "a" and "c" but not "e"
    "[^abc]" matches "e" but not "a" and "c"

You can also use ranges in the list:

    "[a-z]" matches any lowercase letter
    "[^5-9]" matches "a" and "4" but neither "5" nor "8"

Use `-` at the begininng of the list so that it is not interpreted as a range:

    "[-09]" matches "-", "0" and "9" while "[0-9]" is equivalent to "\d"
    "[^-a-z]" matches all characters but "-" and lowercase letters

Finally you can combine all of these elements. For example, `[^-a-cX-Z02468]`
matches all characters but `-`, `a`, `b`, `c`, `X`, `Y`, `Z` and even digits.

You can use the following to train your characters matching ability:

    a b c d e f g h i j k l m n o p q r s t u v w x y z
    A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
    0 1 2 3 4 5 6 7 8 9 _ - / : & # % µ ! " @

### 6.2.3: repeaters

You can follow a character expression by a repeater. A repeater specifies how
many times the preceding character should be matched:

* `*` means 0 or more times (any number of times)
* `+` means 1 or more times (at least once)
* `?` means 0 or 1 time (at most once)
* `{n}` means n times
* `{min,}` means at least min times
* `{,max}` means at most min times
* `{min,max}` means between min and max times

Let's see some examples:

    "a*" matches "", "a" and "aaa"
    "a+" matches "a" and "aaa" but not ""
    "a?" matches "" and "a" but not "aaa"
    "a{5}" only matches "aaaaa"
    "a{1,3}" only matches "a", "aa" and "aaa"

You can use the following to train your repeaters use:

    a aa aaa aaaa aaaaa aaaaaa aaaaaaa


## 6.3: building a pattern

Now that you read about all the syntax elements supported by edit, let's see how
to build a pattern.

### 6.3.1: concatenation

The simplest rule is concatenation. Just concatenate regexes to create a new
one, that matches all possible concatenations of matches. For example, as "a"
matches "a" and "\d" matches "0", the concatenated regex "a\d" matches "a0".

### 6.3.2: OR logic

The `|` symbol is a logical OR between the two adjacent elements:

    "\D|6" does not match digits except "6"
    "a+|b" matches "a", "aaa" and "b"

### 6.3.3: grouping

You can group characters of the regex in blocks with delimiters `\(` and `\)`.
It might be useful for several purposes.

First, blocks also support repeaters and OR logic:

    "a\(ba\){4}" only matches "ababababa"
    "\(apple\)|\(orange\)" only matches "apple" and "orange"

Lastly, characters matched by this portion of the regex can be reused in a
replace pattern, which is very powerful.
