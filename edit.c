/* TASKS
 *
 * add features
 * check correctness of all line fields and variables in any circumstances
 * chase unstated assumptions, possibility of failure
 * improve error management, assure safe and graceful fails
 * restructuring, cleaning, commenting code
 * documentation
 * publish
 */

/* TODO
 *
 * remove duplicity between y and "cursor_line - first_screen_line"
 * type cast, correct printing
 * get back when reload/save
 * add dialog mode
 * add insert mode
 * add line management
 * manage malloc error
 * str alike management (strcpy...)
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
#define MAX_CHARS                   (1 << 10)
#define MIN_WIDTH                   81
#define SCROLL_LINE_NUMBER          3

/* ERROR CODES */
#define ERR_TERM_NOT_BIG_ENOUGH     1
#define ERR_MISSING_FILE_NAME       2
#define ERR_MALLOC                  3
#define ERR_TOO_LONG_LINE           4
#define ERR_INVALID_LINE_VALUE      5

/* VARIABLES */
int screen_height, screen_width;
int empty = 1;

/* LINE STRUCT */
struct line {
    int line_nb;
    int length;
    int was_modified;
    struct line *prev;  /* ptr to prev line struct */
    struct line *next;  /* ptr to next line struct */
    char *chars;        /* ptr to characters */
};

/* MEMORY MANAGEMENT FUNCTIONS */
struct line *new_line(int line_nb, int length, int was_modified);
void free_line(struct line *ptr);
void free_everything(struct line **first_line, struct line **last_line,
    struct line **first_screen_line, struct line **cursor_line);

/* FILE MANAGEMENT FUNCTIONS */
int load_file(char *src_file_name, int *nb_line,
    struct line **first_line, struct line **last_line,
    struct line **first_screen_line, struct line **cursor_line);
int write_file(char *dest_file_name,
    struct line *first_line, struct line *last_line);

/* GRAPHICAL FUNCTIONS */
void print_line(char *chars, int screen_line);
void print_screen(struct line *first_screen_line, struct line *last_line,
    char *buf, int x, int y, int line_nb);

/* UGLY STUFF */
int move_screen(int nlines, int *x, int *y,
    struct line *first_line, struct line *last_line,
    struct line **first_screen_line, struct line **cursor_line);
void move_cursor_line(int line_nb, struct line **cursor_line);
int set_y(int value);
int set_x(int value, int line_length);



/* MAIN */

