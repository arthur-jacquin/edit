// INCLUDES
#include <stdio.h>
#include <stdlib.h>
#define TB_IMPL
#include "termbox.h"
#include "config.h"

// CONSTANTS
#define VERSION                     "alpha"
#define MAX_CHARS                   (1 << 8)
#define INTERFACE_WIDTH             (MIN_WIDTH - RULER_WIDTH)
#define MIN_HEIGHT                  2



// STRUCTS *********************************************************************

struct line {
    int line_nb;
    int length;
    struct line *prev;
    struct line *next;
    char *chars;
};

struct pos {
    int l;
    int x;
};

struct selection {
    int l, x;                           // line and column
    int n;                              // number of characters
    int temp;
    struct selection *next;
};

struct substring {
    int st;                             // starting position in original string
    int n;                              // number of characters
};



// FUNCTIONS *******************************************************************

// utils
int is_blank(char c);
int is_word_char(char c);
int is_number(char c);
int is_in(const char *list, const char *chars, int x, int length);

// lines management
struct line *new_line(int line_nb, int length);
struct line *get_line(int delta_from_first_line_on_screen);
void free_lines(struct line *starting);
void link_lines(struct line *l1, struct line *l2);
void shift_line_nb(int starting_line_nb, int ending_line_nb, int shift);
int insert_line(int asked_line_nb, const char *chars);
int move_line(int delta);
void copy_to_clip(int starting_line_nb, int nb);
void move_to_clip(int starting_line_nb, int nb);
void insert_clip(struct line *starting_line, int below);

// file management
int load_file(char *file_name, int first_line_on_screen_nb);
int write_file(char *file_name);

// moving
struct pos pos_of(int l, int x);
struct pos find_first_non_blanck(void);
struct pos find_matching_bracket(void);
struct pos find_next_selection(int delta);
int find_start_of_block(int starting_line_nb, int nb);
int find_end_of_block(int starting_line_nb, int nb);
void go_to(struct pos p);

// selections
int is_inf(struct selection *s1, struct selection *s2);
struct selection *sel_of_pos(struct pos p, int temp);
struct pos pos_of_curs(void);
int closest_after_nb(void);
struct pos get_pos_sel(int nb);
int nb_sels(void);
void empty_sels(void);
void merge_sels(struct selection *starting);
void add_running_sels(int temp);
void add_range_sels(int start, int end, int temp);
void shift_sels(struct pos starting, struct pos ending, struct pos delta);
void delete_temp_sels(void);
void search(void);

// actions on selections
void act(void (*process)(struct line *, struct selection *), int line_op);
void insert(struct line *l, struct selection *s);
void split_lines(struct line *l, struct selection *s);
void suppress(struct line *l, struct selection *s);
void concatenate_lines(struct line *l, struct selection *s);
void indent(struct line *l, struct selection *s);
void comment(struct line *l, struct selection *s);
void lower(struct line *l, struct selection *s);
void upper(struct line *l, struct selection *s);
void replace(struct line *l, struct selection *s);

// graphical
int resize(int width, int height);
void echo(const char *str);
struct selection *print_line(const char *chars, int length, int line_nb,
    struct selection *s, int screen_line);
void print_dialog(void);
void print_ruler(void);
void print_all(void);

// interaction
int dialog(const char *prompt, const char *specifics, int writable);
void display_help(void);



// VARIABLES *******************************************************************

struct {
    int autoindent;
    int syntax_highlight;
    int highlight_selections;
    int case_sensitive;
    int replace_tabs;
    char field_separator;
    int tab_width;
    char language[5];
} settings;

int anchored;
int in_insert_mode;
int has_been_changes;
int read_only;

int m;                              // multiplier
int asked_indent;
char dialog_chars[INTERFACE_WIDTH]; // dialog interface buffer
int prompt_length, dialog_x;        // prompt length, cursor column in interface

char file_name[INTERFACE_WIDTH];    // name of file to write

struct selection *sel;              // selections queue

char spattern[INTERFACE_WIDTH];     // search pattern
char pspattern[INTERFACE_WIDTH];    // previous search pattern
char rpattern[INTERFACE_WIDTH];     // replace pattern
char prpattern[INTERFACE_WIDTH];    // previous replace pattern
struct substring fields[9];
struct substring subpatterns[9];

int nb_line;                        // number of lines in file
struct line *first_line;
struct line *first_line_on_screen;
struct {
    struct line *start;
    int nb_line;
} clipboard;

struct pos anchor;                  // anchor line and column
int y, x;                           // cursor position in file area
int screen_height, screen_width;    // terminal dimensions
struct tb_event ev;                 // struct to retrieve events

struct lang *syntax = &languages[0]; // TODO manage language dynamically



// MAIN ************************************************************************

