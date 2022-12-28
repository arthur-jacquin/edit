/* text editor
 *
 * LIMITATIONS
 * assume screen is >= 81 chars wide
 * don't manage 81+ chars line
 *
 *  DIFFERENT management of forgotten lines -> directly write to output file
 *
 * TODO
 * HAS BEEN CHANGES
 * move screen when moving cursor
 * save file
 * support for 80+ chars lines -> virtual lines (to manage)
 * error management
 * interface line management
 * check for screen height >= 2, width >= MAX_CHARS, else must fail beautifully in all cases
 *
 * get rid of termbox.h
 * write a version with C ptr management
 *  only save real line length -> automatic support for 80+ chars lines
 *  manage copy/paste, variables
 *
 * add features
 * check correctness of all line fields and variables in any circumstances
 * chase unstated assumptions
 * restructuring, cleaning, commenting code
 * documentation
 * publish
 */

/* BUFFERS
 *
 * screen
 * graphical buffer
 * lines buffer
 * files
 */


/* INCLUDES */

#include <stdio.h>
/* termbox */
#define TB_IMPL
#include "termbox.h"


/* CONSTANTS */

#define BUFFER_SIZE (1 << 8)            /* will be 1 << 11 */
#define MAX_CHARS 81
#define MOUSE_SUPPORT 1
#define SCROLL_LINE_NUMBER 2

#define UNSAVED_CHANGES 1


/* TYPES */

struct line {
    int was_modified;
    int prev;
    int next;
    int chars[MAX_CHARS];
};


/* VARIABLES */

int screen_height, screen_width;
int x, y;                               /* cursor position */

FILE *src_file = NULL;                  /* input file */
FILE *dest_file = NULL;                 /* output file */

struct line buf[BUFFER_SIZE];           /* line buffer */
char interface_line[MAX_CHARS];         /* message */

int buffer_first_line;
int buffer_last_line;
int logical_first_line;
int logical_last_line;
int first_line_on_screen;
int cursor_line;

int number_of_forgotten_lines;
int number_of_eaten_lines;
int reached_EOF;
int empty_buffer;
int HAS_BEEN_CHANGES;


/* FUNCTIONS */

/* file management */
void load_file(char *src_file_name, char *dest_file_name, int nb_to_forget);
void write_file(char *src_file_name, char *dest_file_name, int APPLY_CHANGES);

/* buffer management */
int buffer_is_full(void);               /* TODO: replaced by a macro ? */
void move_line(int src, int dest);
void suppress_line(int line);
int pop_line(char *dest_file_name);
int eat_line(char *dest_file_name);
/* int load_lines(int nlines); */
/* int add_line(int was_modified, int prec, int next, int *chars); */

/* buffer.h */
int set_y(int value);
int move_screen(int nlines);

/* graphical.h */
void print_cursor(void);
void hide_cursor(void);
void print_line(int line_index, int screen_line);
void print_interface(const char *str);
void print_screen(void);



/* MAIN */

