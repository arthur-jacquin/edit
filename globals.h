// INCLUDES
#include <stdio.h>
#include <stdlib.h>

#define TB_IMPL
#include "termbox.h"
#include "config.h"

// CONSTANTS
#define VERSION                     "alpha"
#define HELP_MESSAGE                "Refer to https://jacquin.xyz/edit for complete help."
#define MAX_CHARS                   (1 << 8)
#define INTERFACE_WIDTH             (MIN_WIDTH - RULER_WIDTH)
#define MIN_HEIGHT                  2
#define LANG_WIDTH                  5


// ERROR CODES *****************************************************************

// TODO
#define ERR_FILE_CONNECTION         2
// #define ERR_MALLOC                  3
#define ERR_TERM_NOT_BIG_ENOUGH     4
// #define ERR_UNICODE                 5


// STRUCTS *********************************************************************

struct line {                   // double linked list of lines
    struct line *prev;          // pointer to previous line, NULL if none
    struct line *next;          // pointer to next line, NULL if none
    int line_nb;                // line number, between 1 and nb_lines
    int ml, dl;                 // length in memory, on screen
    char *chars;                // pointer to UTF-8, NULL-ended string
};

struct pos {                    // position in file
    int l, x;                   // line number, column
};

struct selection {              // sorted list of selections
    int l, x, n;                // line number, column, number of characters
    struct selection *next;     // pointer to next line, NULL if end of list
};

struct substring {              // marks a substring in an original string
    int st, mst;                // starting position (characters, bytes)
    int n, mn;                  // length (characters, bytes)
};

struct interface {              // interface for dialog mode
    // at most INTERFACE_WIDTH 4-bytes UTF-8 characters + NULL terminator
    char current[4*INTERFACE_WIDTH + 1], previous[4*INTERFACE_WIDTH + 1];
};


// VARIABLES *******************************************************************

// UTF-8/Unicode management TODO comment
char first_bytes_mask[6] = {0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8};
char masks[4] = {0x7f, 0x1f, 0x0f, 0x07};
char utf8_start[4] = {0, 0xc0, 0xe0, 0xf0};
uint32_t unicode_delimiter[4] = {0x80, 0x800, 0x10000, 0x200000};
#define FIRST_BYTE_ACCENTUATED      0xc3

// file properties
struct interface file_name_int;     // interface for storing file name
struct line *first_line;            // pointer to first line in the file
int nb_lines;                       // number of lines in file

// settings
struct {
    int syntax_highlight;
    int highlight_selections;
    int case_sensitive;
    char field_separator;
    int tab_width;
    struct lang *syntax;
} settings;
struct interface settings_int;

// lines
struct line *first_line_on_screen;

// selections
struct selection *saved, *temp, *displayed;
struct interface range_int;

// search and replace engine
struct interface search_pattern, replace_pattern;
struct substring fields[10], subpatterns[10];

// clipboard
struct {
    struct line *start;
    int nb_lines;
} clipboard;

// graphical
int y, x;                           // cursor position in file area
int screen_height, screen_width;    // terminal dimensions
char dialog_chars[4*INTERFACE_WIDTH];

// editor variables
int anchored, in_insert_mode;
int has_been_changes;
int is_bracket;
int m;                              // multiplier
int asked_indent, asked_remove;
int has_been_invalid_resizing;
struct pos anchor;
struct pos matching_bracket;
struct tb_event ev;                 // struct to retrieve events


// FUNCTIONS *******************************************************************

// utils.c
int utf8_char_length(char c);
int unicode_char_length(uint32_t c);
int get_str_index(const char *chars, int x);
int is_blank(char c); // ctype.h
int is_word_char(char c); // ctype.h
int is_digit(char c); // ctype.h
int is_in(const char *list, const char *chars, int x, int length);
int is_first_line(const struct line *l); //#define
int is_last_line(const struct line *l); //#define
void init_interface(struct interface *interf, const char *chars);
void echo(const char *chars); //#define

// file.c
int load_file(const char *file_name, int first_line_on_screen_nb);
int write_file(const char *file_name);

// movements.c TODO
int move(struct line **l, int *dx, int sens);
struct pos pos_of(int l, int x);
struct pos find_first_non_blank(void);
struct pos find_start_of_word(int n);
struct pos find_matching_bracket(void);
struct pos find_next_selection(int delta);
int find_start_of_block(int starting_line_nb, int nb);
int find_end_of_block(int starting_line_nb, int nb);
void go_to(struct pos p);

// selections.c TODO
struct selection *create_sel(int l, int x, int n, struct selection *next);
int is_inf(struct pos p1, struct pos p2);
struct pos pos_of_sel(struct selection *s);
struct pos pos_of_cursor(void);
int index_closest_after_cursor(struct selection *a);
struct pos get_pos_of_sel(struct selection *a, int index);
int nb_sel(struct selection *a);
void forget_sel_list(struct selection *a);
void reset_selections(void);
struct pos column_sel(int m);
struct selection *merge_sel(struct selection *a, struct selection *b);
struct selection *range_lines_sel(int start, int end, struct selection *next);
struct selection *running_sel(void);
struct selection *search(struct selection *a);

void shift_sel_line_nb(struct selection *a, int min, int max, int delta);
void move_sel_end_of_line(struct selection *a, int l, int x, int concatenate);
void remove_sel_line_range(int min, int max);
void reorder_sel(int l, int new_l);

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
void insert_line(int asked_line_nb, int ml, int dl);
int move_line(int delta);
void copy_to_clip(int starting_line_nb, int nb);
void move_to_clip(int starting_line_nb, int nb);
void insert_clip(struct line *starting_line, int below);

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

// search_and_replace.c
int mark_pattern(const char *chars, int x, int n);
int mark_fields(const char *chars, int x, int n);

// interaction.c
int dialog(const char *prompt, struct interface *interf, int search);
int set_parameter(const char *assign);
int parse_range(const char *range);
void load_lang(const char *extension);

// graphical.c
int resize(int width, int height);
struct selection *print_line(const struct line *l, struct selection *s, int screen_line);
void print_dialog(void);
void print_ruler(void);
void print_all(void);