int
main(int argc, char *argv[])
{
    int a, l1, i, old_line_nb;
    uint32_t c;
    struct pos p;


    // PARSING ARGUMENTS *******************************************************

    if (argc == 1) {
        printf("Refer to https://jacquin.xyz/edit for complete help.\n");
        return 0;
    } else if (argc == 2) {
        if (!(strcmp(argv[1], "--version") && strcmp(argv[1], "-v"))) {
            printf("%s\n", VERSION);
            return 0;
        } else if (!(strcmp(argv[1], "--help") && strcmp(argv[1], "-h"))) {
            printf("Refer to https://jacquin.xyz/edit for complete help.\n");
            return 0;
        } else {
            strcpy(file_name, argv[1]);
            read_only = 0;
        }
    } else if (argc == 3) {
        if (!(strcmp(argv[2], "--read-only") && strcmp(argv[1], "-r"))) {
            strcpy(file_name, argv[1]);
            read_only = 1;
        }
    } else {
        return ERR_BAD_ARGUMENTS;
    }


    // INIT VARIABLES **********************************************************

    // default settings
    settings.autoindent             = AUTOINDENT;
    settings.syntax_highlight       = SYNTAX_HIGHLIGHT;
    settings.highlight_selections   = HIGHLIGHT_SELECTIONS;
    settings.case_sensitive         = CASE_SENSITIVE;
    settings.replace_tabs           = REPLACE_TABS;
    settings.field_separator        = FIELD_SEPARATOR;
    settings.tab_width              = TAB_WIDTH;
    strcpy(settings.language, "none"); // TODO: detect language

    // editor variables
    m = in_insert_mode = anchored = 0;
    strcpy(spattern, "");
    strcpy(pspattern, "");
    strcpy(rpattern, "");
    strcpy(prpattern, "");
    sel = NULL;
    clipboard.start = NULL;

    // initialise termbox
    x = y = 0;
    tb_init();
    tb_set_output_mode(OUTPUT_MODE);
    if (MOUSE_SUPPORT)
        tb_set_input_mode(TB_INPUT_ESC | TB_INPUT_MOUSE);
    if (resize(tb_width(), tb_height()))
        return ERR_TERM_NOT_BIG_ENOUGH;
    echo("Welcome to edit!");

    // load file
    load_file(file_name, 1);


    // MAIN LOOP ***************************************************************
    
    while (1) {
        delete_temp_sels();
        add_running_sels(1);
        print_all();
        tb_present();
        tb_poll_event(&ev);
        switch (ev.type) {
        case TB_EVENT_KEY:
            if (ev.ch && in_insert_mode) {
                act(insert, 0);
                has_been_changes = 1;
                go_to(pos_of(first_line_on_screen->line_nb + y, x + 1));
            } else if (ev.ch && !in_insert_mode) {
                if ((m && ev.ch == '0') || ('1' <= ev.ch && ev.ch <= '9')) {
                    m = 10*m + ev.ch - '0';
                    sprintf(dialog_chars, "Multiplier: %d", m);
                    break;
                } else {
                if (m == 0)
                    m = 1;
                switch (ev.ch) {
                case KB_HELP:
                    display_help();
                    break;
                case KB_QUIT:
                    /*TODO if (has_been_changes) {
                        if (a = dialog("Lose changes ? (q: quit, w: write and quit, ESC: cancel)", "qw", 0)) {
                            if (a == 'w')
                                write_file(file_name);
                            tb_shutdown();
                            return 0;
                        }
                    } else {*/
                        tb_shutdown();
                        return 0;
                    /*}*/
                    break;
                case KB_WRITE:
                    if (has_been_changes) {
                        write_file(file_name);
                        has_been_changes = 0;
                        echo("File saved.");
                    } else {
                        echo("No changes to write.");
                    }
                    break;
                case KB_WRITE_AS:
                    /*TODO if (dialog("Save as (ESC to cancel): ", "", 1)) {
                        for (i = interface_x; i >= prompt_length; i--)
                            file_name[i-prompt_length] = interface[i];
                        write_file(file_name);
                        has_been_changes = 0;
                        echo("File saved.");
                    }*/
                    break;
                case KB_RELOAD:
                    if (has_been_changes) {
                        old_line_nb = first_line_on_screen->line_nb + y;
                        load_file(file_name, first_line_on_screen->line_nb);
                        go_to(pos_of(old_line_nb, x));
                        has_been_changes = 0;
                        echo("File reloaded.");
                    } else {
                        echo("No changes to revert.");
                    }
                    break;
                case KB_INSERT_MODE:
                    in_insert_mode = 1;
                    echo("INSERT (ESC to exit)");
                    break;
                case KB_CHANGE_SETTING:
                    /* TODO if (dialog("Change parameter: ", "", 1)) {
                        for (i = interface_x; i >= prompt_length; i--)
                            file_name[i-prompt_length] = interface[i];
                        // TODO: parameter modif, echoes if not succesful
                    }*/
                    break;
                case KB_INSERT_START_LINE:
                    empty_sels();
                    go_to(pos_of(first_line_on_screen->line_nb + y, 0));
                    in_insert_mode = 1;
                    echo("INSERT (ESC to exit)");
                    break;
                case KB_INSERT_END_LINE:
                    empty_sels();
                    go_to(pos_of(first_line_on_screen->line_nb + y,
                        get_line(y)->length - 1));
                    in_insert_mode = 1;
                    echo("INSERT (ESC to exit)");
                    break;
                case KB_INSERT_LINE_BELOW:
                    empty_sels();
                    go_to(pos_of(insert_line(
                        first_line_on_screen->line_nb + y + 1, ""), 0));
                    in_insert_mode = 1;
                    has_been_changes = 1;
                    echo("INSERT (ESC to exit)");
                    break;
                case KB_INSERT_LINE_ABOVE:
                    empty_sels();
                    go_to(pos_of(insert_line(
                        first_line_on_screen->line_nb + y, ""), 0));
                    in_insert_mode = 1;
                    has_been_changes = 1;
                    echo("INSERT (ESC to exit)");
                    break;
                case KB_CLIP_YANK_LINE:
                    copy_to_clip(first_line_on_screen->line_nb + y, m);
                    break;
                case KB_CLIP_YANK_BLOCK:
                    l1 = find_start_of_block(first_line_on_screen->line_nb + y, 1);
                    copy_to_clip(l1, find_end_of_block(l1, m) - l1 + 1);
                    break;
                case KB_CLIP_DELETE_LINE:
                    move_to_clip(first_line_on_screen->line_nb + y, m);
                    go_to(pos_of(first_line_on_screen->line_nb + y, x));
                    has_been_changes = 1;
                    break;
                case KB_CLIP_DELETE_BLOCK:
                    l1 = find_start_of_block(first_line_on_screen->line_nb + y, 1);
                    move_to_clip(l1, find_end_of_block(l1, m) - l1 + 1);
                    go_to(pos_of(l1, x));
                    has_been_changes = 1;
                    break;
                case KB_CLIP_PASTE_AFTER:
                    while (m--)
                        insert_clip(get_line(y), 1);
                    has_been_changes = 1;
                    break;
                case KB_CLIP_PASTE_BEFORE:
                    while (m--)
                        insert_clip(get_line(y), 0);
                    has_been_changes = 1;
                    break;
                case KB_MOVE_MATCHING:
                    go_to(find_matching_bracket());
                    break;
                case KB_MOVE_START_LINE:
                    go_to(pos_of(first_line_on_screen->line_nb + y, 0));
                    break;
                case KB_MOVE_NON_BLANCK:
                    go_to(find_first_non_blanck());
                    break;
                case KB_MOVE_END_LINE:
                    go_to(pos_of(first_line_on_screen->line_nb + y,
                        get_line(y)->length - 1));
                    break;
                case KB_MOVE_SPECIFIC_LINE:
                    go_to(pos_of(m, x));
                    break;
                case KB_MOVE_END_FILE:
                    go_to(pos_of(nb_line, x));
                    break;
                case KB_MOVE_NEXT_CHAR:
                    go_to(pos_of(first_line_on_screen->line_nb + y, x + m));
                    break;
                case KB_MOVE_PREV_CHAR:
                    go_to(pos_of(first_line_on_screen->line_nb + y, x - m));
                    break;
                case KB_MOVE_NEXT_LINE:
                    go_to(pos_of(first_line_on_screen->line_nb + y + m, x));
                    break;
                case KB_MOVE_PREV_LINE:
                    go_to(pos_of(first_line_on_screen->line_nb + y - m, x));
                    break;
                case KB_MOVE_NEXT_WORD:
                    // TODO
                    break;
                case KB_MOVE_PREV_WORD:
                    // TODO
                    break;
                case KB_MOVE_NEXT_BLOCK:
                    go_to(pos_of(find_end_of_block(
                        first_line_on_screen->line_nb + y, m), x));
                    break;
                case KB_MOVE_PREV_BLOCK:
                    go_to(pos_of(find_start_of_block(
                        first_line_on_screen->line_nb + y, m), x));
                    break;
                case KB_MOVE_NEXT_SEL:
                    // if ((p = find_next_selection(m)).l) {
                    //     go_to(p);
                    // } else {
                    //     echo("No more selections downwards.");
                    // }
                    break;
                case KB_MOVE_PREV_SEL:
                    // if ((p = find_next_selection(-m)).l) {
                    //     go_to(p);
                    // } else {
                    //     echo("No more selections upwards.");
                    // }
                    break;
                case KB_SEL_DISPLAY_COUNT:
                    sprintf(dialog_chars, "%d selections.", nb_sels());
                    break;
                case KB_SEL_CURSOR_LINE:
                    add_range_sels(first_line_on_screen->line_nb + y,
                        first_line_on_screen->line_nb + y, 0);
                    break;
                case KB_SEL_CUSTOM_RANGE:
                    // TODO
                    break;
                case KB_SEL_ALL_LINES:
                    add_range_sels(1, nb_line, 0);
                    break;
                case KB_SEL_LINES_BLOCK:
                    l1 = find_start_of_block(first_line_on_screen->line_nb + y, 1);
                    add_range_sels(l1, find_end_of_block(l1, m),0);
                    break;
                case KB_SEL_FIND:
                case KB_SEL_SEARCH:
                    // TODO
                    break;
                case KB_SEL_ANCHOR:
                    if (anchored) {
                        anchored = 0;
                    } else {
                        anchor = pos_of_curs();
                        anchored = 1;
                    }
                    break;
                case KB_SEL_APPEND:
                    add_running_sels(0);
                    anchored = 0;
                    break;
                case KB_SEL_COLUMN:
                    // TODO
                    break;
                case KB_ACT_INCREASE_INDENT:
                    asked_indent = m * settings.tab_width;
                    act(indent, 1);
                    break;
                case KB_ACT_DECREASE_INDENT:
                    asked_indent = - m * settings.tab_width;
                    act(indent, 1);
                    break;
                case KB_ACT_COMMENT:
                    act(comment, 1);
                    break;
                case KB_ACT_SUPPRESS:
                    // TODO
                    break;
                case KB_ACT_REPLACE:
                    // TODO
                    break;
                case KB_ACT_LOWERCASE:
                    act(lower, 0);
                    break;
                case KB_ACT_UPPERCASE:
                    act(upper, 0);
                    break;
                }
                m = 0;
                }
            } else if (ev.key) {
                switch (ev.key) {
                case TB_KEY_ARROW_RIGHT:
                    m = (m == 0) ? 1 : m;
                    go_to(pos_of(first_line_on_screen->line_nb + y, x + m));
                    m = 0;
                    break;
                case TB_KEY_ARROW_LEFT:
                    m = (m == 0) ? 1 : m;
                    go_to(pos_of(first_line_on_screen->line_nb + y, x - m));
                    m = 0;
                    break;
                case TB_KEY_ARROW_DOWN:
                    m = (m == 0) ? 1 : m;
                    if (ev.mod == TB_MOD_SHIFT) {
                        go_to(pos_of(move_line(m), x));
                        has_been_changes = 1;
                    } else {
                        go_to(pos_of(first_line_on_screen->line_nb + y + m, x));
                    }
                    m = 0;
                    break;
                case TB_KEY_ARROW_UP:
                    m = (m == 0) ? 1 : m;
                    if (ev.mod == TB_MOD_SHIFT) {
                        go_to(pos_of(move_line(-m), x));
                        has_been_changes = 1;
                    } else {
                        go_to(pos_of(first_line_on_screen->line_nb + y - m, x));
                    }
                    m = 0;
                    break;
                case TB_KEY_ESC:
                    if (in_insert_mode) {
                        in_insert_mode = 0;
                    } else {
                        empty_sels();
                        anchored = 0;
                        m = 0;
                    }
                    echo("");
                    break;
                case TB_KEY_ENTER:
                    if (in_insert_mode) {
                        // TODO
                        // break_line(x, cursor_line);
                        // has_been_changes = 1;
                    } else {
                        go_to(pos_of(first_line_on_screen->line_nb + y + 1, x));
                    }
                    break;
                case TB_KEY_BACKSPACE:
                    if (in_insert_mode) {
                        // TODO
                    } else {
                        // TODO
                    }
                    break;
                // TODO: tab, ², SUPPR
                }
            }
            break;

        case TB_EVENT_MOUSE:
            switch (ev.key) {
                case TB_KEY_MOUSE_LEFT:
                    if (ev.y < screen_height - 1)
                        go_to(pos_of(first_line_on_screen->line_nb + ev.y, ev.x));
                    break;
                case TB_KEY_MOUSE_WHEEL_UP:
                    old_line_nb = first_line_on_screen->line_nb + y;
                    first_line_on_screen = get_line(-SCROLL_LINE_NUMBER);
                    if (old_line_nb - first_line_on_screen->line_nb > screen_height - 2) {
                        go_to(pos_of(first_line_on_screen->line_nb + screen_height - 2, x));
                    } else {
                        go_to(pos_of(old_line_nb, x));
                    }
                    break;
                case TB_KEY_MOUSE_WHEEL_DOWN:
                    old_line_nb = first_line_on_screen->line_nb + y;
                    first_line_on_screen = get_line(SCROLL_LINE_NUMBER);
                    if (old_line_nb < first_line_on_screen->line_nb) {
                        go_to(pos_of(first_line_on_screen->line_nb, x));
                    } else {
                        go_to(pos_of(old_line_nb, x));
                    }
                    break;
            }
            break;

        case TB_EVENT_RESIZE:
            if (resize(ev.w, ev.h)) {
                if (has_been_changes)
                    write_file(BACKUP_FILE_NAME);
                return ERR_TERM_NOT_BIG_ENOUGH;
            } else {
                go_to(pos_of(first_line_on_screen->line_nb + y, x));
            }
            break;
        }
    }
}