int
main(int argc, char *argv[])
{
    struct tb_event ev;
    int i;
    int c;

    /* INITIALISATION */
    tb_init();
    screen_height = tb_height();
    screen_width = tb_width();
    if (screen_width < MAX_CHARS) {
        /* TODO: error, fail with error message */
        printf("terminal is not wide enough: %d\n", screen_width);
        return 1;
    }
    if (MOUSE_SUPPORT)
        tb_set_input_mode(TB_INPUT_ESC | TB_INPUT_MOUSE);

    /* -- PARSING ARGUMENTS */
    if (argc == 1) {
        fprintf(stderr, "Missing argument.");
        return 1;
    } else {
        /* TODO: manage options */
    }

    /* -- INIT BUFFER */
    load_file(argv[argc - 1], "output", 0);

    /* -- INIT INTERFACE */
    interface_line[0] = 'W';
    interface_line[1] = '\n';
    print_screen();

    /* MAIN LOOP */
    while (1)
    {
        tb_present();
        tb_poll_event(&ev);
        if (ev.type == TB_EVENT_KEY) {
            /* TODO: manage modifiers
                (key XOR ch, one will be zero), mod. Note there is
                overlap between TB_MOD_CTRL and TB_KEY_CTRL_*.
                TB_MOD_CTRL and TB_MOD_SHIFT are only set as
                modifiers to TB_KEY_ARROW_*. */
            if (c = ev.ch) {
                /* TODO; ev.ch, ev.mod usable */
                if (c == 'q') {
                    /* QUIT */
                    /* TODO: check for need to save ? */
                    break;
                }
                if (c == 'd')
                    move_screen(1);
                if (c == 'u')
                    move_screen(-1);

                interface_line[0] = c;
                interface_line[1] = '\n';
                print_interface(interface_line);
            } else {
                /* TODO; ev.key, ev.mod usable */
                switch (ev.key) {
                    case TB_KEY_ARROW_UP:
                        if (set_y(y - 1))
                            print_cursor();
                        break;
                    case TB_KEY_ARROW_DOWN:
                        if (set_y(y + 1))
                            print_cursor();
                        break;
                    case TB_KEY_ARROW_LEFT:
                        if (x > 0) {
                            x--;
                            print_cursor();
                        }
                        break;
                    case TB_KEY_ARROW_RIGHT:
                        if (buf[cursor_line].chars[x] != '\n') {
                            x++;
                            print_cursor();
                        }
                        break;
                }
            }
        } else if (ev.type == TB_EVENT_MOUSE) {
            /* TODO; ev.key, ev.x, ev.y usable */
            /* TB_KEY_MOUSE_{LEFT, RIGHT, MIDDLE, RELEASE, WHEEL_UP, WHEEL_DOWN} */
            if (ev.key == TB_KEY_MOUSE_LEFT) {
                if (ev.y < screen_height - 1) {
                    x = (ev.x < MAX_CHARS) ? ev.x : MAX_CHARS - 1 ;
                    if (set_y(ev.y))
                        print_cursor();
                } else if (ev.y == screen_height - 1) {
                    /* TODO; check if in interface mode, etc */
                }
            } else if (ev.key == TB_KEY_MOUSE_WHEEL_UP) {
                move_screen(-SCROLL_LINE_NUMBER);
            } else if (ev.key == TB_KEY_MOUSE_WHEEL_DOWN) {
                move_screen(SCROLL_LINE_NUMBER);
            }
        } else if (ev.type == TB_EVENT_RESIZE) {
            if (ev.w < MAX_CHARS) {
                /* TODO: error, must fail gracefully */
            }
            /* TODO: cursor */
            screen_height = ev.h;
            screen_width = ev.w;
            if (y >= screen_height - 1)
                set_y(screen_height - 2);
            print_screen();
        }
    }

    /* CLOSING */
    tb_shutdown();
    if (src_file != NULL)
        fclose(src_file);
    return 0;
}

int
move_screen(int nlines)
{
    int i;

    if (nlines > 0) {
        for (i = 0; i < nlines; i++) {
            if (first_line_on_screen == logical_last_line) {
                if (reached_EOF) {
                    return 0;
                } else {
                    eat_line("output");
                }
            }
            first_line_on_screen = buf[first_line_on_screen].next;
        }
        if (y - nlines >= 0) {
            y = y - nlines;
        } else {
            cursor_line = first_line_on_screen;
            y = 0;
            for (i = x; i >= 0; i--)
                if (buf[cursor_line].chars[i] == '\n')
                    x = i;
        }
    } else if (nlines < 0) {
        for (i = 0; i > nlines; i--) {
            if (first_line_on_screen == logical_first_line) {
                if (number_of_forgotten_lines > 0) {
                    /* TODO: must reload to see upwards */
                    return 0;
                } else {
                    return 0;
                }
            }
            first_line_on_screen = buf[first_line_on_screen].prev;
        }
        if (y - nlines < screen_height - 1) {
            y = y - nlines;
        } else {
            cursor_line = first_line_on_screen;
            for (i = 0; i < screen_height - 2; i++)
                cursor_line = buf[cursor_line].next;
            y = screen_height - 2;
            for (i = x; i >= 0; i--)
                if (buf[cursor_line].chars[i] == '\n')
                    x = i;
        }
    }
    print_screen();
    return 0;
}

