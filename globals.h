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
#define LANG_WIDTH                  5


// ERROR CODES *****************************************************************

#define ERR_BAD_ARGUMENTS           1
#define ERR_FILE_CONNECTION         2
#define ERR_MALLOC                  3
#define ERR_TERM_NOT_BIG_ENOUGH     4

/*// TODO
#define ERR_TOO_LONG_LINE           4
#define ERR_INVALID_LINE_VALUE      5
#define ERR_UNICODE                 6

// TODO: change to enum ?
#define BLANK                       0
#define WORD_CHAR                   1
#define DIGIT                       2
#define ELSE                        3
*/


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

struct selection {              // list of selections
    int l, x, n;                // line number, column, number of characters
    struct selection *next;     // pointer to next line, NULL if end of list
};

struct substring {              // marks a substring in an original string
    int st, n;                  // starting position, number of characters
};

struct interface {              // interface for dialog mode
    char current[4*INTERFACE_WIDTH], previous[4*INTERFACE_WIDTH];
};


// VARIABLES *******************************************************************

// UTF-8 management
char masks[4] = {0x7f, 0x1f, 0x0f, 0x07};

// file properties
struct interface file_name_int;     // interface for storing file name
struct line *first_line;            // pointer to first line in the file
int nb_lines;                       // number of lines in file

// settings
struct {
    int autoindent;
    int syntax_highlight;
    int highlight_selections;
    int case_sensitive;
    char field_separator;
    int tab_width;
    char language[LANG_WIDTH];
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
    int nb_line;
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
struct pos anchor;
struct pos matching_bracket;
struct tb_event ev;                 // struct to retrieve events


// FUNCTIONS *******************************************************************

// utils.c
int utf8_char_length(char c);
int is_blank(char c);
int is_word_char(char c);
int is_digit(char c);
int is_in(const char *list, const char *chars, int x, int length);

// file.c
int load_file(char *file_name, int first_line_on_screen_nb);
int write_file(char *file_name);
void get_extension(void);
void load_lang(void);

// lines.c
int is_first_line(const struct line *l);
int is_last_line(const struct line *l);
struct line *create_line(int line_nb, int ml, int dl);
struct line *insert_line(int asked_line_nb, int ml, int dl);
struct line *get_line(int delta_from_first_line_on_screen);
void forget_line(struct line *l);
void forget_lines_list(struct line *start);
void link_lines(struct line *l1, struct line *l2);
void shift_line_nb(struct line *start, int min, int max, int delta);

// interaction.c
void init_interface(struct interface *interf, const char *chars);

// graphical.c
int resize(int width, int height);
void echo(const char *chars);
struct selection *print_line(struct line *l, struct selection *s, int screen_line);
void print_dialog(void);
void print_ruler(void);
void print_all(void);

// FUNCTIONS *******************************************************************
/*
// utils.c
int get_str_index(struct line *l, int x);
int type(char c);

// clipboard.c
int move_line(int delta);
void copy_to_clip(int starting_line_nb, int nb);
void move_to_clip(int starting_line_nb, int nb);
void insert_clip(struct line *starting_line, int below);


// movements.c
int move(struct line **l, int *dx, int sens);
struct pos pos_of(int l, int x);
struct pos find_first_non_blanck(void);
struct pos find_start_of_word(int n);
struct pos find_matching_bracket(void);
struct pos find_next_selection(int delta);
int find_start_of_block(int starting_line_nb, int nb);
int find_end_of_block(int starting_line_nb, int nb);
void go_to(struct pos p);

// selections.c
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

// actions.c
void act(void (*process)(struct line *, struct selection *), int line_op);
void insert(struct line *l, struct selection *s);
void split_lines(struct line *l, struct selection *s);
void suppress(struct line *l, struct selection *s);
void indent(struct line *l, struct selection *s);
void comment(struct line *l, struct selection *s);
void lower(struct line *l, struct selection *s);
void upper(struct line *l, struct selection *s);
void replace(struct line *l, struct selection *s);

// interaction.c
int dialog(const char *prompt, struct interface *interf, int refresh);
void display_help(void);
int set_parameter(char *assign);
int parse_range(char *range);
*/