// UTILS ***********************************************************************

int
is_blank(char c)
{
    return (c == ' ');
}

int
is_word_char(char c)
{
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || (c == '_');
}

int
is_number(char c)
{
    return ('0' <= c && c <= '9');
}

int
is_in(const char *list, const char *chars, int x, int length)
{
    int i, j, ok;
    char c;

    i = j = 0;
    ok = 1;
    while (c = list[i++]) {
        if (c == ' ') {
            if (ok && j == length) {
                return 1;
            } else {
                j = 0;
                ok = 1;
            }
        } else if (c != chars[x+(j++)]) {
            ok = 0;
        }
    }

    return 0;
}


// LINES MANAGEMENT ************************************************************

struct line *
new_line(int line_nb, int length)
{
    struct line *res;

    res = (struct line *) malloc(sizeof(struct line));
    res->line_nb = line_nb;
    res->length = length;
    res->prev = res->next = NULL;
    res->chars = (char *) malloc(length);

    return res;
}

struct line *
get_line(int delta_from_first_line_on_screen)
{
    struct line *ptr;
    int n;

    n = delta_from_first_line_on_screen;
    ptr = first_line_on_screen;
    if (n > 0) {
        while (n--) {
            if (ptr->next == NULL)
                break;
            ptr = ptr->next;
        }
    } else {
        while (n++) {
            if (ptr->prev == NULL)
                break;
            ptr = ptr->prev;
        }
    }

    return ptr;
}