int
set_y(int value)
{
    int i;

    if (value < 0 || screen_height - 1 <= value) {
        return 0;
    } else {
        if (value > y) {
            for (; y < value; y++)
                if (cursor_line == logical_last_line) {
                    break;
                } else {
                    cursor_line = buf[cursor_line].next;
                }
        } else if (value < y) {
            for (; y > value; y--)
                cursor_line = buf[cursor_line].prev; 
        }
        for (i = x; i >= 0; i--)
            if (buf[cursor_line].chars[i] == '\n')
                x = i;
        return 1;
    }
}




/* FILE MANAGEMENT *************************************************************/

void
load_file(char *src_file_name, char *dest_file_name, int nb_to_forget)
{
    /* connection to src_file is closed */
    /* opens connection to src_file */

    int i;
    int c;

    /* open connection to src_file */
    src_file = fopen(src_file_name, "r");
    HAS_BEEN_CHANGES = 0;
    reached_EOF = 0;

    /* empty buffer */
    buffer_first_line = buffer_last_line = 0;
    empty_buffer = 1;

    /* ignore lines to forget */
    i = 0;
    while (i < nb_to_forget)
        if (getc(src_file) == '\n')
            i++;
    number_of_forgotten_lines = number_of_eaten_lines = nb_to_forget;
 
    /* initialise cursor */
    x = y = 0;

    /* load some lines */
    for (i = 0; i < screen_height - 1 && ~reached_EOF; i++)
        eat_line(dest_file_name);
}

void
write_file(char *src_file_name, char *dest_file_name, int APPLY_CHANGES)
{
    /* connection to dest_file is opened */
    /* connection to src_file is opened, unless EOF has been reached */
    /* closes both connections */

    int current_line;
    int n, i;
    int c;

    if (HAS_BEEN_CHANGES) {
        if (APPLY_CHANGES) {
            /* write buffer to dest */
            current_line = logical_first_line;
            while (current_line != logical_last_line) {
                for (i = 0; (c = buf[current_line].chars[i]) != '\n'; i++)
                    putc(c, dest_file);
                putc('\n', dest_file);
                current_line = buf[current_line].next;
            }
            for (i = 0; (c = buf[logical_last_line].chars[i]) != '\n'; i++)
                putc(c, dest_file);
            putc('\n', dest_file);
        } else {
            /* (re)open src file */
            if (~reached_EOF)
                fclose(src_file);
            src_file = fopen(src_file_name, "r");

            /* read and do nothing of forgotten lines */
            n = 0;
            while (n < number_of_forgotten_lines)
                if (c == '\n')
                    n++;
        }
        /* write end of src file to dest */
        while ((c = getc(src_file)) != EOF)
            putc(c, dest_file);
        putc(EOF, dest_file);

        /* exchange src and dest modes of connections */
        fclose(src_file);
        fclose(dest_file);
        src_file = fopen(src_file_name, "w");
        dest_file = fopen(dest_file_name, "r");

        /* write entire dest to src */
        while ((c = getc(dest_file)) != EOF)
            putc(c, src_file);
        putc(EOF, src_file);
    }
    /* close connections */
    if (src_file != NULL)
        fclose(src_file);
    fclose(dest_file);
}


/* BUFFER MANAGEMENT ***********************************************************/

int
buffer_is_full(void)
{
    return empty_buffer == 0 && buffer_first_line == buffer_last_line;
}

void
move_line(int src, int dest)
{
    int i;
    char c;

    /* refresh was_modified */
    buf[dest].was_modified = buf[src].was_modified;

    /* refresh chars; assumes '\n' is in line */
    for (i = 0; (c = buf[src].chars[i]) != '\n'; i++)
        buf[dest].chars[i] = c;
    buf[dest].chars[i] = '\n';

    /* refresh pointers */
    if (src == logical_first_line) {
        buf[dest].prev = dest;
        logical_first_line = dest;
    } else {
        buf[dest].prev = buf[src].prev;
        buf[buf[src].prev].next = dest;
    }
    if (src == logical_last_line) {
        buf[dest].next = dest;
        logical_last_line = dest;
    } else {
        buf[dest].next = buf[src].next;
        buf[buf[src].next].prev = dest;
    }
    if (first_line_on_screen == src)
        first_line_on_screen = dest;
    if (cursor_line == src)
        cursor_line = dest;
}

void
suppress_line(int line)
{
    move_line(buffer_first_line, line);
    buffer_first_line = (buffer_first_line + 1)%BUFFER_SIZE;
}

