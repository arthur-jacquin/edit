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
 * fix bug on reload, get back at position
 * add dialog mode
 * better insert mode
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


// FLAGS
#define TB_IMPL
#define MOUSE_SUPPORT               1

// INCLUDES
#include <stdio.h>
#include "termbox.h"

// CONSTANTS
#define MAX_CHARS                   (1 << 10)
#define MIN_WIDTH                   81
#define RULER_WIDTH                 8
#define INTERFACE_WIDTH             (MIN_WIDTH - RULER_WIDTH)
#define MIN_HEIGHT                  2
#define SCROLL_LINE_NUMBER          3

// ERROR CODES
#define ERR_TERM_NOT_BIG_ENOUGH     1
#define ERR_MISSING_FILE_NAME       2
#define ERR_MALLOC                  3
#define ERR_TOO_LONG_LINE           4
#define ERR_INVALID_LINE_VALUE      5
 
// LINE STRUCT
struct line {
    int line_nb;
    int length;
    int was_modified;
    struct line *prev;                      // ptr to prev line struct
    struct line *next;                      // ptr to next line struct
    char *chars;
};

// MEMORY MANAGEMENT FUNCTIONS
struct line *new_line(int line_nb, int length, int was_modified);
void free_everything(void);

// FILE MANAGEMENT FUNCTIONS
int load_file(char *src_file_name, int first_line_on_screen_nb, int asked_y);
int write_file(char *file_name);

// LINE MANAGEMENT FUNCTIONS
void move_cursor_line(int line_nb);
int move_screen(int nlines);
void insert(char c, int pos, struct line *line);

// GRAPHICAL FUNCTIONS
int resize(int width, int height);
void print_line(const char *chars, int screen_line, int length);
void print_screen(void);
void print_ruler(void);
void echo(const char *str);
void set_x(int value);

// INTERACTION FUNCTIONS
int dialog(const char *prompt, const char *specific_chars, int WRITE_TO_BUF);

// VARIABLES
char file_name[INTERFACE_WIDTH];            // manipulated file name
int nb_line;                                // number of lines in file
int has_been_changes;                       // store the existence of changes
int screen_height, screen_width;            // terminal dimensions
int x, y;                                   // cursor position in file area
char interface[INTERFACE_WIDTH];            // interface buffer
char message_archive[INTERFACE_WIDTH];      // echoed message archive
struct tb_event ev;                         // struct to retrieve events
struct line *first_line;
struct line *first_line_on_screen;
struct line *cursor_line;                   // only for fast access



// MAIN

