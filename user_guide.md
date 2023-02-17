## User guide

The interface (file, dialog interface, ruler). Should not surprise vim users

Main idea:
1. Init the editor (parse command line arguments, read the file...)
2. Loop
    1. Refresh the screen
    2. Wait for an user event (keypress, mouse, terminal resize)
    3. Process the event

Modal editor.

when in dialog, get the last validated input with arrow up, or clear the input
with arrow down.

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
- some are runtime: list
- some are compile time (as i find it makes no sense to change them at runtime): list
    colorscheme
    ...
You can modify settings in config.h, such as:
- mouse support, scroll line number...
- default values for parameters
- 8 or 256 colors modes, colorscheme
- keybinds
- ...
If there is no config.h at compile time, a default configuration is loaded
from config.def.h.

Language support:
- syntax (simplified rules)
- comment/uncomment
- ...
Support few languages by default. See hacker_guide.md to add a language.
Some languages are ready to be added at TODO.

The dialog mode
