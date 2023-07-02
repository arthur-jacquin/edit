// see LICENSE file for copyright and license details

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "config.h"
#include "languages.h"
#define TB_IMPL
#include "termbox.h"

#define ERR_FILE_CONNECTION         1
#define ERR_UNICODE_OR_UTF8         2
#define ERR_MALLOC                  3
#define ERR_TERM_NOT_BIG_ENOUGH     4

#define VERSION                     "0.2.1"
#define HELP_MESSAGE                "Help available at https://jacquin.xyz/edit"
#define TERM_NOT_BIG_ENOUGH         "Terminal is too small."

#define INTERFACE_WIDTH             (MIN_WIDTH - RULER_WIDTH)
#define INTERFACE_MEM_LENGTH        (4*INTERFACE_WIDTH + 1)
#define DEFAULT_BUF_SIZE            (1 << 7)

#define INIT_INTERFACE(I, S)        strcpy(I.current, S); strcpy(I.previous, S);
#define MOVE_SEL_LIST(A, B)         forget_sel_list(B); B = A; A = NULL;
#define echo(MESSAGE)               strcpy(message, MESSAGE)
#define echof(PATTERN, INTEGER)     sprintf(message, PATTERN, INTEGER)
#define way(DIRECT_CONDITION)       ((DIRECT_CONDITION) ? m : -m)


// TYPES

struct line {                       // doubly linked list of lines
    struct line *prev;              // pointer to previous line, NULL if none
    struct line *next;              // pointer to next line, NULL if none
    int line_nb;                    // line number, between 1 and nb_lines
    int ml, dl;                     // length in memory, on screen
    char *chars;                    // pointer to UTF-8, NULL-ended string
};
#define is_first_line(L)            ((L)->prev == NULL)
#define is_last_line(L)             ((L)->next == NULL)

struct pos {                        // position in file
    int l, x;                       // line number, column
};

struct selection {                  // sorted list of non-overlapping selections
    int l, x, n;                    // line number, column, number of characters
    struct selection *next;         // pointer to next selection, NULL if none
};

struct interface {                  // interfaces for dialog mode
    char current[INTERFACE_MEM_LENGTH], previous[INTERFACE_MEM_LENGTH];
};

struct substring {                  // marks a substring in an original string
    int st, mst;                    // starting position (characters, bytes)
    int n, mn;                      // length (characters, bytes)
};


// FUNCTIONS DECLARATIONS

// actions.c
void act(void (*process)(struct line *, struct selection *), int line_op);
void lower(struct line *l, struct selection *s);
void upper(struct line *l, struct selection *s);
void insert(struct line *l, struct selection *s);
void split(struct line *l, struct selection *s);
void indent(struct line *l, struct selection *s);
void comment(struct line *l, struct selection *s);
void suppress(struct line *l, struct selection *s);
void replace(struct line *l, struct selection *s);
void autocomplete(struct line *l, struct selection *s);

// file.c
void load_file(int first_line_on_screen_nb);
void write_file(const char *file_name);

// graphical.c
void init_termbox(void);
int resize(int width, int height);
struct selection *print_line(const struct line *l, struct selection *s,
    int screen_line);
void print_dialog(void);
void print_ruler(void);
void print_all(void);

// interaction.c
int dialog(const char *prompt, struct interface *interf, int refresh);
int parse_assign(const char *assign);
int parse_range(const char *range);
void parse_lang(const char *extension);

// lines.c
struct line *get_line(int delta_from_first_line_on_screen);
struct line *create_line(int line_nb, int ml, int dl);
void link_lines(struct line *l1, struct line *l2);
void shift_line_nb(struct line *start, int min, int max, int delta);
void forget_lines(struct line *start);
int replace_chars(struct line *l, struct selection *a, int start, int n,
    int new_n, int nb_bytes);
void break_line(struct line *l, struct selection *s, int start);
void concatenate_line(struct line *l, struct selection *s);
void insert_line(int line_nb, int ml, int dl);
void move_line(int delta);
void empty_clip(int was_defined);
void copy_to_clip(int starting_line_nb, int nb);
void move_to_clip(int starting_line_nb, int nb);
void insert_clip(struct line *starting_line, int below);

// marks.c
int mark_subpatterns(const char *sp, const char *chars, int dl, int ss, int sx,
    int n);
int mark_fields(const char *chars, int sx, int n);