int
main(int argc, char *argv[])
{
    int i;
    uint32_t c;

    // parse arguments
    if (argc == 1) {
        fprintf(stderr, "Missing file name.");
        return ERR_MISSING_FILE_NAME;
    } else {
        // TODO: manage options
        strcpy(file_name, argv[argc - 1]);
    }

    // load file
    first_line = first_line_on_screen = cursor_line = NULL;
    has_been_changes = 0;
    load_file(file_name, 1, 0);
    printf("File %s loaded.\n", file_name);
    
    // initialise termbox
    x = y = 0;
    tb_init();
    if (MOUSE_SUPPORT)
        tb_set_input_mode(TB_INPUT_ESC | TB_INPUT_MOUSE);
    if (resize(tb_width(), tb_height()))
        return ERR_TERM_NOT_BIG_ENOUGH;
    echo("Welcome to edit!");

    // main loop
    while (1) {
        print_screen();
        tb_present();
        tb_poll_event(&ev);
        if (ev.type == TB_EVENT_KEY) {
            // TODO: manage modifiers
            //  (key XOR ch, one will be zero), mod. Note there is
            //  overlap between TB_MOD_CTRL and TB_KEY_CTRL_*.
            //  TB_MOD_CTRL and TB_MOD_SHIFT are only set as
            //  modifiers to TB_KEY_ARROW_*.
            if (c = ev.ch) {
                // TODO; c = ev.ch, ev.mod usable
                if (c == 'q') {
                    // QUIT
                    // TODO: check for need to save ?
                    break;
                }
                if (c == 'i') { // INSERT MODE
                    echo("INSERT (ESC to exit)");
                    while (1) {
                        tb_present();
                        tb_poll_event(&ev);
                        if (ev.type == TB_EVENT_KEY) {
                            if (c = ev.ch) {
                                insert(c, x, cursor_line);
                                has_been_changes = 1;
                                print_line(cursor_line->chars, y, cursor_line->length);
                                if (x < screen_width - 1) {
                                    x++;
                                    tb_set_cursor(x, y);
                                }
                            } else if (ev.key == 27) { // ESCAPE
                                break;
                            } else if (ev.key == 13) { // ENTER

                            } else if (ev.key == 127) { // RETURN

                            } else { // TODO: HANDLE OTHER THINGS

                            }
                        } else {
                            // TODO: HANDLE OTHER THINGS
                        }

                    }
                }
                if (c == 'd')
                    move_screen(1);
                if (c == 'u')
                    move_screen(-1);
                if (c == 'r') { // RELOAD
                    // TODO: check if get back to right line..., get cursor right
                    printf("To reload...\n");
                    free_everything();
                    printf("Successful free.\n");
                    load_file(file_name, first_line_on_screen->line_nb, y);
                    has_been_changes = 0;
                    echo("File reloaded.");
                }
                if (c == 'w') { // WRITE
                    write_file(file_name);
                    has_been_changes = 0;
                    echo("File saved.");
                }
            } else {
                // TODO; ev.key, ev.mod usable
                switch (ev.key) {
                case TB_KEY_ARROW_UP:
                    if (y > 0) {
                        y--;
                        cursor_line = cursor_line->prev;
                    } else if (cursor_line->prev != NULL) {
                        first_line_on_screen = cursor_line = cursor_line->prev;
                    }
                    set_x(x);
                    break;
                case TB_KEY_ARROW_DOWN:
                    if (y < screen_height - 2) {
                        y++;
                        cursor_line = cursor_line->next;
                    } else if (cursor_line->next != NULL) {
                        first_line_on_screen = first_line_on_screen->next;
                        cursor_line = cursor_line->next;
                    }
                    set_x(x);
                    break;
                case TB_KEY_ARROW_LEFT:
                    set_x(x - 1);
                    break;
                case TB_KEY_ARROW_RIGHT:
                    set_x(x + 1);
                    break;
                }
            }
        } else if (ev.type == TB_EVENT_MOUSE) {
            if (ev.key == TB_KEY_MOUSE_LEFT && ev.y < screen_height - 1) {
                if ((cursor_line->line_nb + ev.y - y) >= nb_line) {
                    y = y + nb_line - 1 - cursor_line->line_nb;
                    move_cursor_line(nb_line - 1);
                } else {
                    move_cursor_line(cursor_line->line_nb + ev.y - y);
                    y = ev.y;
                }
                set_x(ev.x);
            } else if (ev.key == TB_KEY_MOUSE_WHEEL_UP) {
                move_screen(-SCROLL_LINE_NUMBER);
            } else if (ev.key == TB_KEY_MOUSE_WHEEL_DOWN) {
                move_screen(SCROLL_LINE_NUMBER);
            }
        } else if (ev.type == TB_EVENT_RESIZE) {
            if (resize(ev.w, ev.h))
                // TODO: graceful crash if unsaved changes
                return ERR_TERM_NOT_BIG_ENOUGH;
            if (y >= screen_height - 1)
                y = screen_height - 2;
            set_x(x);
        }
    }

    // close
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
free_everything(void)
{
    struct line *old_ptr;
    struct line *ptr;

    printf("Trying to free everything.\n");
    if (first_line != NULL)
    {
        printf("First line at %p.\n", first_line);
        ptr = first_line;
        while (ptr->next != NULL) {
            old_ptr = ptr;
            ptr = ptr->next;
            free(old_ptr->chars);
            free(old_ptr);
        }
        // free(ptr->chars);
        // free(ptr);
        first_line = first_line_on_screen = cursor_line = NULL;
    }
}


/* FILE MANAGEMENT *************************************************************/

int
load_file(char *src_file_name, int first_line_on_screen_nb, int asked_y)
{
    FILE *src_file = NULL;
    char buf[MAX_CHARS];
    struct line *ptr;
    struct line *last_line;
    int i, j;
    int c;
    int line_nb;
    int reached_EOF;

    // open connection to src_file
    src_file = fopen(src_file_name, "r");
    reached_EOF = 0;
    line_nb = 1;
    first_line = first_line_on_screen = cursor_line = NULL;

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
                return ERR_TOO_LONG_LINE;
            } else {
                buf[i++] = (char) c;
            }
        }
        buf[i++] = '\0';

        // store line
        ptr = new_line(line_nb, i, 0);
        if (first_line == NULL) {
            first_line = last_line = ptr;
            ptr->prev = NULL;
            ptr->next = NULL;
        } else {
            last_line->next = ptr;
            ptr->prev = last_line;
            ptr->next = NULL;
            last_line = ptr;
        }
        strcpy(ptr->chars, buf);
        if (line_nb == first_line_on_screen_nb)
            first_line_on_screen = ptr;
        if (line_nb == first_line_on_screen_nb + asked_y)
            cursor_line = ptr;
        line_nb++;
    }

    // fall back for pointers
    if (cursor_line == NULL) {
        first_line_on_screen = cursor_line = ptr;
        y = 0; // TODO: check if coherent
    }
    set_x(x);

    // close connection to src_file
    nb_line = line_nb;
    fclose(src_file);

    return 0;
}

int
write_file(char *dest_file_name)
{
    FILE *dest_file = NULL;
    struct line *ptr;
    char *chars;
    int c;

    // open connection to dest_file
    dest_file = fopen(dest_file_name, "w");

    // write content to dest_file
    if (first_line != NULL) {
        ptr = first_line;
        while (1) {
            chars = ptr->chars;
            while (c = *chars++)
                putc(c, dest_file);
            if (ptr->next == NULL) {
                break;
            } else {
                putc('\n', dest_file);
                ptr = ptr->next;
            }
        }
    }

    // close connection to dest_file
    fclose(dest_file);

    return 0;
}