void
free_lines(struct line *starting)
{
    struct line *ptr;
    struct line *old_ptr;

    if (starting != NULL) {
        ptr = starting;
        while (ptr->next != NULL) {
            old_ptr = ptr;
            ptr = ptr->next;
            free(old_ptr->chars);
            free(old_ptr);
        }
        free(ptr->chars);
        free(ptr);
    }
}

void
link_lines(struct line *l1, struct line *l2)
{
    l1->next = l2;
    l2->prev = l1;
}

void
shift_line_nb(int starting_line_nb, int ending_line_nb, int shift)
{
    struct line *ptr;

    if (first_line != NULL) {
        ptr = first_line;
        while (ptr->next != NULL) {
            if (ptr->line_nb >= starting_line_nb && ptr->line_nb <= ending_line_nb)
                ptr->line_nb += shift;
            ptr = ptr->next;
        }
        if (ptr->line_nb >= starting_line_nb && ptr->line_nb <= ending_line_nb)
            ptr->line_nb += shift;
    }
}

int
insert_line(int asked_line_nb, const char *chars)
{
    struct line *replaced_line;
    struct line *new;

    if (asked_line_nb > nb_line + 1)
        asked_line_nb = nb_line + 1;

    if (asked_line_nb <= nb_line) {
        replaced_line = get_line(asked_line_nb - first_line_on_screen->line_nb);
        shift_line_nb(asked_line_nb, nb_line, 1);
        new = new_line(asked_line_nb, strlen(chars));
        strcpy(new->chars, chars);
        if (replaced_line->prev != NULL) {
            link_lines(replaced_line->prev, new);
        } else {
            new->prev = NULL;
        }
        link_lines(new, replaced_line);
    } else {
        new = new_line(asked_line_nb, strlen(chars));
        strcpy(new->chars, chars);
        new->prev = get_line(nb_line - first_line_on_screen->line_nb);
        new->next = NULL;
    }

    nb_line++;
    return asked_line_nb;
}

int
move_line(int delta)
{
    int new_line_nb;
    struct line *src;
    struct line *dest;

    new_line_nb = first_line_on_screen->line_nb + y + delta;
    if (new_line_nb < 1)
        new_line_nb = 1;
    if (new_line_nb > nb_line)
        new_line_nb = nb_line;
    if (new_line_nb == first_line_on_screen->line_nb + y)
        return new_line_nb;

    src = get_line(y);
    dest = get_line(new_line_nb - first_line_on_screen->line_nb);

    if (delta > 0) {
        shift_line_nb(first_line_on_screen->line_nb + y + 1, new_line_nb, -1);
        if (src == first_line_on_screen)
            first_line_on_screen = first_line_on_screen->next;
        if (src->prev != NULL) {
            link_lines(src->prev, src->next);
        } else {
            (src->next)->prev = NULL;
            first_line = src->next;
        }
        if (dest->next != NULL) {
            link_lines(src, dest->next);
        } else {
            src->next = NULL;
        }
        link_lines(dest, src);
    } else {
        shift_line_nb(new_line_nb, first_line_on_screen->line_nb + y - 1, 1);
        if (src->next != NULL) {
            link_lines(src->prev, src->next);
        } else {
            (src->prev)->next = NULL;
        }
        if (dest->prev != NULL) {
            link_lines(dest->prev, src);
        } else {
            src->prev = NULL;
        }
        link_lines(src, dest);
    }
    src->line_nb = new_line_nb;
    
    return new_line_nb;
}

