#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "config.h"


// ERROR CODES *****************************************************************

#define ERR_FILE_CONNECTION         1
#define ERR_UNICODE_OR_UTF8         2
#define ERR_MALLOC                  3
#define ERR_TERM_NOT_BIG_ENOUGH     4


// CONSTANTS *******************************************************************

#define INTERFACE_WIDTH             (MIN_WIDTH - RULER_WIDTH)
#define INTERFACE_MEM_LENGTH        (4*INTERFACE_WIDTH + 1)
#define DEFAULT_BUF_SIZE            (1 << 7)


// STRUCTS *********************************************************************

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


// FUNCTIONS *******************************************************************

// utils.c
#define MIN(A, B)                   (((A) < (B)) ? (A) : (B))
#define MAX(A, B)                   (((A) > (B)) ? (A) : (B))
int is_word_char(char c);
int utf8_char_length(char c);
int unicode_char_length(uint32_t c);
uint32_t unicode(const char *chars, int k, int len);
void insert_utf8(char *chars, int k, int len, uint32_t c);
int get_str_index(const char *chars, int x);
void decrement(const char *chars, int *i, int *k, int goal);
int is_in(const char *list, const char *chars, int x, int length);
void *_malloc(int size);

// file.c
void load_file(int first_line_on_screen_nb);
void write_file(const char *file_name);

// movements.c
int move(struct line **l, int *dx, int sens);
struct pos pos_of(int l, int x);
void unwrap_pos(struct pos p);
int find_first_non_blank(void);
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
struct pos column_sel(int m);
struct selection *merge_sel(struct selection *a, struct selection *b);
struct selection *range_lines_sel(int start, int end, struct selection *next);
struct selection *running_sel(void);
struct selection *search(struct selection *a);
int select_word_under_cursor(void);
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
void insert_line(int line_nb, int ml, int dl);
void move_line(int delta);
void empty_clip(int was_defined);
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
void autocomplete(struct line *l, struct selection *s);

// marks.c
int mark_subpatterns(const char *chars, int dl, int ss, int x, int n);
int mark_fields(const char *chars, int x, int n);

// interaction.c
int dialog(const char *prompt, struct interface *interf, int refresh);
int set_parameter(const char *assign);
int parse_range(const char *range);
void load_lang(const char *extension);

// graphical.c
void init_termbox(void);
int resize(int width, int height);
struct selection *print_line(const struct line *l, struct selection *s,
    int screen_line);
void print_dialog(void);
void print_ruler(void);
void print_all(void);


// VARIABLES *******************************************************************

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