int
main(int argc, char *argv[])
{
    char file_name[MIN_WIDTH];
    int HAS_BEEN_CHANGES = 0;
    int x, y;                               /* cursor position */
    struct line *first_line = NULL;
    struct line *last_line = NULL;
    struct line *first_screen_line = NULL;
    struct line *cursor_line = NULL;
    struct tb_event ev;
    char interface_line[MIN_WIDTH];
    int nb_line;

    int i;
    uint32_t c;

    /* INITIALISATION */
    tb_init();
    screen_height = tb_height();
    screen_width = tb_width();
    if (screen_width < MIN_WIDTH) {
        fprintf(stderr, "Terminal is not wide enough: %d\n", screen_width);
        return ERR_TERM_NOT_BIG_ENOUGH;
    } else if (screen_height < 2) {
        fprintf(stderr, "Terminal is not high enough: %d\n", screen_height);
        return ERR_TERM_NOT_BIG_ENOUGH;
    }
    if (MOUSE_SUPPORT)
        tb_set_input_mode(TB_INPUT_ESC | TB_INPUT_MOUSE);

    /* -- PARSING ARGUMENTS */
    if (argc == 1) {
        fprintf(stderr, "Missing file name.");
        return ERR_MISSING_FILE_NAME;
    } else {
        /* TODO: manage options */
        strcpy(file_name, argv[argc - 1]);
    }

    /* -- INIT BUFFER */
    load_file(file_name, &nb_line, &first_line, &last_line,
        &first_screen_line, &cursor_line);
    HAS_BEEN_CHANGES = 0;
    strcpy(interface_line, "Welcome to vsed!");
    x = y = 0;

    /* MAIN LOOP */
    while (1) {
        print_screen(first_screen_line, last_line, interface_line,
            x, y, cursor_line->line_nb);
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
                if (c == 'd')
                    move_screen(1, &x, &y, first_line, last_line,
                        &first_screen_line, &cursor_line);
                if (c == 'u')
                    move_screen(-1, &x, &y, first_line, last_line,
                        &first_screen_line, &cursor_line);
                if (c == 'r') { /* reload */
                    free_everything(&first_line, &last_line,
                        &first_screen_line, &cursor_line);
                    load_file(file_name, &nb_line, &first_line, &last_line,
                        &first_screen_line, &cursor_line);
                    HAS_BEEN_CHANGES = 0;
                    strcpy(interface_line, "File reloaded.");
                }
                if (c == 'w') { /* save */
                    write_file(file_name, first_line, last_line);
                    HAS_BEEN_CHANGES = 0;
                    strcpy(interface_line, "File saved.");
                }
                if (c == 't') { /* TESTING */
                }
            } else {
                /* TODO; ev.key, ev.mod usable */
                switch (ev.key) {
                    case TB_KEY_ARROW_UP:
                        if (y > 0) {
                            y--;
                            cursor_line = cursor_line->prev;
                        } else if (cursor_line != first_line) {
                            first_screen_line = cursor_line = first_screen_line->prev;
                        }
                        x = set_x(x, cursor_line->length);
                        break;
                    case TB_KEY_ARROW_DOWN:
                        if (y < screen_height - 2) {
                            y++;
                            cursor_line = cursor_line->next;
                        } else if (cursor_line != last_line) {
                            first_screen_line = first_screen_line->next;
                            cursor_line = cursor_line->next;
                        }
                        x = set_x(x, cursor_line->length);
                        break;
                    case TB_KEY_ARROW_LEFT:
                        x = set_x(x - 1, cursor_line->length);
                        break;
                    case TB_KEY_ARROW_RIGHT:
                        x = set_x(x + 1, cursor_line->length);
                        break;
                }
            }
        } else if (ev.type == TB_EVENT_MOUSE) {
            /* TODO; ev.key, ev.x, ev.y usable */
            /* TB_KEY_MOUSE_{LEFT, RIGHT, MIDDLE, RELEASE, WHEEL_UP, WHEEL_DOWN} */
            if (ev.key == TB_KEY_MOUSE_LEFT) {
                if (ev.y < screen_height - 1) {
                    if ((cursor_line->line_nb + ev.y - y) >= nb_line) {
                        y = y + nb_line - 1 - cursor_line->line_nb;
                        move_cursor_line(nb_line - 1, &cursor_line);
                    } else {
                        move_cursor_line(cursor_line->line_nb + ev.y - y, &cursor_line);
                        y = ev.y;
                    }
                    x = set_x(ev.x, cursor_line->length);
                } else if (ev.y == screen_height - 1) {
                    /* TODO; check if in interface mode, etc */
                }
            } else if (ev.key == TB_KEY_MOUSE_WHEEL_UP) {
                move_screen(-SCROLL_LINE_NUMBER, &x, &y, first_line, last_line,
                    &first_screen_line, &cursor_line);
            } else if (ev.key == TB_KEY_MOUSE_WHEEL_DOWN) {
                move_screen(SCROLL_LINE_NUMBER, &x, &y, first_line, last_line,
                    &first_screen_line, &cursor_line);
            }
        } else if (ev.type == TB_EVENT_RESIZE) {
            if (ev.w < MIN_WIDTH) {
                fprintf(stderr, "Terminal is not wide enough: %d\n", ev.w);
                return ERR_TERM_NOT_BIG_ENOUGH;
            } else if (ev.h < 2) {
                fprintf(stderr, "Terminal is not high enough: %d\n", ev.h);
                return ERR_TERM_NOT_BIG_ENOUGH;
            }
            /* TODO: cursor */
            screen_height = ev.h;
            screen_width = ev.w;
            if (y >= screen_height - 1)
                set_y(screen_height - 2);
        }
    }

    /* CLOSING */
    tb_shutdown();

    return 0;
}



/* MEMORY MANAGEMENT ***********************************************************/

struct line *
new_line(int line_nb, int length, int was_modified)
{
    struct line *res;

    res = (struct line *) malloc(sizeof(struct line));
    res->line_nb = line_nb;
    res->length = length;
    res->was_modified = was_modified;
    res->prev = res->next = NULL;
    res->chars = (char *) malloc(length * sizeof(char));

    return res;
}

void
free_line(struct line *ptr)
{
    free(ptr->chars);
    free(ptr);
}

void
free_everything(struct line **first_line, struct line **last_line,
    struct line **first_screen_line, struct line **cursor_line)
{
    struct line *old_ptr;
    struct line *ptr;

    if (empty == 0)
    {
        ptr = *first_line;
        while (ptr != *last_line) {
            old_ptr = ptr;
            ptr = ptr->next;
            free_line(old_ptr);
        }
        free_line(ptr);
        empty = 1;
    }
    
    *first_line = *last_line = *first_screen_line = *cursor_line = NULL;
}

void
move_cursor_line(int line_nb, struct line **cursor_line)
{
    int mov;

    mov = line_nb - (*cursor_line)->line_nb;
    if (mov > 0) {
        while (mov--)
            *cursor_line = (*cursor_line)->next;
    } else {
        while (mov++)
            *cursor_line = (*cursor_line)->prev;
    }
}


/* FILE MANAGEMENT *************************************************************/