void
copy_to_clip(int starting_line_nb, int nb)
{
    int i;
    struct line *ptr;
    struct line *cp_ptr;
    struct line *old_cp_ptr;

    free_lines(clipboard.start);
    clipboard.start = NULL;

    if (starting_line_nb + nb - 1 > nb_line)
        nb = nb_line + 1 - starting_line_nb;

    ptr = get_line(starting_line_nb - first_line_on_screen->line_nb);
    for (i = 0; i < nb; i++) {
        cp_ptr = new_line(0, ptr->length);
        strcpy(cp_ptr->chars, ptr->chars);
        if (i == 0) {
            cp_ptr->prev = NULL;
            clipboard.start = cp_ptr;
        } else {
            link_lines(old_cp_ptr, cp_ptr);
        }
        old_cp_ptr = cp_ptr;
        if (ptr->next != NULL)
            ptr = ptr->next;
    }
    cp_ptr->next = NULL;
    clipboard.nb_line = nb;
}

void
move_to_clip(int starting_line_nb, int nb)
{
    struct line *starting;
    struct line *ending;

    free_lines(clipboard.start);
    clipboard.start = NULL;

    if (starting_line_nb + nb - 1 > nb_line)
        nb = nb_line + 1 - starting_line_nb;
    nb_line -= nb;

    starting = get_line(starting_line_nb - first_line_on_screen->line_nb);
    ending = get_line(starting_line_nb + nb-1 - first_line_on_screen->line_nb);

    if (ending->next == NULL) {
        if (starting->prev == NULL) {
            first_line = first_line_on_screen = new_line(1, 1);
            first_line->chars;
            first_line->prev = first_line->next = NULL;
            strcpy(first_line->chars, "");
            nb_line = 1;
        } else {
            (starting->prev)->next = NULL;
        }
    } else {
        if (starting->prev == NULL) {
            first_line = first_line_on_screen = ending->next;
            (ending->next)->prev = NULL;
        } else {
            link_lines(starting->prev, ending->next);
        }
        shift_line_nb(ending->line_nb + 1, nb_line + nb, -nb);
    }

    starting->prev = ending->next = NULL;
    clipboard.start = starting;
    clipboard.nb_line = nb;
}

void
insert_clip(struct line *starting_line, int below)
{
    int i, old_starting_line_nb;
    struct line *cp_ptr;
    struct line *ptr;
    struct line *old_ptr;
    struct line *next_starting_line;

    if (clipboard.start != NULL) {
        if (below) {
            next_starting_line = starting_line->next;
        } else {
            next_starting_line = starting_line->prev;
        }

        old_starting_line_nb = starting_line->line_nb;
        shift_line_nb(old_starting_line_nb + below, nb_line, clipboard.nb_line);
        nb_line += clipboard.nb_line;
        
        cp_ptr = clipboard.start;
        for (i = 0; i < clipboard.nb_line; i++) {
            ptr = new_line(old_starting_line_nb + i + below, cp_ptr->length);
            strcpy(ptr->chars, cp_ptr->chars);
            if (i == 0) {
                if (below) {
                    link_lines(starting_line, ptr);
                } else if (next_starting_line != NULL) {
                    link_lines(next_starting_line, ptr);
                } else {
                    ptr->prev = NULL;
                }
            } else {
                link_lines(old_ptr, ptr);
            }
            old_ptr = ptr;
            if (cp_ptr->next != NULL)
                cp_ptr = cp_ptr->next;
        }
        
        if (below) {
            if (next_starting_line != NULL) {
                link_lines(ptr, next_starting_line);
            } else {
                ptr->next = NULL;
            }
        } else {
            link_lines(ptr, starting_line);
        }
    }
}


// FILE MANAGEMENT *************************************************************

int
load_file(char *file_name, int first_line_on_screen_nb)
{
    FILE *src_file = NULL;
    int buf_size = DEFAULT_BUF_SIZE;
    char *buf = NULL;
    char *new_buf = NULL;
    struct line *ptr;
    struct line *last_line;
    int i;
    int c;
    int line_nb;
    int reached_EOF;

    // liberate lines
    free_lines(first_line);
    first_line = first_line_on_screen = NULL;

    // open connection to src_file
    src_file = fopen(file_name, "r");
    reached_EOF = 0;
    line_nb = 1;

    // prepare buffer
    buf = (char *) malloc(buf_size);

    // read content into memory
    while (!reached_EOF) {
        i = 0;
        while (1) {
            if (i == buf_size - 1) {
                buf_size <<= 1;
                new_buf = (char *) malloc(buf_size);
                strcpy(new_buf, buf);
                free(buf);
                buf = new_buf;
            }
            if ((c = getc(src_file)) == EOF) {
                reached_EOF = 1;
                break;
            } else if (c == '\n') {
                break;
            } else {
                buf[i++] = (char) c;
            }
        }
        buf[i++] = '\0';

        // store line
        ptr = new_line(line_nb, i);
        if (first_line == NULL) {
            first_line = last_line = ptr;
            ptr->prev = NULL;
        } else {
            link_lines(last_line, ptr);
            last_line = ptr;
        }
        strcpy(ptr->chars, buf);
        if (line_nb == first_line_on_screen_nb)
            first_line_on_screen = ptr;
        line_nb++;
    }

    if (first_line_on_screen == NULL)
        first_line_on_screen = last_line;
    last_line->next = NULL;
    free(buf);

    // close connection to src_file
    fclose(src_file);

    // refresh parameters
    nb_line = line_nb - 1;
    has_been_changes = 0;

    return 0;
}

int
write_file(char *file_name)
{
    FILE *dest_file = NULL;
    struct line *ptr;
    char *chars;
    int c;

    if (first_line != NULL) {
        dest_file = fopen(file_name, "w");

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

        fclose(dest_file);
    }

    return 0;
}


// MOVING **********************************************************************

