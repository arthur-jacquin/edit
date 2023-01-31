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

// TODO
#define ERR_TERM_NOT_BIG_ENOUGH     1
#define ERR_BAD_ARGUMENTS           2
#define ERR_MALLOC                  3
#define ERR_TOO_LONG_LINE           4
#define ERR_INVALID_LINE_VALUE      5

#define BLANK                       0
#define WORD_CHAR                   1
#define DIGIT                       2
#define ELSE                        3



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

struct interface {
    char current[INTERFACE_WIDTH];
    char previous[INTERFACE_WIDTH];
};



// FUNCTIONS *******************************************************************

// utils.c
int char_length(char c);
int is_blank(char c);
int is_word_char(char c);
int is_number(char c);
int type(char c);
int is_in(const char *list, const char *chars, int x, int length);

// lines.c
struct line *new_line(int line_nb, int length);
struct line *get_line(int delta_from_first_line_on_screen);
void free_lines(struct line *starting);
void link_lines(struct line *l1, struct line *l2);
void shift_line_nb(int starting_line_nb, int ending_line_nb, int shift);
struct line *insert_line(int asked_line_nb, int length);
int move_line(int delta);
void copy_to_clip(int starting_line_nb, int nb);
void move_to_clip(int starting_line_nb, int nb);
void insert_clip(struct line *starting_line, int below);

// file.c
int load_file(char *file_name, int first_line_on_screen_nb);
int write_file(char *file_name);
void get_extension(void);
void load_lang(void);

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
void concatenate_lines(struct line *l, struct selection *s);
void indent(struct line *l, struct selection *s);
void comment(struct line *l, struct selection *s);
void lower(struct line *l, struct selection *s);
void upper(struct line *l, struct selection *s);
void replace(struct line *l, struct selection *s);

// graphical.c
int resize(int width, int height);
void echo(const char *str);
struct selection *print_line(const char *chars, int length, int line_nb,
    struct selection *s, int screen_line);
void print_dialog(void);
void print_ruler(void);
void print_all(void);

// interaction.c
int dialog(const char *prompt, struct interface *interf, int refresh);
void display_help(void);
int set_parameter(char *assign);
int parse_range(char *range);



// VARIABLES *******************************************************************

struct {
    int autoindent;
    int syntax_highlight;
    int highlight_selections;
    int case_sensitive;
    char field_separator;
    int tab_width;
    char language[LANG_WIDTH];
} settings;

int anchored;
int in_insert_mode;
int has_been_changes;
int read_only;

int m;                              // multiplier
int asked_indent, asked_remove;
char dialog_chars[INTERFACE_WIDTH];

struct interface file_name_int;
struct interface settings_int;
struct interface range_int;
struct interface search_pattern;
struct interface replace_pattern;

struct selection *sel;              // selections queue
struct lang *syntax;
struct substring fields[10];
struct substring subpatterns[10];

int nb_line;                        // number of lines in file
struct line *first_line;
struct line *first_line_on_screen;
struct {
    struct line *start;
    int nb_line;
} clipboard;

struct pos anchor;
struct pos matching_bracket;
int is_bracket;
int y, x;                           // cursor position in file area
int screen_height, screen_width;    // terminal dimensions
struct tb_event ev;                 // struct to retrieve events