int
load_file(char *src_file_name, int *nb_line,
    struct line **first_line, struct line **last_line,
    struct line **first_screen_line, struct line **cursor_line)
{
    int HAS_BEEN_TRUNCATIONS;
    FILE *src_file = NULL;
    char buf[MAX_CHARS];
    struct line *ptr;
    char *chars;
    int i, j;
    int c;
    int line_nb;
    int reached_EOF;

    // open connection to src_file
    src_file = fopen(src_file_name, "r");
    HAS_BEEN_TRUNCATIONS = reached_EOF = 0;
    empty = 1;
    line_nb = 1;
    *first_line = *last_line = *first_screen_line = *cursor_line = NULL;

    // read content into memory
    while (reached_EOF == 0) {
        i = 0;
        while (1) {
            c = getc(src_file);
            if (c == EOF) {
                reached_EOF = 1;
                break;
            } else if (c == '\n') {
                break;
            } else if (i == MAX_CHARS - 1) {
                // no break, i will remain MAX_CHARS - 1
                // break with end of logical line (EOF or '\n')
                HAS_BEEN_TRUNCATIONS = 1;
            } else {
                buf[i++] = (char) c;
            }
        }
        buf[i++] = '\0';

        // store line
        ptr = new_line(line_nb++, i, 0);
        if (empty) {
            empty = 0;
            *first_line = *last_line = *first_screen_line = *cursor_line = ptr;
            ptr->prev = NULL;
            ptr->next = NULL;
        } else {
            (*last_line)->next = ptr;
            ptr->prev = *last_line;
            ptr->next = NULL;
            *last_line = ptr;
        }
        strcpy(ptr->chars, buf);
    }

    // close connection to src_file
    *nb_line = line_nb;
    fclose(src_file);

    return HAS_BEEN_TRUNCATIONS;
}

int
write_file(char *dest_file_name,
    struct line *first_line, struct line *last_line)
{
    FILE *dest_file = NULL;
    struct line *ptr;
    char *chars;
    int c;

    /* open connection to dest_file */
    dest_file = fopen(dest_file_name, "w");

    /* write content to dest_file */
    ptr = first_line;
    while (1) {
        chars = ptr->chars;
        while (c = *chars++)
            putc(c, dest_file);
        if (ptr == last_line) {
            break;
        } else {
            putc('\n', dest_file);
            ptr = ptr->next;
        }
    }

    /* close connection to dest_file */
    fclose(dest_file);

    return 0;
}


/* GRAPHICAL ******************************************************************/

void
print_line(char *chars, int screen_line)
{
    int i;
    char c;

    for (i = 0; c = *chars++ && i < screen_width; i++)
        tb_set_cell(i, screen_line, (uint32_t) c, 0, 0);
    for (; i < screen_width; i++)
        tb_set_cell(i, screen_line, ' ', 0, 0);
}

void
print_screen(struct line *first_screen_line, struct line *last_line,
    char *buf, int x, int y, int line_nb)
{
    struct line *ptr;
    int sc_line;

    ptr = first_screen_line;

    /* clear screen */
    tb_clear();

    /* print lines */
    for (sc_line = 0; sc_line < screen_height - 1; sc_line++) {
        print_line(ptr->chars, sc_line);
        if (ptr == last_line)
            break;
        ptr = ptr->next;
    }

     /* print interface */
    print_line(buf, screen_height - 1);

    /* print cursor */
    tb_set_cursor(x, y);

    /* print ruler */
    tb_printf(50, screen_height - 1, 0, 0, "%d,%d", line_nb, x);
}


/* UGLY STUFF *****************************************************************/

int
move_screen(int nlines, int *x, int *y,
    struct line *first_line, struct line *last_line,
    struct line **first_screen_line, struct line **cursor_line)
{
    int i;

    if (nlines > 0) {
        for (i = 0; i < nlines; i++) {
            if (*first_screen_line == last_line) {
                break;
            } else {
                *first_screen_line = (*first_screen_line)->next;
            }
        }
        if (*y - nlines >= 0) {
            *y = *y - nlines;
        } else {
            *cursor_line = *first_screen_line;
            *y = 0;
            *x = set_x(*x, (*cursor_line)->length);
        }
    } else if (nlines < 0) {
        for (i = 0; i > nlines; i--) {
            if (*first_screen_line == first_line) {
                break;
            } else {
                *first_screen_line = (*first_screen_line)->prev;
            }
        }
        if (*y - nlines < screen_height - 1) {
            *y = *y - nlines;
        } else {
            *cursor_line = *first_screen_line;
            for (i = 0; i < screen_height - 2; i++)
                *cursor_line = (*cursor_line)->next;
            *y = screen_height - 2;
            *x = set_x(*x, (*cursor_line)->length);
        }
    }

    return 0;
}

int
set_y(int value)
{
    return (value < 0) ? 0 : ((value >= screen_height - 1) ? screen_height - 2 : value);
}

int
set_x(int value, int line_length)
{
    if (value >= line_length) {
        return line_length - 1;
    } else {
        return value;
    }
}
