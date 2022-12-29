/* text editor
 *
 * LIMITATIONS
 * assume screen is >= 81 chars wide
 * don't manage 81+ chars line
 *
 * TODO
 * getting to struct, separing chars
 * switch interface to ~normal line
 * move screen when moving cursor
 * macro for incr/decr buffer indexes
 * line number management, get back when reload/save
 * support for 80+ chars lines -> virtual lines (to manage)
 * error management
 * interface line management
 * check for screen height >= 2, width >= MAX_CHARS, else must fail beautifully in all cases
 *
 * write a version with C ptr management
 *  UTF-8 support
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


/* FLAGS */
#define TB_IMPL
#define MOUSE_SUPPORT               1

/* INCLUDES */
#include <stdio.h>
#include "termbox.h"

/* CONSTANTS */
#define MAX_CHARS                   81
#define SCROLL_LINE_NUMBER          3

/* ERROR CODES */
#define ERR_TERM_NOT_WIDE_ENOUGH    1
#define ERR_MISSING_FILE_NAME       2
#define ERR_MALLOC                  3
#define ERR_TOO_LONG_LINE           4


/* VARIABLES */
int screen_height, screen_width;
int empty = 1;
char interface_line[MAX_CHARS];


/* FUNCTIONS */

int set_y(int value);
int move_screen(int nlines);

/* memory management */
int *new_line(int line_byte_length);
int *length_line(int *ptr);
int *was_modified(int *ptr);
int *prev(int *ptr);
int *next(int *ptr);
int *chars(int *ptr);
void free_everything(int *first_line, int *last_line,
    int *first_screen_line, int *cursor_line);
void set_message(char *str);

/* file management */
int load_file(char *src_file_name, int *first_line, int *last_line,
    int *first_screen_line, int *cursor_line);
int write_file(char *dest_file_name, int *first_line);

/* graphical */
void print_cursor(void);
void hide_cursor(void);
void print_line(int *ptr, int screen_line);
void print_interface(void);
void print_screen(int *first_screen_line, int *last_line, int x, int y);



/* MAIN */

int
main(int argc, char *argv[])
{
    int HAS_BEEN_CHANGES = 0;
    int x, y;                               /* cursor position */
    int first_line = 0;
    int last_line = 0;
    int first_screen_line = 0;
    int cursor_line = 0;
    struct tb_event ev;

    int i;
    int c;

    /* INITIALISATION */
    tb_init();
    screen_height = tb_height();
    screen_width = tb_width();
    if (screen_width < MAX_CHARS) {
        fprintf(stderr, "terminal is not wide enough: %d\n", screen_width);
        return ERR_TERM_NOT_WIDE_ENOUGH;
    }
    if (MOUSE_SUPPORT)
        tb_set_input_mode(TB_INPUT_ESC | TB_INPUT_MOUSE);

    /* -- PARSING ARGUMENTS */
    if (argc == 1) {
        fprintf(stderr, "Missing file name.");
        return ERR_MISSING_FILE_NAME;
    } else {
        /* TODO: manage options */
    }

    /* -- INIT BUFFER */
    load_file(argv[argc - 1], &first_line, &last_line,
        &first_screen_line, &cursor_line);
    HAS_BEEN_CHANGES = 0;
    set_message("Welcome to vsed!");

    /* MAIN LOOP */
    while (1) {
        print_screen(&first_screen_line, &last_line, x, y);
        tb_present();
        tb_poll_event(&ev);
        if (ev.type == TB_EVENT_KEY) {
            /* TODO: manage modifiers
                (key XOR ch, one will be zero), mod. Note there is
                overlap between TB_MOD_CTRL and TB_KEY_CTRL_*.
                TB_MOD_CTRL and TB_MOD_SHIFT are only set as
                modifiers to TB_KEY_ARROW_*. */
            if (c = ev.ch) {
                /* TODO; c = ev.ch, ev.mod usable */
                if (c == 'q') {
                    /* QUIT */
                    /* TODO: check for need to save ? */
                    break;
                }
                interface_line[0] = c;
                interface_line[1] = '\0';
                /*if (c == 'd')
                    move_screen(1);
                if (c == 'u')
                    move_screen(-1);*/
                if (c == 'r') { /* reload */
                    free_everything(&first_line, &last_line,
                        &first_screen_line, &cursor_line);
                    load_file(argv[argc - 1], &first_line, &last_line,
                        &first_screen_line, &cursor_line);
                    HAS_BEEN_CHANGES = 0;
                    set_message("File reloaded.");
                }
                if (c == 'w') { /* save */
                    write_file(argv[argc - 1], &first_line);
                    HAS_BEEN_CHANGES = 0;
                    set_message("File saved.");
                }
                if (c == 't') { /* TESTING */
                }
            } else {
                /* TODO; ev.key, ev.mod usable */
                /*switch (ev.key) {
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
                }*/
            }
        } else if (ev.type == TB_EVENT_MOUSE) {
            /* TODO; ev.key, ev.x, ev.y usable */
            /* TB_KEY_MOUSE_{LEFT, RIGHT, MIDDLE, RELEASE, WHEEL_UP, WHEEL_DOWN} */
            /*if (ev.key == TB_KEY_MOUSE_LEFT) {
                if (ev.y < screen_height - 1) {
                    x = (ev.x < MAX_CHARS) ? ev.x : MAX_CHARS - 1 ;
                    if (set_y(ev.y))
                        print_cursor();
                } else if (ev.y == screen_height - 1) {
                    * TODO; check if in interface mode, etc *
                }
            } else if (ev.key == TB_KEY_MOUSE_WHEEL_UP) {
                move_screen(-SCROLL_LINE_NUMBER);
            } else if (ev.key == TB_KEY_MOUSE_WHEEL_DOWN) {
                move_screen(SCROLL_LINE_NUMBER);
            }*/
        } else if (ev.type == TB_EVENT_RESIZE) {
            /*if (ev.w < MAX_CHARS) {
                * TODO: error, must fail gracefully *
            }
            * TODO: cursor *
            screen_height = ev.h;
            screen_width = ev.w;
            if (y >= screen_height - 1)
                set_y(screen_height - 2);
            print_screen();*/
        }
    }

    /* CLOSING */
    tb_shutdown();

    return 0;
}
/*
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
                    * TODO: must reload to see upwards *
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
*/



