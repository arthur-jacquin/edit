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
 * add line management
 * add parameters
 * dialog mode: click, arrow, return
 * insert mode: return, enter
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
#include <stdlib.h>
#include "termbox.h"

// CONSTANTS
#define BACKUP_FILE_NAME            "edit_backup_file"
#define MAX_CHARS                   (1 << 10)
#define MIN_WIDTH                   81
#define RULER_WIDTH                 8
#define INTERFACE_WIDTH             (MIN_WIDTH - RULER_WIDTH)
#define MIN_HEIGHT                  2
#define SCROLL_LINE_NUMBER          3

// STATES
#define DEFAULT_MODE                0
#define INSERT_MODE                 1

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
    struct line *prev;                      // pointer to prev line struct
    struct line *next;                      // pointer to next line struct
    char *chars;
};

// MEMORY MANAGEMENT FUNCTIONS
struct line *new_line(int line_nb, int length, int was_modified);
void free_lines(void);

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

// INTERACTION
int dialog(const char *prompt, const char *specific, int writable);

// VARIABLES
char file_name[INTERFACE_WIDTH];            // manipulated file name
int nb_line;                                // number of lines in file
int has_been_changes;                       // store the existence of changes
int screen_height, screen_width;            // terminal dimensions
int x, y;                                   // cursor position in file area
char interface[INTERFACE_WIDTH];            // interface buffer
int prompt_length, interface_x;             // prompt length, cursor position in interface
char message_archive[INTERFACE_WIDTH];      // echoed message archive
struct tb_event ev;                         // struct to retrieve events
struct line *first_line;
struct line *first_line_on_screen;
struct line *cursor_line;                   // only for fast access



// MAIN

int
main(int argc, char *argv[])
{
    int state;
    int a; /* answer to dialog */
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
    state = DEFAULT_MODE;
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
        print_screen(); // TODO: be smarter
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
                if (state == DEFAULT_MODE) {
                    // control key
                    if (c == 'q') { // QUIT
                        // TODO: check for need to save ?
                        if (has_been_changes) {
                            if (a = dialog("Lose changes ? (q: quit, w: write and quit, ESC: cancel)", "qw", 0)) {
                                if (a == 'w')
                                    write_file(file_name);
                                break;
                            }
                        } else {
                            break;
                        }
                    } else if (c == 'w') { // WRITE
                        if (has_been_changes) {
                            write_file(file_name);
                            has_been_changes = 0;
                            echo("File saved.");
                        } else {
                            echo("No changes to write.");
                        }
                    } else if (c == 'W') { // WRITE AS
                        if (dialog("Save as (ESC to cancel): ", "", 1)) {
                            for (i = interface_x; i >= prompt_length; i--)
                                file_name[i-prompt_length] = interface[i];
                            write_file(file_name);
                            has_been_changes = 0;
                            echo("File saved.");
                        }
                    } else if (c == 'r') { // RELOAD
                        if (has_been_changes) {
                            free_lines();
                            load_file(file_name, first_line_on_screen->line_nb, y);
                            has_been_changes = 0;
                            echo("File reloaded.");
                        } else {
                            echo("No changes to revert.");
                        }

                    } else if (c == 'i') { // INSERT MODE
                        state = INSERT_MODE;
                        echo("INSERT (ESC to exit)");

                    } else if (c == 'S') { // CHANGE PARAMETER
                        if (dialog("Change parameter: ", "", 1)) {
                            for (i = interface_x; i >= prompt_length; i--)
                                file_name[i-prompt_length] = interface[i];
                            // TODO: parameter modif, echoes if not succesful

                        }

                    } else if (c == 't') { // TESTING
                        echo(file_name);
                    } else if (c == 'd') {
                        move_screen(1);
                    } else if (c == 'u') {
                        move_screen(-1);
                    }

                } else if (state == INSERT_MODE) {
                    // character insertion
                    insert(c, x, cursor_line);
                    has_been_changes = 1;
                    print_line(cursor_line->chars, y, cursor_line->length);
                    if (x < screen_width - 1) {
                        x++;
                        tb_set_cursor(x, y);
                    }

                }

            } else {
                // much TODO; ev.key, ev.mod usable
                if (ev.key == TB_KEY_ESC) {
                    state = DEFAULT_MODE;
                    echo("");
                } else if (ev.key == TB_KEY_ENTER) {
                    // TODO
                } else if (ev.key == 127) { // RETURN (TODO: change code)
                    // TODO
                } else {
                    // arrows movement
                    if (ev.key == TB_KEY_ARROW_UP) {
                        if (y > 0) {
                            y--;
                            cursor_line = cursor_line->prev;
                        } else if (cursor_line->prev != NULL) {
                            first_line_on_screen = cursor_line = cursor_line->prev;
                        }
                        set_x(x);
                    } else if (ev.key == TB_KEY_ARROW_DOWN) {
                        if (cursor_line->next != NULL) {
                            cursor_line = cursor_line->next;
                            if (y < screen_height - 2) {
                                y++;
                            } else {
                                first_line_on_screen = first_line_on_screen->next;
                            }
                            set_x(x);
                        }
                    } else if (ev.key == TB_KEY_ARROW_LEFT) {
                        set_x(x - 1);
                    } else if (ev.key == TB_KEY_ARROW_RIGHT) {
                        set_x(x + 1);
                    }
                }
            }

        } else if (ev.type == TB_EVENT_MOUSE) {
            if (ev.key == TB_KEY_MOUSE_LEFT) {
                // left mouse click
                if (ev.y < screen_height - 1) {
                    if ((cursor_line->line_nb + ev.y - y) >= nb_line) {
                        move_cursor_line(nb_line - 1);
                        y = y + nb_line - 1 - cursor_line->line_nb;
                    } else {
                        move_cursor_line(cursor_line->line_nb + ev.y - y);
                        y = ev.y;
                    }
                    set_x(ev.x);
                }
            } else if (ev.key == TB_KEY_MOUSE_WHEEL_UP) {
                // scrolling up
                move_screen(-SCROLL_LINE_NUMBER);

            } else if (ev.key == TB_KEY_MOUSE_WHEEL_DOWN) {
                // scrolling down
                move_screen(SCROLL_LINE_NUMBER);
            }

        } else if (ev.type == TB_EVENT_RESIZE) {
            // terminal is resized
            if (resize(ev.w, ev.h)) {
                if (has_been_changes) {
                    write_file(BACKUP_FILE_NAME);
                    fprintf(stderr, "%s as been wrote as backup.\n", BACKUP_FILE_NAME);
                }
                return ERR_TERM_NOT_BIG_ENOUGH;
            } else {
                if (y >= screen_height - 1)
                    move_cursor_line(first_line_on_screen->line_nb + screen_height - 2);
                    y = screen_height - 2;
                set_x(x);
            }
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

    printf("Asking for a line... ");
    res = (struct line *) malloc(sizeof(struct line));
    printf("got %p (line %d, length %d)\n", res, line_nb, length);
    res->line_nb = line_nb;
    res->length = length;
    res->was_modified = was_modified;
    res->prev = res->next = NULL;
    printf("Asking for %d characters... \n", length);
    res->chars = (char *) malloc(length * sizeof(char));

    return res;
}