int
pop_line(char *dest_file_name)
{
    /* connection to dest_file can be opened or closed */

    int old_logical_first_line;
    int i;
    int c;

    if (buffer_is_full()) {
        if (buf[logical_first_line].was_modified) {
            return UNSAVED_CHANGES;
        } else {
            /* open dest_file if needed */
            if (dest_file == NULL)
                dest_file = fopen(dest_file_name, "w");
            
            /* push logical_first_line to dest_file */
            for (i = 0; (c = buf[logical_first_line].chars[i]) != '\n'; i++)
                putc(c, dest_file);
            putc('\n', dest_file);

            /* suppress logical_first_line */
            old_logical_first_line = logical_first_line;
            logical_first_line = buf[logical_first_line].next;
            buf[logical_first_line].prev = logical_first_line;
            suppress_line(old_logical_first_line);
            number_of_forgotten_lines++;

            return 0;
        }
    }
    return 0;
}

int
eat_line(char *dest_file_name)
{
    /* connection to src_file is opened */
    /* close connection to src_file on reaching EOF */

    int i;
    char c;
    int working_line;

    /* make sure of the existence of an empty slot */
    if (pop_line(dest_file_name)) {
        return UNSAVED_CHANGES;
    }

    /* empty slot to fill */
    working_line = (empty_buffer) ? 0 : buffer_last_line;
    buffer_last_line = (working_line + 1)%BUFFER_SIZE;

    /* read characters */
    for (i = 0; ; i++) {
        if (i == MAX_CHARS) {
            /* TODO: manage line longer than MAX_CHARS chars */
        } else {
            c = getc(src_file);
            buf[working_line].chars[i] = (c == EOF) ? '\n' : c;
            if (c == EOF) {
                reached_EOF = 1;
                fclose(src_file);
                src_file = NULL;
                break;
            } else if (c == '\n') {
                break;
            }
        }
    }

    /* manage pointers */
    number_of_eaten_lines++;
    buf[working_line].was_modified = 0;
    if (empty_buffer) {
        buffer_first_line = logical_first_line = working_line;
        empty_buffer = 0;
        first_line_on_screen = cursor_line = working_line;
        buf[working_line].prev = working_line;
    } else {
        buf[logical_last_line].next = working_line;
        buf[working_line].prev = logical_last_line;
    }
    buf[working_line].next = working_line;
    logical_last_line = working_line;

    return 0;
}


/* GRAPHICAL *******************************************************************

    tb_printf(0, y++, 0, 0, "event type=%d key=%d ch=%c", ev.type, ev.key, ev.ch);
    tb_present();
    tb_poll_event(&ev);

int tb_init();
int tb_shutdown();

int tb_width();
int tb_height();

int tb_clear();
int tb_present();

int tb_set_cursor(int cx, int cy);
int tb_hide_cursor();

int tb_set_cell(int x, int y, uint32_t ch, uintattr_t fg, uintattr_t bg);

int tb_poll_event(struct tb_event *event);

int tb_print(int x, int y, uintattr_t fg, uintattr_t bg, const char *str);
int tb_printf(int x, int y, uintattr_t fg, uintattr_t bg, const char *fmt, ...);

*******************************************************************************/

void
print_cursor(void)
{
    tb_set_cursor(x, y);
}

void
hide_cursor(void)
{
    tb_hide_cursor();
}

void
print_line(int line_index, int screen_line)
{
    int i;
    char c;

    for (i = 0; (c = buf[line_index].chars[i]) != '\n'; i++)
        tb_set_cell(i, screen_line, c, 0, 0);
    for (; i < screen_width; i++)
        tb_set_cell(i, screen_line, ' ', 0, 0);
}

void
print_interface(const char *str)
{
    tb_print(0, screen_height - 1, 0, 0, str);
}

void
print_screen(void)
{
    int line_index, y;

    line_index = first_line_on_screen;

    /* clear screen */
    tb_clear();

    /* print lines */
    for (y = 0; y < screen_height - 1; y++) {
        print_line(line_index, y);
        if (line_index == logical_last_line) {
            if (reached_EOF) {
                break;
            } else {
                eat_line("output");
            }
        }
        line_index = buf[line_index].next;
    }

    /* print interface */
    print_interface(interface_line);

    /* print cursor */
    print_cursor();
}