struct pos
pos_of(int l, int x)
{
    struct pos res;

    res.l = l;
    res.x = x;

    return res;
}

struct pos
find_first_non_blanck(void)
{
    int i, x;
    struct line *cursor_line;

    cursor_line = get_line(y);
    for (i = x = cursor_line->length - 1; i >= 0; i--)
        if (!(is_blank(cursor_line->chars[i])))
            x = i;

    return pos_of(cursor_line->line_nb, x);
}

struct pos
find_matching_bracket(void)
{
    // TODO

}

/*struct pos
find_next_selection(int delta)
{
    int asked_nb, closest, n;
    struct selection *cursor;
    struct pos res;
        
    if (sel == NULL) {
        return pos_of(0, 0);
    } else {
        cursor = (struct selection *) malloc(sizeof(struct selection));
        cursor->l = first_line_on_screen->line_nb + y;
        cursor->x = x;
        closest = closest_nb(cursor);
        n = nb_sels();

        if ((closest == 0 && delta < 0)
            || (closest == n && delta > 0)) { // n - 1 ?
            free(cursor);
            return pos_of(0, 0);
        } else {
            asked_nb = closest + delta - 1*(delta > 0);
            if (asked_nb < 0)
                asked_nb = 0;
            if (asked_nb >= n)
                asked_nb = n - 1;
            cursor = get_sel(asked_nb);
            res = pos_of(cursor->l, cursor->x);
            free(cursor);
            return res;
        }
    }
}*/

int
find_start_of_block(int starting_line_nb, int nb)
{
    int l;
    struct line *ptr;

    l = starting_line_nb;
    ptr = get_line(l - first_line_on_screen->line_nb);

    while (nb--) {
        while (ptr->length == 1) {
            if (ptr->prev != NULL) {
                ptr = ptr->prev;
                l--;
            } else {
                return l;
            }
        }
        while (ptr->length > 1) {
            if (ptr->prev != NULL) {
                ptr = ptr->prev;
                l--;
            } else {
                return l;
            }
        }
    }

    return l + 1; 
}

int
find_end_of_block(int starting_line_nb, int nb)
{
    int l;
    struct line *ptr;

    l = starting_line_nb;
    ptr = get_line(l - first_line_on_screen->line_nb);

    while (nb--) {
        while (ptr->length == 1) {
            if (ptr->next != NULL) {
                ptr = ptr->next;
                l++;
            } else {
                return l;
            }
        }
        while (ptr->length > 1) {
            if (ptr->next != NULL) {
                ptr = ptr->next;
                l++;
            } else {
                return l;
            }
        }
    }

    return l; 
}

void
go_to(struct pos p)
{
    int delta, n;

    // reach line
    if (p.l > nb_line)
        p.l = nb_line;
    if (p.l < 1)
        p.l = 1;

    delta = p.l - first_line_on_screen->line_nb;
    if (0 <= delta && delta < screen_height - 1) {
        y = delta;
    } else if (delta >= screen_height - 1) {
        first_line_on_screen = get_line(delta - (screen_height - 2));
        y = screen_height - 2;
    } else {
        first_line_on_screen = get_line(delta);
        y = 0;
    }

    // adjust x
    if (p.x >= (n = get_line(y)->length))
        p.x = n - 1;
    if (p.x < 0)
        p.x = 0;
    x = p.x;
}


// SELECTIONS ******************************************************************

int
is_inf(struct selection *s1, struct selection *s2)
{
    // 1 if s1 < s2, else 0
    return (s1->l < s2->l || (s1->l == s2->l && s1->x < s2->x));
}

struct selection *
sel_of_pos(struct pos p, int temp)
{
    struct selection *res;

    res = (struct selection *) malloc(sizeof(struct selection));
    res->l = p.l;
    res->x = p.x;
    res->n = 0;
    res->temp = temp;
    res->next = NULL;

    return res;
}

struct pos
pos_of_curs(void)
{
    struct pos res;

    res.l = first_line_on_screen->line_nb + y;
    res.x = x;

    return res;
}

int
closest_after_nb(void)
{
    // only consider non temporary selections
    // TODO
}

struct pos
get_pos_sel(int nb)
{
    // only consider non temporary selections
    int i;
    struct selection *ptr;

    ptr = sel;
    while (ptr->temp)
        ptr = ptr->next;
    while (nb--) {
        ptr = ptr->next;
        while (ptr->temp)
            ptr = ptr->next;
    }

    return pos_of(ptr->l, ptr->x);
}

int
nb_sels(void)
{
    int nb;
    struct selection *ptr;

    if (sel != NULL) {
        nb = 1;
        ptr = sel;
        while (ptr->next != NULL) {
            ptr = ptr->next;
            nb++;
        }
        return nb;
    } else {
        return 0;
    }
}

void
empty_sels(void)
{
    struct selection *ptr;
    struct selection *old_ptr;

    if (sel != NULL) {
        ptr = sel;
        while (ptr->next != NULL) {
            old_ptr = ptr;
            ptr = ptr->next;
            free(old_ptr);
        }
        free(ptr);
    }

    sel = NULL;
}

void
merge_sels(struct selection *starting)
{
    // TODO: check for covering
    struct selection *new_sel;
    struct selection *old;
    struct selection *new;

    if (sel == NULL) {
        sel = starting;
    } else if (starting != NULL) {
        new_sel = NULL;
        old = sel;
        new = starting;

        if (is_inf(old, new)) {
            new_sel = sel = old;
            old = old->next;
        } else {
            new_sel = sel = new;
            new = new->next;
        }

        while (1) {
            if (new == NULL) {
                break;
            } else if (old == NULL) {
                break;
            } else if (is_inf(old, new)) {
                new_sel->next = old;
                old = old->next;
            } else {
                new_sel->next = new;
                new = new->next;
            }
            new_sel = new_sel->next;
        }

/*         if (new_sel == NULL) */
/*             tb_poll_event(&ev); */

        if (new == NULL) {
    //tb_poll_event(&ev);
            new_sel->next = old;
        } else {
    //tb_poll_event(&ev);
            new_sel->next = new;
        }
    //tb_poll_event(&ev);

        /*while (old != NULL) {
            new_sel->next = old;
            old = old->next;
            new_sel = new_sel->next;
        }
        while (new != NULL) {
            new_sel->next = new;
            new = new->next;
            new_sel = new_sel->next;
        }
        new_sel->next = NULL;*/
    }
}