/* LINE MANAGEMENT *************************************************************/

void
move_cursor_line(int line_nb)
{
    int mov;

    mov = line_nb - cursor_line->line_nb;
    if (mov > 0) {
        while (mov--)
            cursor_line = cursor_line->next;
    } else {
        while (mov++)
            cursor_line = cursor_line->prev;
    }
}

int
move_screen(int nlines)
{
    int i;

    if (nlines > 0) {
        for (i = 0; i < nlines; i++) {
            if (first_line_on_screen->next == NULL) {
                break;
            } else {
                first_line_on_screen = first_line_on_screen->next;
            }
        }
        if (y - i >= 0) {
            y = y - i;
        } else {
            cursor_line = first_line_on_screen;
            y = 0;
            set_x(x);
        }
    } else if (nlines < 0) {
        for (i = 0; i > nlines; i--) {
            if (first_line_on_screen->prev == NULL) {
                break;
            } else {
                first_line_on_screen = first_line_on_screen->prev;
            }
        }
        if (y - i < screen_height - 1) {
            y = y - i;
        } else {
            cursor_line = first_line_on_screen;
            for (i = 0; i < screen_height - 2; i++)
                cursor_line = cursor_line->next;
            y = screen_height - 2;
            set_x(x);
        }
    }

    return 0;
}

void
insert(char c, int pos, struct line *line)
{
    int i;
    char *old_chars_start;
    char *old_chars;
    char *chars;

    old_chars_start = old_chars = line->chars;
    line->chars = chars = (char *) malloc((line->length + 1) * sizeof(char));

    for (i = 0; i < pos; i++)
        *chars++ = *old_chars++;
    *chars++ = c;
    strcpy(chars, old_chars);

    line->length++;
    line->was_modified = 1;

    free(old_chars_start);
}


/* GRAPHICAL ******************************************************************/

int
resize(int width, int height)
{
    if ((screen_width = width) < MIN_WIDTH) {
        fprintf(stderr, "Terminal is not wide enough: %d\n", screen_width);
        return ERR_TERM_NOT_BIG_ENOUGH;
    } else if ((screen_height = height) < MIN_HEIGHT) {
        fprintf(stderr, "Terminal is not high enough: %d\n", screen_height);
        return ERR_TERM_NOT_BIG_ENOUGH;
    }

    return 0;
}

void
print_line(const char *chars, int screen_line, int length)
{
    int i;

    tb_printf(0, screen_line, 0, 0, chars);
    for (i = length - 1; i < screen_width; i++)
        tb_set_cell(i, screen_line, ' ', 0, 0);
}

void
print_screen(void)
{
    // write everything but interface
    
    struct line *ptr;
    int sc_line;

    tb_clear();

    ptr = first_line_on_screen;
    for (sc_line = 0; sc_line < screen_height - 1; sc_line++) {
        print_line(ptr->chars, sc_line, ptr->length);
        if (ptr->next == NULL)
            break;
        ptr = ptr->next;
    }

    tb_set_cursor(x, y);
    print_ruler();
    echo(message_archive);
}

void
print_ruler(void)
{
    int i;

    for (i = 0; i < RULER_WIDTH; i++)
        tb_set_cell(screen_width - RULER_WIDTH + i, screen_height - 1, ' ', 0, 0);
    tb_printf(screen_width - RULER_WIDTH, screen_height - 1, 0, 0,
        "%d,%d", cursor_line->line_nb, x);
}

void
echo(const char *str)
{
    int i;

    strcpy(message_archive, str);
    tb_print(0, screen_height - 1, 0, 0, str);
    for (i = strlen(str); i < screen_width - RULER_WIDTH; i++)
        tb_set_cell(i, screen_height - 1, ' ', 0, 0);
}

void
set_x(int value)
{
    x = (value >= cursor_line->length) ?
        (cursor_line->length - 1) :
        ((value >= 0) ? value : 0);
}


/* INTERACTION ****************************************************************/

int
is_in(const char *chars, char x)
{
    char c;

    while (c = *chars++)
        if (c == x)
            return 1;

    return 0;
}

int
dialog(const char *prompt, const char *specific_chars, int WRITE_TO_BUF)
{
    struct tb_event ev;
    int prompt_length;
    int available_width;
    int i;
    char c;

    prompt_length = strlen(prompt);
    available_width = INTERFACE_WIDTH - prompt_length;

    // TODO: echo prompt
    print_line(prompt, screen_height - 1, prompt_length);

    i = 0;
    while (1) {
        tb_present();
        tb_poll_event(&ev);
        if (ev.type == TB_EVENT_KEY) {
            if (c = ev.ch) {
                if (is_in(specific_chars, c)) {
                    return c;
                } else if (WRITE_TO_BUF && i < available_width) {
                    tb_set_cell(prompt_length + i, screen_height - 1, c, 0, 0);
                    interface[i++] = c;
                }
            } else if (ev.key == 27) { // ESCAPE
                return 0;
            } else if (ev.key == 13) { // ENTER
                return -1;
            } else if (ev.key == 127) { // RETURN
            }
        } else {
            // TODO: manage others
        }
    }
}