/* MEMORY MANAGEMENT ***********************************************************/
 
/* LINE
 * int  line length
 * int  was_modified
 * ptr  prev
 * ptr  next
 * int[]characters */

int *
new_line(int line_byte_length)
{
    return malloc(2*sizeof(int) + 2*sizeof(int *) + line_byte_length);
}

int *
length_line(int *ptr)
{
    return ptr;
}

int *
was_modified(int *ptr)
{
    return ptr + sizeof(int);
}

int *
prev(int *ptr)
{
    return ptr + 2*sizeof(int);
}

int *
next(int *ptr)
{
    return ptr + 2*sizeof(int) + sizeof(int *);
}

int *
chars(int *ptr)
{
    return ptr + 2*sizeof(int) + 2*sizeof(int *);
}

void
free_everything(int *first_line, int *last_line,
    int *first_screen_line, int *cursor_line)
{
    int *old_ptr;
    int *ptr;

    if (~empty)
    {
        ptr = *first_line;
        while (ptr != *last_line) {
            old_ptr = ptr;
            ptr = *next(ptr);
            free(old_ptr);
        }
        free(ptr);
        empty = 1;
    }
    
    *first_line = *last_line = *first_screen_line = *cursor_line = NULL;
}

void
set_message(char *str)
{
    int i;
    char c;

    i = 0;
    while (c = *str++)
        interface_line[i++] = c;
    interface_line[i] = '\0';
}


/* FILE MANAGEMENT *************************************************************/

int
load_file(char *src_file_name, int *first_line, int *last_line,
    int *first_screen_line, int *cursor_line)
{
    FILE *src_file = NULL;
    int buf[MAX_CHARS];
    int *ptr;
    int i;
    int c;
    int reached_EOF;

    /* open connection to src_file */
    src_file = fopen(src_file_name, "r");
    reached_EOF = 0;
    empty = 1;
    *first_line = *last_line = *first_screen_line = *cursor_line = NULL;

    /* read content into memory */
    while (~reached_EOF) {
        i = 0;
        while (1) {
            if (i == MAX_CHARS) {
                return ERR_TOO_LONG_LINE;
            } else {
                /* TODO */
                buf[i++] = c = getc(src_file);
                if (c == EOF) {
                    reached_EOF = 1;
                    break;
                } else if (c == '\n') {
                    break;
                }
            }
        }
        /* store line */
        if ((ptr = new_line(i * sizeof(int))) == NULL)
            return ERR_MALLOC;
        *length_line(ptr) = i;
        *was_modified(ptr) = 0;
        if (empty) {
            empty = 0;
            *first_line = *last_line = *first_screen_line = *cursor_line = ptr;
            *prev(ptr) = ptr;
            *next(ptr) = ptr;
        } else {
            *prev(ptr) = *last_line;
            *next(ptr) = ptr;
            *last_line = ptr;
        }
        while (i--)
            *(chars(ptr) + i*sizeof(int)) = buf[i];
    }

    /* close connection to src_file */
    fclose(src_file);

    return 0;
}

int
write_file(char *dest_file_name, int *first_line)
{
    FILE *dest_file = NULL;
    int *ptr;
    int i;
    int c;

    /* open connection to dest_file */
    dest_file = fopen(dest_file_name, "w");

    /* write content to dest_file */
    ptr = *first_line;
    while (1) {
        for (i = 0; (c = *(chars(ptr) + i*sizeof(int))) != EOF && c != '\n'; i++)
            putc(c, dest_file);
        putc(c, dest_file);
        if (c == EOF) {
            break;
        } else {
            ptr = *next(ptr);
        }
    }

    /* close connection to dest_file */
    fclose(dest_file);

    return 0;
}


/* GRAPHICAL *******************************************************************

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

/* TODO: merge print_line and print_interface ? */
void
print_line(int *ptr, int screen_line)
{
    int i;
    char c;

    for (i = 0; (c = *(chars(ptr) + i*sizeof(int))) != '\n' && c != EOF; i++)
        tb_set_cell(i, screen_line, c, 0, 0);
    for (; i < screen_width; i++)
        tb_set_cell(i, screen_line, ' ', 0, 0);
}

void
print_interface(void)
{
    int i;
    char c;

    for (i = 0; (c = interface_line[i]) != '\0'; i++)
        tb_set_cell(i, screen_height - 1, c, 0, 0);
    for (; i < screen_width; i++)
        tb_set_cell(i, screen_height - 1, ' ', 0, 0);
}

void
print_screen(int *first_screen_line, int *last_line, int x, int y)
{
    int *ptr;
    int sc_line;

    ptr = *first_screen_line;

    /* clear screen */
    tb_clear();

    /* print lines */
    for (sc_line = 0; sc_line < screen_height - 1; sc_line++) {
        print_line(ptr, sc_line);
        if (ptr == *last_line)
            break;
        ptr = *next(ptr);
    }

     /* print interface */
    print_interface();

    /* print cursor */
    tb_set_cursor(x, y);
}