void
free_lines(void)
{

    struct line *old_ptr;
    struct line *ptr;

    if (first_line != NULL) {
        printf("First line at %p.\n", first_line);
        ptr = first_line;
        while (ptr->next != NULL) {
            old_ptr = ptr;
            ptr = ptr->next;
            free(old_ptr->chars);
            free(old_ptr);
        }
        free(ptr->chars);
        free(ptr);
    }

    /* first_line, cursor_line and first_line_on_screen points to nowhere */
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
        } else {
            last_line->next = ptr;
            ptr->prev = last_line;
            last_line = ptr;
        }
        strcpy(ptr->chars, buf);
        if (line_nb == first_line_on_screen_nb)
            first_line_on_screen = ptr;
        if (line_nb == first_line_on_screen_nb + asked_y)
            cursor_line = ptr;
        line_nb++;
    }

    last_line->next = NULL;

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
dialog(const char *prompt, const char *specific_chars, int writable_dialog)
{ 
    int available_width;
    int i;
    char c;

    prompt_length = strlen(prompt);
    interface_x = prompt_length;
    strcpy(interface, prompt);
    echo(interface);

    while (1) {
        print_screen(); // TODO: be smarter
        tb_present();
        tb_poll_event(&ev);
        if (ev.type == TB_EVENT_KEY) {
            // TODO: manage modifiers
            //  (key XOR ch, one will be zero), mod. Note there is
            //  overlap between TB_MOD_CTRL and TB_KEY_CTRL_*.
            //  TB_MOD_CTRL and TB_MOD_SHIFT are only set as
            //  modifiers to TB_KEY_ARROW_*.
            if (c = ev.ch) {
                if (is_in(specific_chars, c)) {
                    return c;
                } else if (writable_dialog
                    && interface_x < INTERFACE_WIDTH - 1) {
                    // TODO: manage deletes, cursor click, arrow movement
                    interface[interface_x++] = c;
                    interface[interface_x] = '\0';
                    echo(interface);
                }
            } else {
                // much TODO; ev.key, ev.mod usable
                if (ev.key == TB_KEY_ESC) { // cancel
                    echo("");
                    return 0;
                } else if (writable_dialog && ev.key == TB_KEY_ENTER) {
                    return -1;
                } /*else if (ev.key == 127) { // RETURN (TODO: change code)
                    // TODO
                } else {
                    // arrows movement
                    if (ev.key == TB_KEY_ARROW_UP) {
                    } else if (ev.key == TB_KEY_ARROW_DOWN) {
                    } else if (ev.key == TB_KEY_ARROW_LEFT) {
                    } else if (ev.key == TB_KEY_ARROW_RIGHT) {
                    }
                }*/
            }
    
        } /*else if (ev.type == TB_EVENT_MOUSE) {
            if (ev.key == TB_KEY_MOUSE_LEFT
                // left mouse click
                && ev.y = screen_height - 1
                && prompt_length <= ev.x && ev.x < INTERFACE_WIDTH) {
                interface_x = ev.x;
                tb_set_cursor(interface_x, screen_height - 1);
            } else if (ev.key == TB_KEY_MOUSE_WHEEL_UP) {
                // scrolling up
                move_screen(-SCROLL_LINE_NUMBER);
    
            } else if (ev.key == TB_KEY_MOUSE_WHEEL_DOWN) {
                // scrolling down
                move_screen(SCROLL_LINE_NUMBER);
            }
    
        } else if (ev.type == TB_EVENT_RESIZE) {
            // terminal is resized
            if (resize(ev.w, ev.h)) {
                write_file(BACKUP_FILE_NAME);
                fprintf(stderr, "%s as been wrote as backup.\n", BACKUP_FILE_NAME);
            return ERR_TERM_NOT_BIG_ENOUGH;
            } else {
                if (y >= screen_height - 1)
                    move_cursor_line(first_line_on_screen->line_nb + screen_height - 2);
                    y = screen_height - 2;
                set_x(x);
            }
        }*/
    }
}