// movements.c
int move(struct line **l, int *dx, int sens);
struct pos pos_of(int l, int x);
void unwrap_pos(struct pos p);
int find_first_non_blank(const struct line *l);
struct pos find_start_of_word(int n);
struct pos find_matching_bracket(void);
struct pos find_next_selection(int delta);
int find_block_delim(int starting_line_nb, int nb);
void move_to_cursor(void);

// selections.c
struct selection *create_sel(int l, int x, int n, struct selection *next);
int nb_sel(struct selection *a);
void forget_sel_list(struct selection *a);
void reset_selections(void);
int is_inf(struct pos p1, struct pos p2);
struct pos pos_of_sel(struct selection *s);
struct pos pos_of_cursor(void);
int index_closest_after_cursor(struct selection *a);
struct pos get_pos_of_sel(struct selection *a, int index);
int column_sel(int m);
struct selection *merge_sel(struct selection *a, struct selection *b);
struct selection *range_lines_sel(int start, int end, struct selection *next);
struct selection *running_sel(void);
struct selection *search(struct selection *a);
int select_word_under_cursor(void);
void shift_sel_line_nb(struct selection *a, int min, int max, int delta);
void move_sel_end_of_line(struct selection *a, int l, int x, int concatenate);
void remove_sel_line_range(int min, int max);
void reorder_sel(int l, int nb, int new_l);

// utils.c
#define ABS(A)                      (((A) < 0) ? -(A) : (A))
#define MIN(A, B)                   (((A) < (B)) ? (A) : (B))
#define MAX(A, B)                   (((A) > (B)) ? (A) : (B))
int is_word_char(char c);
int is_word_boundary(const char *chars, int k);
int utf8_char_length(char c);
int unicode_char_length(uint32_t c);
uint32_t unicode(const char *chars, int k, int len);
int compare_chars(const char *s1, int k1, const char *s2, int k2);
void insert_utf8(char *chars, int k, int len, uint32_t c);
int get_str_index(const char *chars, int x);
void decrement(const char *chars, int *i, int *k, int goal);
int is_in(const char *list, const char *chars, int x, int length);
void *_malloc(int size);


// GLOBALS VARIABLES

// file informations
struct interface file_name_int;     // name of the file
struct line *first_line;            // pointer to first line in the file
struct line *first_line_on_screen;  // pointer to first line on screen
#define first_line_nb               (first_line_on_screen->line_nb)
int nb_lines;                       // number of lines in file

// positions
int y, x;                           // cursor position in file area
struct pos anchor;                  // anchor position (if anchored)

// selections lists
struct selection *saved, *running, *displayed;

// settings
struct {
    int case_sensitive;
    char field_separator;
    int highlight_selections;
    struct lang *syntax;            // pointer to selected syntax, NULL if none
    int syntax_highlight;
    int tab_width;
} settings;

// flags
int anchored;
int attribute_x;                    // prefer new x value to saved one
int has_been_changes;
int has_been_invalid_resizing;
int in_insert_mode;

// additional parameters for processing functions
int asked_indent, asked_remove;
uint32_t to_insert;

// search and replace engine
struct interface search_pattern, replace_pattern;
struct substring fields[10], subpatterns[10];


// FUNCTION DEFINITIONS, MAIN

#include "actions.c"
#include "file.c"
#include "graphical.c"
#include "interaction.c"
#include "lines.c"
#include "marks.c"
#include "movements.c"
#include "selections.c"
#include "utils.c"