void
add_running_sels(int temp)
{
    struct pos cursor_pos;
    struct selection *anchor_sel;
    struct selection *cursor_sel;
    int line_delta;

    cursor_pos = pos_of_curs();

    if (anchored) {
        anchor_sel = sel_of_pos(anchor, temp);
        cursor_sel = sel_of_pos(cursor_pos, temp);

        line_delta = cursor_pos.l - anchor.l;

        if (line_delta < 0) {
            if (line_delta < -1)
                add_range_sels(cursor_pos.l + 1, anchor.l - 1, temp);
            cursor_sel->n = get_line(y)->length - x;
            cursor_sel->next = anchor_sel;
            anchor_sel->n = anchor.x;
            anchor_sel->x = 0;
            merge_sels(cursor_sel);
        } else if (line_delta > 0) {
            if (line_delta > 1)
                add_range_sels(anchor.l + 1, cursor_pos.l - 1, temp);
            anchor_sel->n = get_line(anchor.l -
                first_line_on_screen->line_nb)->length - anchor.x;
            anchor_sel->next = cursor_sel;
            cursor_sel->n = x;
            cursor_sel->x = 0;
            merge_sels(anchor_sel);
        } else {
            if (cursor_pos.x <= anchor.x) {
                cursor_sel->n = anchor.x - cursor_pos.x;
                free(anchor_sel);
                merge_sels(cursor_sel);
            } else {
                anchor_sel->n = cursor_pos.x - anchor.x;
                free(cursor_sel);
                merge_sels(anchor_sel);
            }
        }
    } else {
        merge_sels(sel_of_pos(cursor_pos, temp));
    }
}

void
add_range_sels(int start, int end, int temp)
{
    int i;
    struct line *line;
    struct selection *new;
    struct selection *old;

    line = get_line(start - first_line_on_screen->line_nb);

    for (i = start; i <= end; i++) {
        new = (struct selection *) malloc(sizeof(struct selection));
        new->l = i;
        new->x = 0;
        new->n = line->length;
        new->temp = temp;

        if (i == start) {
            sel = new;
        } else {
            old->next = new;
        }
        old = new;

        if (line->next != NULL)
            line = line->next;
    }
    new->next = NULL;
}

void
shift_sels(struct pos starting, struct pos ending, struct pos delta)
{
    struct selection *s;
    struct selection *start;
    struct selection *end;

    start = sel_of_pos(starting, 1); 
    end = sel_of_pos(ending, 1);

    s = sel;
    while (s) {
        if (is_inf(start, s) && is_inf(s, end)) {
            s->l += delta.l;
            s->x += delta.x;
        }
        s = s->next;
    }

    free(start);
    free(end);
}

void
delete_temp_sels(void)
{
    struct selection *old;
    struct selection *new;
    
    old = NULL;

    new = sel;
    while (new != NULL) {
        if (new->temp == 0) {
            if (old == NULL) {
                old = sel = new;
            } else {
                old->next = new;
                old = new;
            }
        }
        new = new->next;
    }
        
    if (old == NULL)
        sel = NULL;
}

void
search(void)
{
    // TODO
}


// ACTIONS ON SELECTIONS *******************************************************

void
act(void (*process)(struct line *, struct selection *), int line_op)
{
    struct selection *s;
    struct line *l;
    int old_line_nb;

    s = sel;
    l = get_line(s->l - first_line_on_screen->line_nb);
    old_line_nb = 0;
    while (s != NULL) {
        while (l->line_nb < s->l)
            l = l->next;
        if (!line_op || s->l > old_line_nb) {
            process(l, s);
        }
        old_line_nb = s->l;
        s = s->next;
    }
    
    has_been_changes = 1;
}

void
insert(struct line *l, struct selection *s)
{
    char *new_chars;
    char *old_chars;
    int i;

    new_chars = (char *) malloc(l->length + 1);
    for (i = 0; i < s->x; i++)
        new_chars[i] = l->chars[i];
    new_chars[s->x] = ev.ch;
    for (i = s->x; i < l->length; i++)
        new_chars[i+1] = l->chars[i];
    old_chars = l->chars;
    l->chars = new_chars;
    free(old_chars);
    l->length++;

    // TODO: shift x for selections of the same line
}

void
split_lines(struct line *l, struct selection *s)
{
    // TODO
}

void
suppress(struct line *l, struct selection *s)
{
    // TODO
}

void
concatenate_lines(struct line *l, struct selection *s)
{
    // TODO
}

void
indent(struct line *l, struct selection *s)
{
    // TODO: change selections length, and x
    int i, j;
    char *new_chars;
    char *old_chars;

    if (l->length == 1)
        return;

    if (asked_indent > 0) {
        new_chars = (char *) malloc(l->length + asked_indent);
        for (i = 0; i < asked_indent; i++)
            new_chars[i] = ' ';
        for (i = 0; i < l->length; i++)
            new_chars[i + asked_indent] = l->chars[i];
        l->length += asked_indent;
    } else {
        for (i = 0; i < (-asked_indent) && l->chars[i] == ' '; i++)
            ;
        printf("%d\n", i);
        new_chars = (char *) malloc(l->length - i);
        for (j = 0; j < l->length - i; j++)
            new_chars[j] = l->chars[j + i];
        l->length -= i;
    }
    old_chars = l->chars;
    l->chars = new_chars;
    free(old_chars);
}

