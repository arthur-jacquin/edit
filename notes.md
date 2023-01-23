Remove all the go_to(pos_of(..., just attribute new asked y and x, and go_to(pos_of(x, y)) at start of main loop
check regex.h
internalize termbox

int
dialog(const char *prompt, const char *specifics, int writable)
{
    dialog_mode = 1;
    if (!writable)
        tb_hide_cursor();

}

-------------------------------------------------------------------------------
ESC
ENTER
ARROWS
MOUSE
RESIZE
BACKSPACE
DELETE
<char>

* [W] write as
* [s] change a parameter
* [R] replace with pattern elements and fields
* [e] execute command
NEED FOR INPUT, PROCESS ON EACH EVENT
* [:] select all lines of custom range
* [f,/] search for pattern