int
main(int argc, char *argv[])
{
    struct tb_event ev;             // struct to retrieve events
    struct interface range_int;     // interface for custom range of lines
    struct interface settings_int;  // interface for changing a setting
    struct interface command_int;   // interface for running a shell command
    int m;                          // multiplier

    int l1, l2, old_line_nb;
    struct pos p;


    // INIT VARIABLES

    y = x = 0; attribute_x = 1;
    saved = running = displayed = NULL;
    m = anchored = has_been_invalid_resizing = in_insert_mode = 0;
    empty_clip(0);

    settings.case_sensitive = CASE_SENSITIVE;
    settings.field_separator = FIELD_SEPARATOR;
    settings.highlight_selections = HIGHLIGHT_SELECTIONS;
    settings.syntax_highlight = SYNTAX_HIGHLIGHT;
    settings.tab_width = TAB_WIDTH;

    INIT_INTERFACE(search_pattern, "")
    INIT_INTERFACE(replace_pattern, "")
    INIT_INTERFACE(range_int, "")
    INIT_INTERFACE(settings_int, "")
    INIT_INTERFACE(command_int, "")

    init_termbox();
    echo(WELCOME_MESSAGE);


    // PARSE ARGUMENTS

    if (argc < 2 || !strcmp(argv[1], "--help") || !strcmp(argv[1], "-h")) {
        printf("%s\n", HELP_MESSAGE);
        return 0;
    } else if (!strcmp(argv[1], "--version") || !strcmp(argv[1], "-v")) {
        printf("%s\n", VERSION);
        return 0;
    } else {
        INIT_INTERFACE(file_name_int, argv[1])
        load_file(1);
    }


    // MAIN LOOP

    while (1) {
        // quit if has been invalid resizing
        if (has_been_invalid_resizing) {
            if (has_been_changes)
                write_file(BACKUP_FILE_NAME);
            tb_shutdown();
            fprintf(stderr, "%s\n", TERM_NOT_BIG_ENOUGH);
            return ERR_TERM_NOT_BIG_ENOUGH;
        }

        // go to correct position, compute new displayed selections
        move_to_cursor();
        forget_sel_list(running);
        running = running_sel();
        forget_sel_list(displayed);
        displayed = merge_sel(running, saved);

        // refresh screen and wait for input
        if (in_insert_mode)
            echo(INSERT_MODE_MESSAGE);
        print_all();
        tb_present();
        tb_poll_event(&ev);

        // process input
        switch (ev.type) {
        case TB_EVENT_KEY:
            if (ev.ch && in_insert_mode) {
                to_insert = ev.ch;
                act(insert, 0);
                break;
            } else if ((m && ev.ch == '0') || ('1' <= ev.ch && ev.ch <= '9')) {
                m = 10*m + ev.ch - '0';
                echof(MULTIPLIER_MESSAGE_PATTERN, m);
                break;
            }
            echo("");
            if (m == 0)
                m = 1;
            if (ev.ch) {
                switch (ev.ch) {
                case KB_HELP:
                    echo(HELP_MESSAGE);
                    break;
                case KB_QUIT:
                case KB_FORCE_QUIT:
                    if (ev.ch == KB_QUIT && has_been_changes) {
                        echo(UNSAVED_CHANGES_MESSAGE);
                    } else {
                        tb_shutdown();
                        return 0;
                    }
                    break;
                case KB_WRITE:
                case KB_WRITE_AS:
                    if (ev.ch == KB_WRITE && !has_been_changes) {
                        echo(NOTHING_TO_WRITE_MESSAGE);
                    } else if (ev.ch == KB_WRITE ||
                        dialog(SAVE_AS_PROMPT, &file_name_int, 0)) {
                        write_file(file_name_int.current);
                        if (ev.ch == KB_WRITE_AS)
                            parse_lang(file_name_int.current);
                        has_been_changes = 0;
                        echo(FILE_SAVED_MESSAGE);
                    }
                    break;
                case KB_RELOAD:
                    if (has_been_changes) {
                        old_line_nb = first_line_nb + y;
                        load_file(first_line_nb);
                        y = old_line_nb - first_line_nb;
                        echo(FILE_RELOADED_MESSAGE);
                    } else {
                        echo(NOTHING_TO_REVERT_MESSAGE);
                    }
                    break;
                case KB_CHANGE_SETTING:
                    if (dialog(CHANGE_SETTING_PROMPT, &settings_int, 0))
                        if (!parse_assign(settings_int.current))
                            echo(INVALID_ASSIGNMENT_MESSAGE);
                    break;
                case KB_RUN_SHELL_COMMAND:
                    if (dialog(COMMAND_PROMPT, &command_int, 0)) {
                        tb_shutdown();
                        system(command_int.current);
                        getchar();
                        init_termbox();
                    }
                    break;
                case KB_INSERT_START_LINE:
                case KB_INSERT_END_LINE:
                case KB_INSERT_LINE_BELOW:
                case KB_INSERT_LINE_ABOVE:
                    reset_selections();
                    if (ev.ch == KB_INSERT_LINE_BELOW ||
                        ev.ch == KB_INSERT_LINE_ABOVE) {
                        y += (ev.ch == KB_INSERT_LINE_BELOW) ? 1 : 0;
                        insert_line(first_line_nb + y, 1, 0);
                    }
                    x = (ev.ch == KB_INSERT_END_LINE) ? get_line(y)->dl : 0;
                    attribute_x = 1;
                    // fall-through
                case KB_INSERT_MODE:
                    in_insert_mode = 1;
                    break;
                case KB_MOVE_MATCHING:
                    unwrap_pos(find_matching_bracket());
                    break;
                case KB_MOVE_START_LINE:
                    x = 0; attribute_x = 1;
                    break;
                case KB_MOVE_NON_BLANK:
                    x = find_first_non_blank(get_line(y)); attribute_x = 1;
                    break;
                case KB_MOVE_END_LINE:
                    x = get_line(y)->dl; attribute_x = 1;
                    break;
                case KB_MOVE_END_FILE:
                    m = nb_lines;
                    // fall-through
                case KB_MOVE_SPECIFIC_LINE:
                    y = m - first_line_nb;
                    break;
                case KB_MOVE_NEXT_CHAR:
                case KB_MOVE_PREV_CHAR:
                    x += way(ev.ch == KB_MOVE_NEXT_CHAR); attribute_x = 1;
                    break;
                case KB_MOVE_NEXT_LINE:
                case KB_MOVE_PREV_LINE:
                    y += way(ev.ch == KB_MOVE_NEXT_LINE);
                    break;
                case KB_MOVE_NEXT_WORD:
                case KB_MOVE_PREV_WORD:
                    unwrap_pos(find_start_of_word(
                        way(ev.ch == KB_MOVE_NEXT_WORD)));
                    break;
                case KB_MOVE_NEXT_BLOCK:
                case KB_MOVE_PREV_BLOCK:
                    y = find_block_delim(first_line_nb + y,
                        way(ev.ch == KB_MOVE_NEXT_BLOCK)) - first_line_nb;
                    break;
                case KB_MOVE_NEXT_SEL:
                case KB_MOVE_PREV_SEL:
                    p = find_next_selection(way(ev.ch == KB_MOVE_NEXT_SEL));
                    if (p.l)
                        unwrap_pos(p);
                    else
                        echo((ev.ch == KB_MOVE_NEXT_SEL) ?
                            NO_SEL_DOWN_MESSAGE : NO_SEL_UP_MESSAGE);
                    break;
                case KB_MOVE_JUMP_TO_NEXT:
                    MOVE_SEL_LIST(saved, running)
                    saved = range_lines_sel(first_line_nb + y, nb_lines, NULL);
                    if (dialog(SEARCH_PATTERN_PROMPT, &search_pattern, 1)) {
                        MOVE_SEL_LIST(displayed, saved);
                        if ((p = find_next_selection(m)).l)
                            unwrap_pos(p);
                        else
                            echo(NO_SEL_DOWN_MESSAGE);
                    }
                    MOVE_SEL_LIST(running, saved)
                    break;
                case KB_SEL_ANCHOR:
                    if (!anchored)
                        anchor = pos_of_cursor();
                    anchored = 1 - anchored;
                    break;
                case KB_SEL_COLUMN:
                    if (anchored && anchor.l != first_line_nb + y)
                        echo(COLUMN_SEL_ERROR_MESSAGE);
                    else
                        y += column_sel(m);
                    break;
                case KB_SEL_DISPLAY_COUNT:
                    echof(SELECTIONS_MESSAGE_PATTERN, nb_sel(saved));
                    break;
                case KB_SEL_CURSOR_LINE:
                case KB_SEL_ALL_LINES:
                case KB_SEL_LINES_BLOCK:
                    if (ev.ch == KB_SEL_CURSOR_LINE) {
                        l1 = l2 = first_line_nb + y;
                    } else if (ev.ch == KB_SEL_ALL_LINES) {
                        l1 = 1;
                        l2 = nb_lines;
                    } else {
                        l1 = find_block_delim(first_line_nb + y, -1);
                        l2 = find_block_delim(l1, m);
                    }
                    forget_sel_list(saved);
                    saved = range_lines_sel(l1, l2, NULL);
                    break;
                case KB_SEL_CUSTOM_RANGE:
                    if (dialog(RANGE_PROMPT, &range_int, 0))
                        if (!parse_range(range_int.current))
                            echo(INVALID_RANGE_MESSAGE);
                    break;
                case KB_SEL_FIND:
                case KB_SEL_SEARCH:
                case KB_SEL_APPEND:
                    if (ev.ch == KB_SEL_APPEND)
                        anchored = 0;
                    if (ev.ch == KB_SEL_APPEND ||
                        dialog(SEARCH_PATTERN_PROMPT, &search_pattern, 1)) {
                        MOVE_SEL_LIST(displayed, saved)
                    }
                    break;
                case KB_SEL_CURSOR_WORD:
                    if (!search_word_under_cursor())
                        echo(NO_WORD_CURSOR_MESSAGE);
                    break;
                case KB_ACT_SUPPRESS:
                    asked_remove = m;
                    act(suppress, 0);
                    break;
                case KB_ACT_LOWERCASE:
                    act(lower, 0);
                    break;
                case KB_ACT_UPPERCASE:
                    act(upper, 0);
                    break;
                case KB_ACT_INCREASE_INDENT:
                case KB_ACT_DECREASE_INDENT:
                    asked_indent = way(ev.ch == KB_ACT_INCREASE_INDENT);
                    act(indent, 1);
                    break;
                case KB_ACT_COMMENT:
                    if (settings.syntax && settings.syntax->highlight_elements)
                        act(comment, 1);
                    break;
                case KB_ACT_REPLACE:
                    if (dialog(REPLACE_PATTERN_PROMPT, &replace_pattern, 0))
                        act(replace, 0);
                    break;
                case KB_CLIP_YANK_LINE:
                case KB_CLIP_YANK_BLOCK:
                case KB_CLIP_DELETE_LINE:
                case KB_CLIP_DELETE_BLOCK:
                    if (ev.ch == KB_CLIP_YANK_LINE ||
                        ev.ch == KB_CLIP_DELETE_LINE) {
                        l1 = first_line_nb + y;
                    } else {
                        l1 = find_block_delim(first_line_nb + y, -1);
                        m = find_block_delim(l1, m) - l1 + 1;
                    }
                    if (ev.ch == KB_CLIP_YANK_LINE ||
                        ev.ch == KB_CLIP_YANK_BLOCK) {
                        copy_to_clip(l1, m);
                    } else {
                        move_to_clip(l1, m);
                    }
                    break;
                case KB_CLIP_PASTE_AFTER:
                case KB_CLIP_PASTE_BEFORE:
                    while (m--)
                        insert_clip(get_line(y), ev.ch == KB_CLIP_PASTE_AFTER);
                    break;
                }
            } else if (ev.key) {
                switch (ev.key) {
                case TB_KEY_ENTER:
                    if (in_insert_mode)
                        act(split, 0);
                    else
                        y += m;
                    break;
                case TB_KEY_ARROW_RIGHT:
                case TB_KEY_ARROW_LEFT:
                    x += way(ev.key == TB_KEY_ARROW_RIGHT); attribute_x = 1;
                    break;
                case TB_KEY_ARROW_DOWN:
                case TB_KEY_ARROW_UP:
                    if (ev.mod == TB_MOD_SHIFT)
                        move_line(way(ev.key == TB_KEY_ARROW_DOWN));
                    else
                        y += way(ev.key == TB_KEY_ARROW_DOWN);
                    break;
                case TB_KEY_ESC:
                    if (in_insert_mode)
                        in_insert_mode = 0;
                    else
                        reset_selections();
                    echo("");
                    break;
                case TB_KEY_BACKSPACE:
                case TB_KEY_BACKSPACE2:
                case TB_KEY_DELETE:
                    asked_remove = way(ev.key == TB_KEY_DELETE);
                    act(suppress, 0);
                    break;
                case TB_KEY_TAB:
                case TB_KEY_BACK_TAB:
                    asked_indent = way(ev.key == TB_KEY_TAB);
                    act(indent, 1);
                    break;
#ifdef ENABLE_AUTOCOMPLETE
                case KB_ACT_AUTOCOMPLETE:
                    act(autocomplete, 0);
                    break;
#endif // ENABLE_AUTOCOMPLETE
                }
            }
            m = 0;
            break;

#ifdef MOUSE_SUPPORT
        case TB_EVENT_MOUSE:
            switch (ev.key) {
            case TB_KEY_MOUSE_LEFT:
                if (ev.y < screen_height - 1) {
                    y = ev.y;
                    x = ev.x - LINE_NUMBERS_WIDTH; attribute_x = 1;
                }
                break;
            case TB_KEY_MOUSE_WHEEL_UP:
                old_line_nb = first_line_nb + y;
                first_line_on_screen = get_line(-SCROLL_LINE_NUMBER);
                y = MIN(old_line_nb - first_line_nb,
                    screen_height - 2 - scroll_offset);
                break;
            case TB_KEY_MOUSE_WHEEL_DOWN:
                old_line_nb = first_line_nb + y;
                first_line_on_screen = get_line(SCROLL_LINE_NUMBER);
                y = MAX(old_line_nb - first_line_nb, scroll_offset);
                break;
            }
            break;
#endif // MOUSE_SUPPORT

        case TB_EVENT_RESIZE:
            has_been_invalid_resizing = resize(ev.w, ev.h);
            break;
        }
    }
}