void
comment(struct line *l, struct selection *s)
{
    // TODO: change selections length ?, and x
    int d, i, j, syntax_length;
    char *new_chars;
    char *old_chars;
    char *comment_syntax = *(syntax->comment);

    if (l->length == 1)
        return;
    
    for (i = 0; i < l->length && l->chars[i] == ' '; i++)
        ;
    if (i < l->length) {
        syntax_length = strlen(comment_syntax);

        d = -1;
        for (j = 0; j < syntax_length; j++)
            if (i + j < l->length && l->chars[i+j] != comment_syntax[j])
                d = 1;
        
        new_chars = (char *) malloc(l->length + d * syntax_length);
        for (j = 0; j < i; j++)
            new_chars[j] = ' ';
        if (d == 1) {
            for (j = 0; j < syntax_length; j++)
                new_chars[i + j] = comment_syntax[j];
            for (j = i; j < l->length; j++)
                new_chars[j + syntax_length] = l->chars[j];
        } else {
            for (j = i; j < l->length - syntax_length; j++)
                new_chars[j] = l->chars[j + syntax_length];
        }
        
        old_chars = l->chars;
        l->chars = new_chars;
        free(old_chars);
        l->length += d * syntax_length;
    }
}

void
lower(struct line *l, struct selection *s)
{
    int i;
    char c;

    for (i = s->x; i < s->x + s->n; i++) {
        if ('A' <= (c = l->chars[i]) && c <= 'Z') {
            l->chars[i] += 'a' - 'A';
        }
    }
}

void
upper(struct line *l, struct selection *s)
{
    int i;
    char c;

    for (i = s->x; i < s->x + s->n; i++) {
        if ('a' <= (c = l->chars[i]) && c <= 'z') {
            l->chars[i] += 'A' - 'a';
        }
    }
}

void
replace(struct line *l, struct selection *s)
{
    // TODO
}


// GRAPHICAL *******************************************************************

int
resize(int width, int height)
{
    if ((screen_width = width) < MIN_WIDTH ||
        (screen_height = height) < MIN_HEIGHT)
        return ERR_TERM_NOT_BIG_ENOUGH;

    return 0;
}

void
echo(const char *str)
{
    strcpy(dialog_chars, str);
}

struct selection *
print_line(const char *chars, int length, int line_nb, struct selection *s, int screen_line)
{
    // variables
    int i, j, color, nb_to_color;
    char c, nc;
    int *fg;
    int *bg;
    fg = (int *) malloc(length * sizeof(int));
    bg = (int *) malloc(length * sizeof(int));

    // foreground
    if (settings.syntax_highlight) {
        i = 0;
        while (i < length) {
            color = COLOR_DEFAULT_FG;
            nb_to_color = 1;
            if (is_word_char(c = chars[i])) {
                for (j = 0; is_word_char(nc = chars[i+j]) || is_number(nc); j++)
                        ;
                if (is_in(*(syntax->keywords), chars, i, j)) {
                    color = COLOR_KEYWORD;
                } else if (is_in(*(syntax->flow_control), chars, i, j)) {
                    color = COLOR_FLOW_CONTROL;
                } else if (is_in(*(syntax->built_ins), chars, i, j)) {
                    color = COLOR_BUILT_IN;
                }
                nb_to_color = j;
            } else if (is_number(c) || (i+1 < length && (c == '-' || c == '.') &&
                (is_number(nc = chars[i+1]) || nc == '.'))) {
                for (j = 1; j+i < length &&
                        (is_number(nc = chars[i+j]) || nc == '.'); j++)
                    ;
                color = COLOR_NUMBER;
                nb_to_color = j;
            } else if (c == '"' || c == '\'') {
                for (j = 1; j+i < length && chars[i+j] != c; j++)
                    if (chars[i+j] == '\\')
                        j++;
                color = COLOR_STRING;
                nb_to_color = (i+j+1 >= length) ? (length - i) : (j + 1);
            } else if (is_in(*(syntax->comment), chars, i, strlen(*(syntax->comment)) - 1)) {
                color = COLOR_COMMENT;
                nb_to_color = length - i;
            } else { // TODO: check for rules
            }
    
            for (j = 0; j < nb_to_color; j++)
                fg[i++] = color;
        }
    } else {
        for (i = 0; i < length; i++)
            fg[i] = COLOR_DEFAULT_FG;
    }
    
    // background
    for (i = 0; i < length; i++)
        bg[i] = COLOR_DEFAULT_BG;
    while (s != NULL && s->l < line_nb)
        s = s->next;
    while (s != NULL && s->l == line_nb) {
        for (i = 0; i < s->n && s->x + i < length; i++)
            bg[s->x + i] = COLOR_SELECTIONS_BG;
        s = s->next;
    } 
    // TODO: highlight matching bracket

    // actual printing
    for (i = 0; i < length; i++)
        tb_set_cell(i, screen_line, chars[i], fg[i], bg[i]);
    for (; i < screen_width; i++)
        tb_set_cell(i, screen_line, ' ', COLOR_DEFAULT_FG, COLOR_DEFAULT_BG);

    // forget about fg, bg
    free(fg);
    free(bg);

    return s;
}

void
print_dialog(void)
{
    int i;

    tb_print(0, screen_height - 1, 0, 0, dialog_chars);
    for (i = strlen(dialog_chars); i < screen_width - RULER_WIDTH; i++)
        tb_set_cell(i, screen_height - 1, ' ', 0, 0);
}

void
print_ruler(void)
{
    int i;

    for (i = 0; i < RULER_WIDTH; i++)
        tb_set_cell(screen_width - RULER_WIDTH + i, screen_height - 1, ' ', 0, 0);
    tb_printf(screen_width - RULER_WIDTH, screen_height - 1, 0, 0,
        "%d,%d", first_line_on_screen->line_nb + y, x);
}

void
print_all(void)
{
    struct selection *s;
    struct line *ptr;
    int sc_line;

    tb_clear();

    s = sel;
    ptr = first_line_on_screen;
    for (sc_line = 0; sc_line < screen_height - 1; sc_line++) {
        s = print_line(ptr->chars, ptr->length, ptr->line_nb, s, sc_line);
        if (ptr->next == NULL)
            break;
        ptr = ptr->next;
    }

    tb_set_cursor(x, y);
    print_dialog();
    print_ruler();
}


// INTERACTION *****************************************************************

int
dialog(const char *prompt, const char *specifics, int writable)
{
    // TODO

}

void
display_help(void)
{
    // TODO: do better
    echo("Quit the editor and run `edit --help` for complete help.");
}
