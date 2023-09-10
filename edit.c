// See LICENSE file for copyright and license details.

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TB_IMPL
#include "termbox2.h"
#include "config.h"
#include "languages.h"


// MACROS

#define ERR_BUFFER_TOO_SMALL        "A buffer was too small."
#define ERR_FILE_IO                 "File connection error occurred."
#define ERR_MALLOC                  "Memory allocation error occurred."
#define ERR_TERM_TOO_SMALL          "Terminal was too small."
#define ERR_UTF8_ENCODING           "UTF-8 error occurred."

#define ACC_LETTER                  ((char) 0xc3)
#define DEFAULT_BUF_SIZE            INTERFACE_WIDTH
#define INTERFACE_MEM_LENGTH        (4*INTERFACE_WIDTH + 1)
#if LINE_NUMBERS_WIDTH < 1
#define LINE_NUMBERS_MODULUS        1
#elif LINE_NUMBERS_WIDTH == 2
#define LINE_NUMBERS_MODULUS        10
#elif LINE_NUMBERS_WIDTH == 3
#define LINE_NUMBERS_MODULUS        100
#else
#define LINE_NUMBERS_MODULUS        1000
#endif // LINE_NUMBERS_WIDTH
#define MIN_HEIGHT                  2

#define ABS(A)                      (((A) < 0) ? -(A) : (A))
#define ATTRIBUTE_X(X)              {x = (X); attribute_x = 1;}
#define CONSTRAIN(A, X, B)          (((X) < (A)) ? (A) : (((X) > (B)) ? (B) : (X)))
#define DECLARE_BRACKETS(A, B) \
    case (A): associated_bracket = (B); e = 1; break; \
    case (B): associated_bracket = (A); e = -1; break;
#define DECLARE_PARAMETER(PARAMETER, NAME, TYPE, VAR) \
    else if (sscanf(assign, NAME"=%"TYPE, &VAR) == 1) settings.PARAMETER = VAR;
#define INIT_INTERFACE(I, S)        strcpy(I.current, S); strcpy(I.previous, S);
#define MAX(A, B)                   (((A) > (B)) ? (A) : (B))
#define MIN(A, B)                   (((A) < (B)) ? (A) : (B))
#define MOVE_SEL_LIST(A, B)         {forget_sel_list(B); B = A; A = NULL;}
#define PARSE_LINE_IDENTIFIER(S, DEFAULT, OPERATOR, VAR) \
    if (!strcmp(S, "")) VAR = DEFAULT; \
    else if (!strcmp(S, ".")) VAR = first_line_nb + y; \
    else if (sscanf(S, "%d", &VAR) == 1) VAR = OPERATOR(VAR, DEFAULT); \
    else return EXIT_FAILURE;
#define FILE_IO(A, E)               if ((A) == E) die(EXIT_FAILURE, ERR_FILE_IO);
#define SET_SEL_LIST(A, B)          {forget_sel_list(A); A = B;}
#define SET_SUBSTRING(S, ST, MST, N, MN) \
    {(S).st = (ST); (S).mst = (MST); (S).n = (N); (S).mn = (MN);}
#define STORE_BUFFER(B, I, V)       if ((I) < nb_displayed) (B)[I] = V;
#define echo(MESSAGE)               strcpy(message, MESSAGE)
#define echof(PATTERN, INTEGER)     sprintf(message, PATTERN, INTEGER)
#define first_line_nb               (first_line_on_screen->line_nb)
#define is_type(TYPE, L)            is_in(lang->TYPE, l->chars + k, L)
#define pos_of_cursor()             pos_of(first_line_nb + y, x)
#define pos_of_sel(S)               pos_of((S).l, (S).x)
#define way(DIRECT_CONDITION)       ((DIRECT_CONDITION) ? m : -m)


// TYPES

typedef struct {                    // interface for dialog mode
    char current[INTERFACE_MEM_LENGTH], previous[INTERFACE_MEM_LENGTH];
} Interface;

typedef struct Line {               // doubly linked list of lines
    struct Line *prev;
    struct Line *next;
    int line_nb;                    // between 1 and nb_lines
    int ml, dl;                     // length in memory, on screen
    char *chars;                    // content (UTF-8, NULL-ended string)
} Line;

typedef struct {                    // position in file
    int l, x;                       // line number, column
} Pos;

typedef struct Selection {          // sorted list of non-overlapping selections
    int l, x, n;                    // line number, column, number of characters
    struct Selection *next;
} Selection;

typedef struct {                    // marks a substring in an original string
    int st, mst;                    // starting position (characters, bytes)
    int n, mn;                      // length (characters, bytes)
} Substring;


// FUNCTIONS DECLARATIONS

static void act(void (*process)(Line *, Selection *), int line_op);
static void autocomplete(Line *l, Selection *s);
static void break_line(Line *l, Selection *s, int start);
static int column_sel(int m);
static void comment(Line *l, Selection *s);
static int compare_chars(const char *s1, int k1, const char *s2, int k2);
static Selection *compute_running_sel(void);
static void concatenate_line(Line *l, Selection *s);
static void copy_to_clip(int starting_line_nb, int nb);
static Line *create_line(int line_nb, int ml, int dl);
static Selection *create_sel(int l, int x, int n, Selection *next);
static void decrement(const char *chars, int *i, int *k, int goal);
static void die(int exit_status, const char *msg);
static int dialog(const char *prompt, Interface *interf, int refresh_sel);
static int eat_pattern_atom(const char *sp, int *j, int *l);
static int eat_pattern_block(const char *sp, int *j, int *l);
static int eat_pattern_character(const char *sp, int *j, int *l);
static int find_block_delim(int starting_line_nb, int nb);
static int find_first_non_blank(const Line *l);
static Pos find_matching_bracket(void);
static Pos find_next_selection(int delta);
static Pos find_start_of_word(int n);
static void forget_lines(Line *start);
static void forget_sel_list(Selection *a);
static Line *get_line(int delta_from_first_line_on_screen);
static Pos get_pos_of_sel(Selection *a, int index);
static int get_str_index(const char *chars, int x);
static void indent(Line *l, Selection *s);
static int index_closest_after_cursor(Selection *a);
static void init_termbox(void);
static void insert(Line *l, Selection *s);
static void insert_clip(Line *starting_line, int below);
static void insert_line(int line_nb, int ml, int dl);
static int is_in(const char *list, const char *chars, int length);
static int is_inf(Pos p1, Pos p2);
static int is_word_boundary(const char *chars, int k);
static int is_word_char(char c);
static void link_lines(Line *l1, Line *l2);
static void load_file(int first_line_on_screen_nb);
static void lower(Line *l, Selection *s);
static int mark_subpatterns(const char *sp, const char *chars, int dl, int ss, int sx, int n);
static int mark_fields(const char *chars, int sx, int n);
static Selection *merge_sel(Selection *a, Selection *b);
static int move(Line **l, int *dx, int e);
static void move_line(int delta);
static void move_sel_end_of_line(Selection *a, int l, int dl, int e);
static void move_to_clip(int starting_line_nb, int nb);
static void move_to_cursor(void);
static int nb_sel(Selection *a);
static int parse_assign(const char *assign);
static int parse_lang(const char *file_name);
static int parse_range(const char *range);
static int parse_repeater(const char *sp, int *j, int *l, int *min, int *max);
static Pos pos_of(int l, int x);
static void print_all(void);
static void print_dialog_ruler(void);
static Selection *print_line(const Line *l, Selection *s, int screen_line);
static Selection *range_lines_sel(int start, int end, Selection *next);
static void remove_sel_line_range(int min, int max);
static void reorder_sel(int l, int nb, int new_l);
static void replace(Line *l, Selection *s);
static int replace_chars(Line *l, Selection *a, int start, int n, int new_n, int nb_bytes);
static void reset_selections(void);
static int resize(int width, int height);
static Selection *search(Selection *a);
static int search_word_under_cursor(void);
static void shift_line_nb(Line *l, int min, int max, int delta);
static void shift_sel_line_nb(Selection *a, int min, int max, int delta);
static void split(Line *l, Selection *s);
static void suppress(Line *l, Selection *s);
static void unwrap_pos(Pos p);
static void upper(Line *l, Selection *s);
static void write_file(const char *file_name);
static void *emalloc(size_t size);


// VARIABLES

static Interface file_name_int, replace_pattern, search_pattern;
static Line *first_line, *first_line_on_screen;
static Pos anchor, matching_bracket;
static Selection *saved, *running, *displayed;
static Substring fields[10], subpatterns[10];
static char message[INTERFACE_MEM_LENGTH];
static int nb_lines, y, x;
static int anchored, in_insert_mode, attribute_x, is_bracket;
static int has_been_changes, has_been_invalid_resizing;
static int asked_indent, asked_remove;
static int screen_height, screen_width, scroll_offset;
static uint32_t *ch, to_insert;
static uintattr_t *fg, *bg;
static struct {
    Line *start;
    int nb_lines;
} clipboard;
static const struct lang *lang;
static struct {
    int case_sensitive;
    char field_separator;
    int highlight_selections;
    int syntax_highlight;
    int tab_width;
} settings = {
    CASE_SENSITIVE,
    FIELD_SEPARATOR,
    HIGHLIGHT_SELECTIONS,
    SYNTAX_HIGHLIGHT,
    TAB_WIDTH,
};


// FUNCTION IMPLEMENTATIONS

void
act(void (*process)(Line *, Selection *), int line_op)
{
    Line *l;
    Selection *s;
    int old_line_nb;

    s = (saved) ? saved : running;
    l = get_line(s->l - first_line_nb);
    old_line_nb = 0;
    has_been_changes = 1;
    for (; s; s = s->next) {
        for (; l->line_nb < s->l; l = l->next);
        if (!line_op || s->l > old_line_nb)
            process(l, s);
        old_line_nb = s->l;
    }
}

#ifdef ENABLE_AUTOCOMPLETE
void
autocomplete(Line *l, Selection *s)
{
    Line *sl;
    char *match, *tmp;
    int k, k1, k2, it, kt, ms, dl, ml, max_ml;

    k2 = kt = get_str_index(l->chars, it = s->x + s->n);
    while ((kt == k2 || is_word_char(l->chars[kt])) && kt > 0)
        decrement(l->chars, &it, &kt, it - 1);
    k1 = (kt == k2 || is_word_char(l->chars[kt])) ? 0 : (kt + tb_utf8_char_length(l->chars[kt]));
    if (k1 == k2)
        return;
    match = NULL;
    ms = dl = ml = max_ml = 0;
    for (sl = first_line, k = 0; sl;) {
        if (is_word_char(sl->chars[k]) && is_word_boundary(sl->chars, k) &&
            (k + k2 - k1) < sl->ml && !strncmp(l->chars + k1, sl->chars + k, k2 - k1) &&
            is_word_char(sl->chars[k + k2 - k1])) {
            k += k2 - k1;
            for (ml = dl = 0; (!match) ? is_word_char(sl->chars[k + ml]) :
                (ml < max_ml && !compare_chars(match, ms + ml, sl->chars, k + ml));
                dl++, ml += tb_utf8_char_length(sl->chars[k + ml]));
            max_ml = ml;
            if (!match) {
                match = sl->chars;
                ms = k;
            }
        }
        if (sl->chars[k])
            k += tb_utf8_char_length(sl->chars[k]);
        else {
            sl = sl->next;
            k = 0;
        }
    }
    if (match) {
        tmp = (char *) emalloc(ml);
        strncpy(tmp, match + ms, ml);
        replace_chars(l, s, s->x + s->n, 0, dl, ml);
        strncpy(l->chars + k2, tmp, ml);
        free(tmp);
    }
}
#endif // ENABLE_AUTOCOMPLETE

void
break_line(Line *l, Selection *s, int start)
{
    Line *new;
    char *new_chars;
    int k;

    shift_sel_line_nb(s, l->line_nb + 1, 0, 1);
    move_sel_end_of_line(s, l->line_nb, start, -1);
    shift_line_nb(l, l->line_nb + 1, 0, 1);
    k = get_str_index(l->chars, start);
    new = create_line(l->line_nb + 1, l->ml - k, l->dl - start);
    strncpy(new->chars, l->chars + k, l->ml - k);
    new_chars = (char *) emalloc(k + 1);
    new_chars[k] = '\0';
    strncpy(new_chars, l->chars, k);
    free(l->chars);
    l->chars = new_chars;
    l->ml = k + 1;
    l->dl = start;
    link_lines(new, l->next);
    link_lines(l, new);
    nb_lines++;
}

int
column_sel(int n)
{
    Line *l;
    Selection *last, *tmp;
    Pos cursor;
    int i, delta, wx, wn;

    cursor = pos_of_cursor();
    if (anchored && anchor.l != cursor.l)
        return 0;
    n = MIN(n, nb_lines - cursor.l + 1);
    if (anchored) {
        delta = cursor.x - anchor.x;
        wx = (delta > 0) ? anchor.x : cursor.x;
        wn = (delta > 0) ? delta : -delta;
    } else {
        wx = x;
        wn = 0;
    }
    last = NULL;
    for (i = 0, l = get_line(y + n - 1); i < n; i++, l = l->prev)
        if (l->dl >= wx)
            last = create_sel(l->line_nb, wx, MIN(wn, l->dl - wx), last);
    tmp = merge_sel(saved, last);
    SET_SEL_LIST(saved, tmp)
    if (anchored) {
        if (cursor.l + n > nb_lines || anchor.x > get_line(y + n)->dl)
            anchored = 0;
        else
            anchor.l = cursor.l + n;
    }
    return n;
}

void
comment(Line *l, Selection *s)
{
    int k, syntax_length = strlen(lang->comment);

    if (!(l->chars[k = find_first_non_blank(l)]))
        return;
    if (is_type(comment, syntax_length - 1))
        replace_chars(l, s, k, syntax_length, 0, 0);
    else {
        k = replace_chars(l, s, k, 0, syntax_length, syntax_length);
        strncpy(l->chars + k, lang->comment, syntax_length);
    }
}

int
compare_chars(const char *s1, int k1, const char *s2, int k2)
{
    int k, l1, l2, delta;

    if ((l1 = tb_utf8_char_length(s1[k1])) != (l2 = tb_utf8_char_length(s2[k2])))
        return l2 - l1;
    else
        for (k = 0; k < l1; k++)
            if ((delta = s2[k2 + k] - s1[k1 + k])) {
                if (!settings.case_sensitive && k == l1 - 1 &&
                    (delta == (1 << 5) || delta == - (1 << 5)))
                    return 0;
                else
                    return delta;
            }
    return 0;
}

Selection *
compute_running_sel(void)
{
    Selection *medium_sel, *end_sel;
    Pos cursor, begin, end;

    cursor = pos_of_cursor();
    if (anchored) {
        if (cursor.l != anchor.l) {
            begin = (cursor.l > anchor.l) ? anchor : cursor;
            end = (cursor.l > anchor.l) ? cursor : anchor;
            end_sel = create_sel(end.l, 0, end.x, NULL);
            medium_sel = (begin.l + 1 > end.l - 1) ? end_sel :
                range_lines_sel(begin.l + 1, end.l - 1, end_sel);
            return create_sel(begin.l, begin.x,
                get_line(begin.l - first_line_nb)->dl - begin.x, medium_sel);
        } else
            return create_sel(cursor.l, MIN(cursor.x, anchor.x), ABS(cursor.x - anchor.x), NULL);
    } else
        return create_sel(cursor.l, cursor.x, 0, NULL);
}

void
concatenate_line(Line *l, Selection *s)
{
    Line *next;
    char *new_chars;

    move_sel_end_of_line(s, l->line_nb + 1, l->dl, 1);
    shift_sel_line_nb(s, l->line_nb + 1, 0, -1);
    shift_line_nb(l, l->line_nb + 1, 0, -1);
    next = l->next;
    new_chars = (char *) emalloc(l->ml + next->ml - 1);
    strncpy(new_chars, l->chars, l->ml - 1);
    free(l->chars);
    strncpy(new_chars + l->ml - 1, next->chars, next->ml);
    free(next->chars);
    l->chars = new_chars;
    l->ml += next->ml - 1;
    l->dl += next->dl;
    link_lines(l, next->next);
    free(next);
    nb_lines--;
}

void
copy_to_clip(int starting_line_nb, int nb)
{
    Line *l, *cb_l, *old_cb_l;
    int i;

    nb = MIN(nb, nb_lines - starting_line_nb + 1);
    forget_lines(clipboard.start);
    clipboard.nb_lines = nb;
    cb_l = old_cb_l = NULL;
    for (i = 0, l = get_line(starting_line_nb - first_line_nb); i < nb; i++, l = l->next) {
        cb_l = create_line(i, l->ml, l->dl);
        strncpy(cb_l->chars, l->chars, l->ml);
        link_lines(old_cb_l, cb_l);
        if (i == 0)
            clipboard.start = cb_l;
        old_cb_l = cb_l;
    }
    link_lines(cb_l, NULL);
}

Line *
create_line(int line_nb, int ml, int dl)
{
    Line *res;

    res = (Line *) emalloc(sizeof(Line));
    res->prev = res->next = NULL;
    res->line_nb = line_nb;
    res->ml = ml;
    res->dl = dl;
    res->chars = (char *) emalloc(ml);
    res->chars[ml - 1] = '\0';
    return res;
}

Selection *
create_sel(int l, int x, int n, Selection *next)
{
    Selection *res;

    res = (Selection *) emalloc(sizeof(Selection));
    res->l = l;
    res->x = x;
    res->n = n;
    res->next = next;
    return res;
}

void
decrement(const char *chars, int *i, int *k, int goal)
{
    while (*i > goal)
        for ((*i)--, (*k)--; (chars[*k] & 0xc0) == 0x80; (*k)--);
}

void
die(int exit_status, const char *msg)
{
    tb_shutdown();
    if (msg)
        fprintf((exit_status == EXIT_SUCCESS) ? stdout : stderr, "%s\n", msg);
    exit(exit_status);
}

int
dialog(const char *prompt, Interface *interf, int refresh_sel)
{
    char unicode_buffer[6];
    int dpl, dx, i, k, n, len, j;
    struct tb_event ev;

    strcpy(interf->current, "");
    for (k = i = 0; prompt[k]; i++, k += tb_utf8_char_length(prompt[k]));
    dpl = i;
    dx = n = 0;
    while (1) {
        if (refresh_sel) {
            SET_SEL_LIST(displayed, search(saved))
            print_all();
        }
        strcpy(message, prompt);
        strcat(message, interf->current);
        print_dialog_ruler();
        tb_set_cursor(dx + dpl, screen_height - 1);
        tb_present();
        tb_poll_event(&ev);
        switch (ev.type) {
        case TB_EVENT_KEY:
            if (ev.ch && dpl + n + 1 < MIN(INTERFACE_WIDTH, screen_width - RULER_WIDTH)) {
                k = get_str_index(interf->current, i = dx);
                len = tb_utf8_unicode_to_char(unicode_buffer, ev.ch);
                for (j = strlen(interf->current); j >= k; j--) // copy NULL
                    interf->current[j + len] = interf->current[j];
                strncpy(interf->current + k, unicode_buffer, len);
                dx++;
                n++;
            } else {
                switch (ev.key) {
                case TB_KEY_ESC:
                    strcpy(interf->current, interf->previous);
                    strcpy(message, "");
                    return 0;
                case TB_KEY_ENTER:
                    strcpy(interf->previous, interf->current);
                    return 1;
                case TB_KEY_BACKSPACE:
                case TB_KEY_BACKSPACE2:
                    if (dx > 0) {
                        k = get_str_index(interf->current, dx - 1);
                        len = tb_utf8_char_length(interf->current[k]);
                        for (; (interf->current[k] = interf->current[k + len]); k++);
                        dx--;
                        n--;
                    }
                    break;
                case TB_KEY_ARROW_RIGHT:
                    if (dx < n)
                        dx++;
                    break;
                case TB_KEY_ARROW_LEFT:
                    if (dx > 0)
                        dx--;
                    break;
                case TB_KEY_ARROW_UP:
                case TB_KEY_ARROW_DOWN:
                    strcpy(interf->current, (ev.key == TB_KEY_ARROW_UP) ? interf->previous : "");
                    for (k = i = 0; interf->current[k]; i++,
                        k += tb_utf8_char_length(interf->current[k]));
                    dx = n = i;
                    break;
                }
            }
            break;
#ifdef MOUSE_SUPPORT
        case TB_EVENT_MOUSE:
            switch (ev.key) {
            case TB_KEY_MOUSE_LEFT:
                dx = CONSTRAIN(0, ev.x - dpl, n);
                break;
            }
            break;
#endif // MOUSE_SUPPORT
        case TB_EVENT_RESIZE:
            if ((has_been_invalid_resizing = resize(ev.w, ev.h)))
                return 0;
            print_all();
            break;
        }
    }
}

int
eat_pattern_atom(const char *sp, int *j, int *l)
{
    int min, max;

    if (sp[*l] == '^' || sp[*l] == '$') {
        (*j)++; (*l)++;
    } else if (sp[*l] == '\\' && strchr("AZbB", sp[*l + 1])) {
        (*j) += 2; (*l) += 2;
    } else {
        if (eat_pattern_character(sp, j, l))
            return EXIT_FAILURE;
        if (parse_repeater(sp, j, l, &min, &max))
            return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int
eat_pattern_block(const char *sp, int *j, int *l)
{
    int min, max;

    if (sp[*l] == '(') {
        while (sp[*l] != ')') {
            if (!(sp[*l]) || eat_pattern_atom(sp, j, l))
                return EXIT_FAILURE;
            while (sp[*l] == '|') {
                (*j)++; (*l)++;
                if (!(sp[*l]) || eat_pattern_atom(sp, j, l))
                    return EXIT_FAILURE;
            }
        }
        (*j)++; (*l)++;
        return parse_repeater(sp, j, l, &min, &max);
    } else
        return eat_pattern_atom(sp, j, l);
}

int
eat_pattern_character(const char *sp, int *j, int *l)
{
    if (sp[*l] == '\\' && strchr("\\^$|()*+?{[.dDwW", sp[*l + 1])) {
        (*j) += 2; (*l) += 2;
    } else if (sp[*l] == '[') {
        for (; sp[*l] && sp[*l] != ']'; (*j)++, (*l) += tb_utf8_char_length(sp[*l]));
        if (!(sp[*l]))
            return EXIT_FAILURE;
        (*j)++; (*l)++;
    } else {
        (*j)++; (*l) += tb_utf8_char_length(sp[*l]);
    }
    return EXIT_SUCCESS;
}

int
find_block_delim(int starting_line_nb, int nb)
{
    Line *l;

    l = get_line(starting_line_nb - first_line_nb);
    if (nb < 0)
        while (nb++) {
            for (; l->prev && !(l->dl); l = l->prev);
            for (; l->prev && l->dl; l = l->prev);
        }
    else
        while (nb--) {
            for (; l->next && !(l->dl); l = l->next);
            for (; l->next && l->dl; l = l->next);
        }
    return l->line_nb + ((nb == 1 && (l->prev || !(l->dl))) ? 1 : 0);
}

int
find_first_non_blank(const Line *l)
{
    int i;

    for (i = 0; l->chars[i] == ' '; i++);
    return i;
}

Pos
find_matching_bracket(void)
{
    Line *l;
    char bracket, associated_bracket, c;
    int dx, e, nb;

    l = get_line(y);
    dx = x;
    nb = 1;
    switch (bracket = l->chars[get_str_index(l->chars, dx)]) {
    DECLARE_BRACKETS('(', ')')
    DECLARE_BRACKETS('{', '}')
    DECLARE_BRACKETS('[', ']')
    DECLARE_BRACKETS('<', '>')
    default:
        return pos_of(l->line_nb, x);
    }
    while (nb && !move(&l, &dx, e)) {
        c = l->chars[get_str_index(l->chars, dx)];
        if (c == bracket)
            nb++;
        else if (c == associated_bracket)
            nb--;
    }
    if (nb)
        is_bracket = 0;
    return pos_of(l->line_nb, dx);
}

Pos
find_next_selection(int delta)
{
    int nb, closest, asked_number, last_strictly_before;

    if (!(nb = nb_sel(saved)))
        return pos_of(0, 0);
    closest = index_closest_after_cursor(saved);
    if (delta > 0) {
        if (closest == -1)
            return pos_of(0, 0);
        asked_number = MIN(closest + delta - 1, nb - 1);
    } else {
        if (closest == 0)
            return pos_of(0, 0);
        last_strictly_before = ((closest == -1) ? nb : closest) - 1;
        if (!is_inf(get_pos_of_sel(saved, last_strictly_before), pos_of_cursor()))
            last_strictly_before--;
        if (last_strictly_before < 0)
            return pos_of(0, 0);
        asked_number = MAX(last_strictly_before + delta + 1, 0);
    }
    return get_pos_of_sel(saved, asked_number);
}

Pos
find_start_of_word(int n)
{
    Line *l;
    int dx, e;

    l = get_line(y);
    dx = x;
    e = (n > 0) ? 1 : -1;
    n *= e;
    while (n--) {
        while (!move(&l, &dx, e) && is_word_char(l->chars[get_str_index(l->chars, dx)]));
        while (!move(&l, &dx, e) && !is_word_char(l->chars[get_str_index(l->chars, dx)]));
    }
    if (e == -1)
        while (!move(&l, &dx, e))
            if (!is_word_char(l->chars[get_str_index(l->chars, dx)])) {
                move(&l, &dx, -e);
                break;
            }
    return pos_of(l->line_nb, dx);
}

void
forget_lines(Line *start)
{
    Line *l, *next;

    for (l = start; l; next = l->next, free(l->chars), free(l), l = next);
}

void
forget_sel_list(Selection *a)
{
    Selection *next;

    for (; a; next = a->next, free(a), a = next);
}

Line *
get_line(int delta_from_first_line_on_screen)
{
    Line *res;

    res = first_line_on_screen;
    if (delta_from_first_line_on_screen > 0)
        for (; res->next && delta_from_first_line_on_screen--; res = res->next);
    else
        for (; res->prev && delta_from_first_line_on_screen++; res = res->prev);
    return res;
}

Pos
get_pos_of_sel(Selection *a, int index)
{
    int i;

    for (i = 0; a && i < index; i++, a = a->next);
    return (a) ? pos_of(a->l, a->x) : pos_of(0, 0);
}

int
get_str_index(const char *chars, int x)
{
    int i, k;

    for (i = k = 0; i < x; i++, k += tb_utf8_char_length(chars[k]));
    return k;
}

void
indent(Line *l, Selection *s)
{
    int k, start, n;

    if (!in_insert_mode && l->dl == 0)
        return;
    start = (in_insert_mode) ? s->x : find_first_non_blank(l);
    if (asked_indent > 0) {
        n = asked_indent*(settings.tab_width) - start%(settings.tab_width);
        k = replace_chars(l, s, start, 0, n, n);
        memset(l->chars + k, ' ', n);
    } else {
        k = get_str_index(l->chars, start);
        for (n = 0; n < k && n < (-asked_indent - 1)*(settings.tab_width) + 1 +
            (start - 1)%(settings.tab_width) && l->chars[k - n - 1] == ' '; n++);
        replace_chars(l, s, start - n, n, 0, 0);
    }
}

int
index_closest_after_cursor(Selection *a)
{
    Pos cursor;
    int res;

    cursor = pos_of_cursor();
    for (res = 0; a; res++, a = a->next)
        if (is_inf(cursor, pos_of_sel(*a)))
            return res;
    return -1;
}

void
init_termbox(void)
{
    tb_init();
    tb_set_output_mode(OUTPUT_MODE);
    tb_set_clear_attrs(COLOR_DEFAULT, COLOR_BG_DEFAULT);
#ifdef MOUSE_SUPPORT
    tb_set_input_mode(TB_INPUT_ESC | TB_INPUT_MOUSE);
#else
    tb_set_input_mode(TB_INPUT_ESC);
#endif // MOUSE_SUPPORT
    has_been_invalid_resizing = resize(tb_width(), tb_height());
}

void
insert(Line *l, Selection *s)
{
    char unicode_buffer[6];
    int len, k;

    len = tb_utf8_unicode_to_char(unicode_buffer, to_insert);
    k = replace_chars(l, s, s->x, 0, 1, len);
    strncpy(l->chars + k, unicode_buffer, len);
}

void
insert_clip(Line *starting_line, int below)
{
    Line *l, *before, *after;
    int i, first_inserted_line_nb;

    if (!(clipboard.start))
        return;
    first_inserted_line_nb = starting_line->line_nb + ((below) ? 1 : 0);
    shift_line_nb(clipboard.start, 0, 0, first_inserted_line_nb);
    shift_line_nb(starting_line, first_inserted_line_nb, 0, clipboard.nb_lines);
    shift_sel_line_nb(saved, first_inserted_line_nb, 0, clipboard.nb_lines);
    for (i = 1, l = clipboard.start; i < clipboard.nb_lines; i++, l = l->next);
    before = (below) ? starting_line : starting_line->prev;
    after = (below) ? starting_line->next : starting_line;
    link_lines(before, clipboard.start);
    link_lines(l, after);
    if (!before)
        first_line = clipboard.start;
    nb_lines += clipboard.nb_lines;
    has_been_changes = 1;
    clipboard.start = NULL;
    copy_to_clip(first_inserted_line_nb, clipboard.nb_lines);
    if (!below) {
        if (y == 0)
            first_line_on_screen = get_line(-clipboard.nb_lines);
        y += clipboard.nb_lines;
    }
}

void
insert_line(int line_nb, int ml, int dl)
{
    Line *replaced_line, *new;

    new = create_line(line_nb, ml, dl);
    if (line_nb == nb_lines + 1) {
        link_lines(get_line(nb_lines - first_line_nb), new);
        link_lines(new, NULL);
    } else {
        replaced_line = get_line(line_nb - first_line_nb);
        shift_line_nb(replaced_line, line_nb, 0, 1);
        shift_sel_line_nb(saved, line_nb, 0, 1);
        link_lines(replaced_line->prev, new);
        link_lines(new, replaced_line);
        if (!(new->prev))
            first_line = new;
        if (replaced_line == first_line_on_screen)
            first_line_on_screen = new;
    }
    nb_lines++;
    has_been_changes = 1;
}

int
is_in(const char *list, const char *chars, int length)
{
    int len;

    for (; *list; list += len + 1) {
        if ((len = strchr(list, ' ') - list) != length)
            continue;
        if (!strncmp(list, chars, len))
            return 1;
    }
    return 0;
}

int
is_inf(Pos p1, Pos p2)
{
    return (p1.l < p2.l || (p1.l == p2.l && p1.x < p2.x));
}

int
is_word_boundary(const char *chars, int k)
{
    int i, is_word;

    is_word = is_word_char(chars[k]);
    if (k == 0)
        return is_word;
    else {
        i = 1;
        decrement(chars, &i, &k, i - 1);
        return (is_word != is_word_char(chars[k]));
    }
}

int
is_word_char(char c)
{
    return (isalpha(c) || c == '_' || c == ACC_LETTER);
}

void
link_lines(Line *l1, Line *l2)
{
    if (l1)
        l1->next = l2;
    if (l2)
        l2->prev = l1;
}

void
load_file(int first_line_on_screen_nb)
{
    Line *line, *last_line;
    FILE *src_file;
    char *buf, *new_buf;
    int reached_EOF, buf_size, line_nb, c, ml, dl, l, k;

    forget_lines(first_line);
    reset_selections();
    if (!(src_file = fopen(file_name_int.current, "r"))) {
        first_line = first_line_on_screen = create_line(1, 1, 0);
        nb_lines = 1;
        has_been_changes = 1;
        parse_lang(file_name_int.current);
        return;
    }
    line_nb = 1;
    first_line = last_line = first_line_on_screen = NULL;
    buf = (char *) emalloc(buf_size = DEFAULT_BUF_SIZE);
    reached_EOF = 0;
    while (!reached_EOF) {
        ml = dl = 0;
        while (1) {
            reached_EOF = ((c = getc(src_file)) == EOF);
            if (c == EOF || c == '\n')
                break;
            if (c == '\t')
                for (l = 1; (dl + l)%(settings.tab_width); l++);
            else
                l = tb_utf8_char_length(c);
            if (ml + l > buf_size) {
                while (ml + l > buf_size)
                    buf_size <<= 1;
                new_buf = (char *) emalloc(buf_size);
                strncpy(new_buf, buf, ml);
                free(buf);
                buf = new_buf;
            }
            if (c == '\t') {
                memset(buf + ml, ' ', l);
                ml += l; dl += l;
            } else {
                buf[ml] = (char) c;
                for (k = 1; k < l; k++) {
                    if (((c = getc(src_file)) == EOF) ||
                        (((char) c & (char) 0xc0) != (char) 0x80))
                        die(EXIT_FAILURE, ERR_UTF8_ENCODING);
                    buf[ml + k] = (char) c;
                }
                ml += l; dl++;
            }
        }
        if (reached_EOF && ml == 0 && line_nb > 1)
            break;
        line = create_line(line_nb, ml + 1, dl);
        if (first_line) {
            link_lines(last_line, line);
            last_line = line;
        } else
            first_line = last_line = line;
        strncpy(line->chars, buf, ml);
        if (line_nb == first_line_on_screen_nb)
            first_line_on_screen = line;
        line_nb++;
    }
    FILE_IO(fclose(src_file), EOF)
    free(buf);
    if (!first_line_on_screen)
        first_line_on_screen = last_line;
    last_line->next = NULL;
    nb_lines = line_nb - 1;
    has_been_changes = 0;
    parse_lang(file_name_int.current);
}

void
lower(Line *l, Selection *s)
{
    char c;
    int i, k;

    for (k = get_str_index(l->chars, i = s->x); i < s->x + s->n; i++, k += tb_utf8_char_length(c))
        if (isupper(c = l->chars[k]) || c == ACC_LETTER)
            l->chars[k + ((c == ACC_LETTER) ? 1 : 0)] |= (1 << 5);
}

int
mark_subpatterns(const char *sp, const char *chars, int dl, int ss, int sx, int n)
{
    // try to read searched pattern sp in chars, store identified subpatterns
    // dl must be the visual length of chars, and ss the real selection start
    // return length of read pattern if found at sx, of length < n, else 0

    int j, l, lsp;      // indexes (characters, bytes), memory length of sp
    int i, k;           // indexes (characters, bytes) of chars
    int s, a;           // number of subpatterns, generic

    //         AUTOMATON            RELEVANT VARIABLES AT THIS LEVEL OR LOWER
    // IN -> READ_PATTERN -> OUT    (j, l), (i, k), state
    //         |       ^
    //         V       |
    // READ_BLOCK --> BLOCK_READ    start_block_i, is_block_ok
    //   |     |       ^     ^
    //   |     |       |     |
    //   |     | GROUP_READ  |      start_group_{j,i}, is_group_ok, nb_group
    //   |     |   |   ^     |
    //   |     V   V   |     |
    //   |    READ_GROUP     |
    //   |     |       ^     |
    //   V     V       |     |
    //  READ_ATOM --> ATOM_READ     in_group, start_atom_i, is_atom_ok
    //      |             ^
    //      V             |
    //  READ_CHAR <-> CHAR_READ     start_char_{j,i}, is_char_ok, nb_char

    enum states { READ_PATTERN, READ_BLOCK, BLOCK_READ, READ_GROUP, GROUP_READ,
        READ_ATOM, ATOM_READ, READ_CHAR, CHAR_READ };
    int state;

    // ORed elements
    int is_block_ok, start_block_i;
    int is_atom_ok, start_atom_i, in_group;

    // repeated elements
    int is_group_ok, nb_group, start_group_j, start_group_i;
    int is_char_ok, nb_char, start_char_j, start_char_i, found, is_neg_class;
    int min, max;

    // init variables
    state = READ_PATTERN;
    lsp = strlen(sp);
    l = j = 0;
    s = 0;

    // init subpatterns
    SET_SUBSTRING(subpatterns[0], i = sx, k = get_str_index(chars, i), n,
        get_str_index(chars + k, n) - k);
    for (a = 1; a < 10; a++)
        SET_SUBSTRING(subpatterns[a], 0, 0, 0, 0);

    // try to read the pattern
    while (1)
    switch (state) {
    case READ_PATTERN:
        if (l == lsp)
            return i - sx; // correct read, return read length
        start_block_i = i;
        state = READ_BLOCK;
        break;

    case READ_BLOCK:
        is_block_ok = 1;
        if (l == lsp)
            return 0; // invalid syntax
        if (sp[l] == '(') {
            j++; l++;
            s++;
            is_group_ok = 1;
            subpatterns[s].st = start_group_i = i;
            subpatterns[s].mst = k;
            start_group_j = j;
            nb_group = 0;
            state = READ_GROUP;
        } else {
            in_group = 0;
            start_atom_i = i;
            state = READ_ATOM;
        }
        break;

    case BLOCK_READ:
        if (is_block_ok) {
            while (sp[l] == '|') { // eat following ORed blocks
                j++; l++;
                if (sp[l] == '\0' || eat_pattern_block(sp, &j, &l))
                    return 0; // invalid syntax
            }
            state = READ_PATTERN;
        } else if (sp[l] == '|') { // another try
            j++; l++;
            decrement(chars, &i, &k, start_block_i);
            state = READ_BLOCK;
        } else
            return 0; // essential block is invalid
        break;

    case READ_GROUP:
        if (l == lsp)
            return 0; // invalid syntax
        if (sp[l] == ')') {
            j++; l++;
            nb_group++;
            state = GROUP_READ;
        } else {
            in_group = 1;
            start_atom_i = i;
            state = READ_ATOM;
        }
        break;

    case GROUP_READ:
        if (parse_repeater(sp, &j, &l, &min, &max)) // compute min and max
            return 0; // invalid syntax
        if (!is_group_ok) { // cancelling read
            if (nb_group - 1 < min)
                is_block_ok = 0;
            decrement(chars, &i, &k, start_group_i);
        } else if (!max || nb_group < max) { // another read
            is_group_ok = 1;
            start_group_i = i;
            decrement(sp, &j, &l, start_group_j);
            state = READ_GROUP;
            break;
        }
        subpatterns[s].n = i - subpatterns[s].st;
        subpatterns[s].mn = k - subpatterns[s].mst;
        state = BLOCK_READ;
        break;

    case READ_ATOM:
        // is_atom_ok must be attributed
        if (l == lsp)
            return 0; // invalid syntax
        if (sp[l] == '^' || sp[l] == '$') { // assertions
            is_atom_ok = (sp[l] == '^') ? (i == 0) : (i == dl);
            j++; l++;
        } else if (sp[l] == '\\' && (sp[l + 1] == 'A' || sp[l + 1] == 'Z')) {
            is_atom_ok = (sp[l + 1] == 'A') ? (i == ss) : (i == sx + n);
            j += 2; l += 2;
        } else if (sp[l] == '\\' && (sp[l + 1] == 'b' || sp[l + 1] == 'B')) {
            is_atom_ok = (sp[l + 1] == 'B') ^ is_word_boundary(chars, k);
            j += 2; l += 2;
        } else { // character
            is_atom_ok = 1;
            start_char_i = i;
            start_char_j = j;
            nb_char = 0;
            state = READ_CHAR;
            break;
        }
        state = ATOM_READ;
        break;

    case ATOM_READ:
        if (in_group) {
            if (is_atom_ok) {
                while (sp[l] == '|') { // eat followind ORed atoms
                    j++; l++;
                    if (sp[l] == '\0' || eat_pattern_atom(sp, &j, &l))
                        return 0; // invalid syntax
                }
            } else if (sp[l] == '|') { // another try
                j++; l++;
                decrement(chars, &i, &k, start_atom_i);
            } else {
                is_group_ok = 0;
            }
            state = READ_GROUP;
        } else {
            is_block_ok = is_atom_ok;
            state = BLOCK_READ;
        }
        break;

    case READ_CHAR:
        // is_char_ok must be attributed
        if (l == lsp)
            return 0; // invalid syntax
        if (i == sx + n) {
            is_char_ok = 0;
            eat_pattern_character(sp, &j, &l);
            state = CHAR_READ;
            break;
        } if (sp[l] == '\\') {
            if (strchr("\\^$|()*+?{[.", sp[l+1])) { // escaped character
                is_char_ok = (sp[l + 1] == chars[k]);
            } else if (sp[l + 1] == 'd' || sp[l + 1] == 'D') { // [non] digit
                is_char_ok = (sp[l + 1] == 'D') ^ isdigit(chars[k]);
            } else if (sp[l + 1] == 'w' || sp[l + 1] == 'W') { // [non] word
                is_char_ok = (sp[l + 1] == 'W') ^ is_word_char(chars[k]);
            } else {
                return 0; // invalid syntax
            }
            j += 2; l += 2;
        } else if (sp[l] == '[') { // custom class
            found = is_neg_class = 0;
            j++; l++;
            if (sp[l] == '^') {
                j++; l++;
                is_neg_class = 1;
            }
            while (sp[l] != '\0' && sp[l] != ']') {
                a = tb_utf8_char_length(sp[l]);
                if (l + a + 1 < lsp && sp[l + a] == '-' && sp[l + a + 1] != ']') { // range
                    if (compare_chars(sp, l, chars, k) >= 0 &&
                         compare_chars(sp, l + a + 1, chars, k) <= 0)
                        found = 1;
                    j += 3; l += a + 1 + tb_utf8_char_length(sp[l + a + 1]);
                } else { // raw comparison
                    if (!compare_chars(sp, l, chars, k))
                        found = 1;
                    j++; l += a;
                }
            }
            if (sp[l] == '\0')
                return 0; // invalid syntax
            is_char_ok = is_neg_class ^ found;
            j++; l++;
        } else { // any or regular character
            is_char_ok = (sp[l] == '.' || (!compare_chars(sp, l, chars, k) &&
                !strchr("\\^$|()*+?{[.", sp[l])));
            j++; l += tb_utf8_char_length(sp[l]);
        }
        i++; k += tb_utf8_char_length(chars[k]);
        nb_char++;
        state = CHAR_READ;
        break;

    case CHAR_READ:
        if (parse_repeater(sp, &j, &l, &min, &max))
            return 0; // invalid syntax
        if (!is_char_ok) { // cancelling read
            if (nb_char - 1 < min)
                is_atom_ok = 0;
            decrement(chars, &i, &k, start_char_i);
        } else if (!max || nb_char < max) { // another read
            start_char_i = i;
            decrement(sp, &j, &l, start_char_j);
            state = READ_CHAR;
            break;
        }
        state = ATOM_READ;
        break;
    }
}

int
mark_fields(const char *chars, int sx, int n)
{
    int f, a, st, mst, i, k;

    k = get_str_index(chars, i = sx);
    SET_SUBSTRING(fields[0], st = i, mst = k, n, get_str_index(chars + k, n) - k);
    for (a = 1; a < 10; a++)
        SET_SUBSTRING(fields[a], 0, 0, 0, 0);
    for (f = 1; f < 10 && i < sx + n; i++, k += tb_utf8_char_length(chars[k]))
        if ((chars[k] == settings.field_separator) && (k == 0 || chars[k - 1] != '\\')) {
            SET_SUBSTRING(fields[f], st, mst, i - st, k - mst);
            f++;
            st = i + 1;
            mst = k + 1;
        } else if (chars[k] == '\\') {
            i++; k++;
        }
    if (f < 10) {
        SET_SUBSTRING(fields[f], st, mst, i - st, k - mst);
        f++;
    }
    return f - 1;
}

Selection *
merge_sel(Selection *a, Selection *b)
{
    Selection *start, *last, *to_add, *new;

    start = last = NULL;
    while (a || b) {
        if (a && b) {
            if (is_inf(pos_of_sel(*a), pos_of_sel(*b))) {
                to_add = a;
                a = a->next;
            } else {
                to_add = b;
                b = b->next;
            }
        } else if (a) {
            to_add = a;
            a = a->next;
        } else {
            to_add = b;
            b = b->next;
        }
        new = create_sel(to_add->l, to_add->x, to_add->n, NULL);
        if (!last)
            start = last = new;
        else if (new->l == last->l && last->x + last->n > new->x)
            free(new); // covering detected
        else
            last = last->next = new;
    }
    return start;
}

int
move(Line **l, int *dx, int e)
{
    if (e > 0) {
        if (*dx + 1 <= (*l)->dl)
            (*dx)++;
        else if (!((*l)->next))
            return EXIT_FAILURE;
        else {
            *l = (*l)->next;
            *dx = 0;
        }
    } else {
        if (*dx > 0)
            (*dx)--;
        else if (!((*l)->prev))
            return EXIT_FAILURE;
        else {
            *l = (*l)->prev;
            *dx = (*l)->dl;
        }
    }
    return EXIT_SUCCESS;
}

void
move_line(int delta)
{
    Line *start_l, *end_l, *l;
    int start, end, nb, new_line_nb, initial_first_line_nb;

    initial_first_line_nb = first_line_nb;
    start = end = first_line_nb + y;
    if (anchored) {
        if (anchor.l < start)
            start = anchor.l;
        else
            end = anchor.l;
    }
    new_line_nb = CONSTRAIN(1, start + delta, nb_lines - (end - start));
    if (new_line_nb == start)
         return;
    reorder_sel(start, nb = end - start + 1, new_line_nb);
    start_l = get_line(start - first_line_nb);
    end_l = get_line(end - first_line_nb);
    if (delta > 0) {
        l = get_line(end + new_line_nb - start - first_line_nb);
        shift_line_nb(end_l, end + 1, end + new_line_nb - start, -nb);
        if (!(start_l->prev))
            first_line = end_l->next;
        link_lines(start_l->prev, end_l->next);
        link_lines(end_l, l->next);
        link_lines(l, start_l);
        shift_line_nb(start_l, start, end, new_line_nb - start);
    } else {
        l = get_line(new_line_nb - first_line_nb);
        shift_line_nb(start_l, start, end, new_line_nb - start);
        if (!(l->prev))
            first_line = start_l;
        link_lines(start_l->prev, end_l->next);
        link_lines(l->prev, start_l);
        link_lines(end_l, l);
        shift_line_nb(l, new_line_nb, start - 1, nb);
    }
    has_been_changes = 1;
    first_line_on_screen = get_line(initial_first_line_nb - first_line_nb);
    y += new_line_nb - start;
}

void
move_sel_end_of_line(Selection *a, int l, int dl, int e)
{
    for (; a && (a->l < l || (a->l == l && e == -1 && a->x < dl)); a = a->next);
    for (; a && a->l == l; a = a->next) {
        a->l -= 1*e;
        a->x += dl*e;
    }
    if (anchored && anchor.l == l && (e == 1 || anchor.x >= dl)) {
        anchor.l -= 1*e;
        anchor.x += dl*e;
    }
    if (first_line_nb + y == l) {
        y -= 1*e;
        x += dl*e; attribute_x = 1;
    } else if (first_line_nb + y > l)
        y -= 1*e;
}

void
move_to_clip(int starting_line_nb, int nb)
{
    Line *starting, *ending;

    nb = MIN(nb, nb_lines + 1 - starting_line_nb);
    starting = get_line(starting_line_nb - first_line_nb);
    ending = get_line(starting_line_nb + nb - 1 - first_line_nb);
    shift_line_nb(ending, ending->line_nb + 1, 0, -nb);
    nb_lines -= nb;
    if (starting_line_nb > first_line_nb)
        y = starting_line_nb - first_line_nb;
    else {
        if (!(ending->next))
            if (!(starting->prev)) {
                first_line = first_line_on_screen = create_line(1, 1, 0);
                nb_lines = 1;
            } else
                first_line_on_screen = starting->prev;
        else {
            first_line_on_screen = ending->next;
            if (!(starting->prev))
                first_line = first_line_on_screen;
        }
        y = 0;
    }
    link_lines(starting->prev, ending->next);
    link_lines(NULL, starting);
    link_lines(ending, NULL);
    shift_line_nb(starting, 0, 0, -starting->line_nb);
    forget_lines(clipboard.start);
    clipboard.start = starting;
    clipboard.nb_lines = nb;
    has_been_changes = 1;
    remove_sel_line_range(starting_line_nb, starting_line_nb + nb - 1);
    shift_sel_line_nb(saved, starting_line_nb + nb, 0, -nb);
    y = starting_line_nb - first_line_nb;
}

void
move_to_cursor(void)
{
    static int saved_x;
    int nl, max_x, delta;

    nl = CONSTRAIN(1, first_line_nb + y, nb_lines);
    if ((delta = nl - first_line_nb) < scroll_offset)
        y = MIN(scroll_offset, nl - 1);
    else
        y = MIN(delta, screen_height - 2 - scroll_offset);
    first_line_on_screen = get_line(delta - y);
#ifdef REMEMBER_CURSOR_COLUMN
    if (attribute_x)
        saved_x = x;
    x = saved_x;
#endif // REMEMBER_CURSOR_COLUMN
    attribute_x = 0;
    max_x = MIN(get_line(y)->dl, screen_width - 1 - LINE_NUMBERS_WIDTH);
    x = CONSTRAIN(0, x, max_x);
}

int
nb_sel(Selection *a)
{
    int res;

    for (res = 0; a; res++, a = a->next);
    return res;
}

int
parse_assign(const char *assign)
{
    char c, s[INTERFACE_MEM_LENGTH];
    int d;

    if (sscanf(assign, "l=%s", s) == 1)
        return parse_lang(s);
    DECLARE_PARAMETER(case_sensitive, "c", "d", d)
    DECLARE_PARAMETER(field_separator, "fs", "c", c)
    DECLARE_PARAMETER(highlight_selections, "h", "d", d)
    DECLARE_PARAMETER(syntax_highlight, "sh", "d", d)
    DECLARE_PARAMETER(tab_width, "tw", "d", d)
    else
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

int
parse_lang(const char *file_name)
{
    char *p;
    int k;

    k = (p = strrchr(file_name, '.')) ? (p - file_name + 1) : 0;
    for (lang = languages; lang->flags & DEFINED; lang++)
        if (is_in(lang->names, file_name + k, strlen(file_name) - k))
            return EXIT_SUCCESS;
    lang = NULL;
    return EXIT_FAILURE;
}

int
parse_range(const char *range)
{
    char *p;
    int l1, l2;

    if (!(p = strchr(range, ',')))
        return EXIT_FAILURE;
    *p = '\0';
    PARSE_LINE_IDENTIFIER(range, 1, MAX, l1)
    *p++ = ',';
    PARSE_LINE_IDENTIFIER(p, nb_lines, MIN, l2)
    if (l2 < l1)
        return EXIT_FAILURE;
    SET_SEL_LIST(saved, range_lines_sel(l1, l2, NULL))
    return EXIT_SUCCESS;
}

int
parse_repeater(const char *sp, int *j, int *l, int *min, int *max)
{
    char c;

    if ((c = sp[*l]) == '{') {
        *min = *max = 0;
        for ((*j)++, (*l)++; (c = sp[*l]) != '}' && c != ','; (*j)++, (*l)++) {
            if (!isdigit(c))
                return EXIT_FAILURE;
            *min = (*min)*10 + c - '0';
        }
        if (c == ',') {
            for ((*j)++, (*l)++; (c = sp[*l]) != '}'; (*j)++, (*l)++) {
                if (!isdigit(c))
                    return EXIT_FAILURE;
                *max = (*max)*10 + c - '0';
            }
        } else
            *max = *min;
        (*j)++; (*l)++;
    } else if (c == '*' || c == '+' || c == '?') {
        *min = (c == '+') ? 1 : 0;
        *max = (c == '?') ? 1 : 0;
        (*j)++; (*l)++;
    } else
        *min = *max = 1;
    return EXIT_SUCCESS;
}

Pos
pos_of(int l, int x)
{
    Pos res;

    res.l = l;
    res.x = x;
    return res;
}

void
print_all(void)
{
    Line *l;
    Selection *s;
    char c;
    int i;

#ifdef HIGHLIGHT_MATCHING_BRACKET
    l = get_line(y);
    c = l->chars[get_str_index(l->chars, x)];
    if ((is_bracket = (c == '{' || c == '}' || c == '[' || c == ']'
                    || c == '(' || c == ')' || c == '<' || c == '>')))
        matching_bracket = find_matching_bracket();
#endif // HIGHLIGHT_MATCHING_BRACKET
    tb_clear();
    s = displayed;
    for (i = 0, l = first_line_on_screen; l && i < screen_height - 1; i++, l = l->next)
        s = print_line(l, s, i);
    tb_set_cursor(x + LINE_NUMBERS_WIDTH, y);
    print_dialog_ruler();
}

void
print_dialog_ruler(void)
{
    int i, k, len;
    uint32_t unicode_char;

    for (i = k = 0; message[k]; i++, k += len) {
        len = tb_utf8_char_to_unicode(&unicode_char, message + k);
        tb_set_cell(i, screen_height - 1, unicode_char, COLOR_DIALOG, COLOR_BG_DEFAULT);
    }
    while (i < screen_width)
        tb_set_cell(i++, screen_height - 1, ' ', COLOR_DEFAULT, COLOR_BG_DEFAULT);
    tb_printf(screen_width - RULER_WIDTH, screen_height - 1,
        COLOR_RULER, COLOR_BG_DEFAULT, RULER_PATTERN, first_line_nb + y, x);
}

Selection *
print_line(const Line *l, Selection *s, int screen_line)
{
    char c, nc;
    int i, k;                       // indexes (characters, bytes) in l->chars
    int nb_displayed, j, dk, len, color, nb_to_color, underline;
    const struct rule *r;

    nb_displayed = MIN(l->dl, screen_width - LINE_NUMBERS_WIDTH);
#ifdef UNDERLINE_CURSOR_LINE
    underline = (screen_line == y) ? TB_UNDERLINE : 0;
#else
    underline = 0;
#endif // UNDERLINE_CURSOR_LINE

    // decompress UTF-8, initialise foreground and background
    for (i = k = 0; i < nb_displayed; i++, k += len) {
        len = tb_utf8_char_to_unicode(ch + i, l->chars + k);
        fg[i] = COLOR_DEFAULT;
        bg[i] = COLOR_BG_DEFAULT;
    }

    // foreground
    if (settings.syntax_highlight && lang) {
        // ignore blank characters at the beginning of the line
        i = k = find_first_non_blank(l);

        // detect a matching rule
        for (r = lang->rules; r->mark[0]; r++)
            if (!strncmp(l->chars + ((r->start_of_line) ? 0 : k), r->mark, strlen(r->mark)))
                break;

        // potential rule mark
        if (r->mark[0]) {
            if (r->start_of_line)
                i = 0;
            for (j = 0; j < strlen(r->mark); j++, i++)
                STORE_BUFFER(fg, i, r->color_mark)
            k = i;
        }

        if (!(lang->flags & ONLY_RULES)) {
            while (i < nb_displayed) {
                // word
                if (is_word_char(c = l->chars[k])) {
                    for (j = dk = 0; is_word_char(nc = l->chars[k + dk]) ||
                        isdigit(nc); j++, dk += tb_utf8_char_length(nc))
                        ;
                    color = COLOR_DEFAULT;
                    if (j != dk) {
                        // non-ASCII character, do not look for match
                    } else if (is_type(keywords, j))
                        color = COLOR_KEYWORD;
                    else if (is_type(flow_control, j))
                        color = COLOR_FLOW_CONTROL;
                    else if (is_type(built_ins, j))
                        color = COLOR_BUILT_IN;
                    nb_to_color = j;
                    k += dk;

                // number
                } else if ((j = mark_subpatterns(
                    "(0b[01]+)|(0x[0-9a-fA-F]+)|(-?\\d+\\.?\\d*)|(-?\\.\\d+)",
                    l->chars, l->dl, i, i, l->dl - i + 1))) {
                    color = COLOR_NUMBER;
                    nb_to_color = j;
                    k += j;

                // string
                } else if (c == '"' || c == '\'') {
                    k++; j = 1;
                    while (1) {
                        if (l->chars[k] == c) {
                            k += tb_utf8_char_length(l->chars[k]); j++;
                            break;
                        } else if (l->chars[k] == '\\') {
                            k++; j++;
                        }
                        if (l->chars[k] == '\0')
                            break;
                        k += tb_utf8_char_length(l->chars[k]); j++;
                    }
                    color = COLOR_STRING;
                    nb_to_color = j;

                // comment
                } else if (is_type(comment, strlen(lang->comment) - 1)) {
                    color = COLOR_COMMENT;
                    nb_to_color = l->dl - i;
                    k = l->ml;

                // something else
                } else {
                    color = COLOR_DEFAULT;
                    nb_to_color = 1;
                    k += tb_utf8_char_length(c);
                }

                // color override if a rule matched
                if (r->mark[0] && color != COLOR_COMMENT)
                    color = r->color_end_of_line;

                for (j = 0; j < nb_to_color; j++, i++)
                    STORE_BUFFER(fg, i, color)
            }

        // potential rule end of line (in case of only rules language)
        } else if (r->mark[0])
            while (i < nb_displayed)
                fg[i++] = r->color_end_of_line;
    }

    // background
    if (settings.highlight_selections) {
        for (; s && s->l < l->line_nb; s = s->next);
        for (; s && s->l == l->line_nb; s = s->next)
            for (i = 0; i < s->n; i++)
                STORE_BUFFER(bg, s->x + i, COLOR_BG_SELECTIONS)
    }
#ifdef HIGHLIGHT_MATCHING_BRACKET
    if (is_bracket) {
        if (l->line_nb == first_line_nb + y)
            STORE_BUFFER(bg, x, COLOR_BG_MATCHING)
        if (l->line_nb == matching_bracket.l)
            STORE_BUFFER(bg, matching_bracket.x, COLOR_BG_MATCHING)
    }
#endif // HIGHLIGHT_MATCHING_BRACKET

    // printing
    if (LINE_NUMBERS_WIDTH > 0)
        tb_printf(0, screen_line, COLOR_LINE_NUMBERS, COLOR_BG_DEFAULT,
            "%*d ", LINE_NUMBERS_WIDTH - 1, (l->line_nb)%LINE_NUMBERS_MODULUS);
    for (i = 0; i < nb_displayed; i++)
        tb_set_cell(i + LINE_NUMBERS_WIDTH, screen_line, ch[i], fg[i] | underline, bg[i]);
    for (i += LINE_NUMBERS_WIDTH; i < screen_width; i++)
        tb_set_cell(i, screen_line, ' ', COLOR_DEFAULT, COLOR_BG_DEFAULT);
#ifdef VISUAL_COLUMN
    tb_set_cell(VISUAL_COLUMN + LINE_NUMBERS_WIDTH, screen_line,
        (VISUAL_COLUMN < nb_displayed) ? (ch[VISUAL_COLUMN]) : ' ',
        (VISUAL_COLUMN < nb_displayed) ? (fg[VISUAL_COLUMN] | underline) :
            COLOR_DEFAULT, COLOR_BG_COLUMN);
#endif // VISUAL_COLUMN

    return s;
}

Selection *
range_lines_sel(int start, int end, Selection *next)
{
    Line *l;
    int i;

    for (i = end, l = get_line(end - first_line_nb); i >= start; i--, l = l->prev)
        next = create_sel(i, 0, l->dl, next);
    return next;
}

void
remove_sel_line_range(int min, int max)
{
    Selection *old, *next, *a;

    if (!(a = saved))
        return;
    if (a->l < min) {
        for (; a && a->l < min; old = a, a = a->next);
        for (; a && a->l <= max; next = a->next, free(a), a = next);
        old->next = a;
    } else {
        for (; a && a->l <= max; next = a->next, free(a), a = next);
        saved = a;
    }
    if (anchored && min <= anchor.l && anchor.l <= max)
        anchored = 0;
}

void
reorder_sel(int l, int nb, int new_l)
{
    // TODO: better with array of Line *
    Selection *s, *last, *last_before, *first, *last_first, *second, *last_second;
    int first_start, first_end, second_end;

    first_start = MIN(l, new_l);
    first_end = l + ((l < new_l) ? nb : 0) - 1;
    second_end = MAX(l, new_l) + nb - 1;
    if (anchored) {
        if (first_start <= anchor.l && anchor.l <= first_end)
            anchor.l += (l < new_l) ? (new_l - l) : nb;
        else if (first_end < anchor.l && anchor.l <= second_end)
            anchor.l -= (l < new_l) ? nb : (l - new_l);
    }
    if (!(s = last = saved))
        return;

    if (s->l < first_start) {
        for (; s && s->l < first_start; last = s, s = s->next);
        last_before = last;
    } else
        last_before = NULL;
    if (!s)
        return;

    if (s->l <= first_end) {
        first = s;
        for (; s && s->l <= first_end; last = s, s = s->next)
            s->l += (l < new_l) ? (new_l - l) : nb;
        last_first = last;
    } else
        first = last_first = NULL;
    if (!s)
        return;

    if (s->l <= second_end) {
        second = s;
        for (; s && s->l <= second_end; last = s, s = s->next)
            s->l -= (l < new_l) ? nb : (l - new_l);
        last_second = last;
    } else
        return;

    if (!last_before)
        saved = second;
    else
        last_before->next = second;
    if (!first)
        last_second->next = s;
    else {
        last_second->next = first;
        last_first->next = s;
    }
}

void
replace(Line *l, Selection *s)
{
    Substring to_add;
    char *rp, *replaced, *new_replaced, *src;
    int lrp, k, lr, ir, kr, k_chars;

    mark_fields(l->chars, s->x, s->n);
    mark_subpatterns(search_pattern.current, l->chars, l->dl, s->x, s->x, s->n);
    lrp = strlen(rp = replace_pattern.current);
    replaced = (char *) emalloc(lr = DEFAULT_BUF_SIZE);
    ir = kr = 0;
    for (k = 0; k < lrp;) {
        if (k < lrp - 1 && (rp[k] == '\\' || rp[k] == '$') && isdigit(rp[k + 1])) {
            src = l->chars;
            to_add = ((rp[k] == '$') ? fields : subpatterns)[rp[k + 1] - '0'];
            k += 2;
        } else {
            if (k < lrp - 1 && rp[k] == '\\')
                k++;
            src = rp;
            SET_SUBSTRING(to_add, 0, k, 1, tb_utf8_char_length(rp[k]));
            k += to_add.mn;
        }
        if (kr + to_add.mn >= lr) {
            while (kr + to_add.mn >= lr)
                lr <<= 1;
            new_replaced = (char *) emalloc(lr);
            strncpy(new_replaced, replaced, kr);
            free(replaced);
            replaced = new_replaced;
        }
        strncpy(replaced + kr, src + to_add.mst, to_add.mn);
        kr += to_add.mn;
        ir += to_add.n;
    }
    k_chars = replace_chars(l, s, s->x, s->n, ir, kr);
    strncpy(l->chars + k_chars, replaced, kr);
    free(replaced);
}

int
replace_chars(Line *l, Selection *a, int start, int n, int new_n, int nb_bytes)
{
    char *new_chars;
    int k1, k2;

    k1 = get_str_index(l->chars, start);
    k2 = k1 + get_str_index(l->chars + k1, n);
    l->ml += nb_bytes - (k2 - k1);
    l->dl += new_n - n;
    new_chars = (char *) emalloc(l->ml);
    strncpy(new_chars, l->chars, k1);
    strncpy(new_chars + k1 + nb_bytes, l->chars + k2, l->ml - (k1 + nb_bytes));
    free(l->chars);
    l->chars = new_chars;
    for (; a && a->l < l->line_nb; a = a->next);
    for (; a && a->l == l->line_nb; a = a->next)
        if (a->x < start)
            if (a->x + a->n <= start) {
            } else if (a->x + a->n <= start + n)
                a->n = start - a->x;
            else
                a->n += new_n - n;
        else if (a->x < start + n)
            if (a->x + a->n <= start + n) {
                a->x = start;
                a->n = new_n;
            } else {
                a->n -= start + new_n - a->x + n;
                a->x = start + new_n;
            }
        else
            a->x += new_n - n;
    if (l->line_nb == first_line_nb + y && start <= x)
        ATTRIBUTE_X((x < start + n) ? (start + new_n) : (x + new_n - n));
    if (anchored && l->line_nb == anchor.l && start <= anchor.x)
        anchor.x = (anchor.x < start + n) ? start : (anchor.x + new_n - n);
    return k1;
}

void
reset_selections(void)
{
    SET_SEL_LIST(saved, NULL)
    anchored = 0;
}

int
resize(int width, int height)
{
    if ((screen_width = width) < MIN_WIDTH || (screen_height = height) < MIN_HEIGHT)
        return EXIT_FAILURE;
    scroll_offset = MIN(SCROLL_OFFSET, (screen_height >> 1) - 1);
    free(ch); ch = emalloc((width - LINE_NUMBERS_WIDTH)*sizeof(uint32_t));
    free(fg); fg = emalloc((width - LINE_NUMBERS_WIDTH)*sizeof(uintattr_t));
    free(bg); bg = emalloc((width - LINE_NUMBERS_WIDTH)*sizeof(uintattr_t));
    return EXIT_SUCCESS;
}

Selection *
search(Selection *a)
{
    Line *l;
    Selection *res, *last, *new;
    int k, len;

    if (!a)
        return NULL;
    l = get_line(a->l - first_line_nb);
    res = last = NULL;
    for (; a; a = a->next) {
        for (; l->line_nb < a->l; l = l->next);
        for (k = 0; k < a->n; k += len)
            if ((len = mark_subpatterns(search_pattern.current, l->chars, l->dl,
                a->x, a->x + k, a->n - k))) {
                new = create_sel(l->line_nb, a->x + k, len, NULL);
                if (!last)
                    res = last = new;
                else {
                    last->next = new;
                    last = new;
                }
            } else
                len = 1;
    }
    return res;
}

int
search_word_under_cursor(void)
{
    Line *l;
    Selection *tmp;
    int i, k, k1, k2;

    l = get_line(y);
    k = k1 = get_str_index(l->chars, i = x);
    if (!is_word_char(l->chars[k]))
        return EXIT_FAILURE;
    for (; is_word_char(l->chars[k]) && k > 0; decrement(l->chars, &i, &k, i - 1));
    k1 = is_word_char(l->chars[k]) ? 0 : (k + tb_utf8_char_length(l->chars[k]));
    for (k2 = k1; is_word_char(l->chars[k2]); k2 += tb_utf8_char_length(l->chars[k2]));
    if (k2 - k1 + 5 > INTERFACE_MEM_LENGTH)
        return EXIT_FAILURE;
    strcpy(search_pattern.current, "\\b");
    strncpy(search_pattern.current + 2, l->chars + k1, k2 - k1);
    strcpy(search_pattern.current + 2 + k2 - k1, "\\b");
    strcpy(search_pattern.previous, search_pattern.current);
    tmp = search(saved);
    SET_SEL_LIST(saved, tmp);
    return EXIT_SUCCESS;
}

void
shift_line_nb(Line *l, int min, int max, int delta)
{
    for (; l && l->line_nb < min; l = l->next);
    for (; l && (!max || l->line_nb <= max); l = l->next)
        l->line_nb += delta;
}

void shift_sel_line_nb(Selection *a, int min, int max, int delta)
{
    for (; a && a->l < min; a = a->next);
    for (; a && (!max || a->l <= max); a = a->next)
        a->l += delta;
    if (anchored && min <= anchor.l && (!max || anchor.l <= max))
        anchor.l += delta;
}

void
split(Line *l, Selection *s)
{
    break_line(l, s, s->x);
}

void
suppress(Line *l, Selection *s)
{
    int start, nb_deleted;

    if (s->n) {
        start = s->x;
        nb_deleted = s->n;
        s->x += nb_deleted; // compensate selection shift done by replace_chars
        s->n = 0;
    } else {
        if (asked_remove > 0) {
            start = s->x;
            if (!(nb_deleted = MIN(asked_remove, l->dl - start))) {
                if (l->next)
                    concatenate_line(l, s);
                return;
            }
        } else {
            start = MAX(s->x + asked_remove, 0);
            if (!(nb_deleted = s->x - start)) {
                if (l->prev)
                    concatenate_line(l->prev, s);
                return;
            }
        }
    }
    replace_chars(l, s, start, nb_deleted, 0, 0);
}

void
unwrap_pos(Pos p)
{
    y = p.l - first_line_nb;
    ATTRIBUTE_X(p.x);
}

void
upper(Line *l, Selection *s)
{
    int i, k;
    char c;

    for (k = get_str_index(l->chars, i = s->x); i < s->x + s->n; i++, k += tb_utf8_char_length(c))
        if (islower(c = l->chars[k]) || c == ACC_LETTER)
            l->chars[k + ((c == ACC_LETTER) ? 1 : 0)] &= ~(1 << 5);
}

void
write_file(const char *file_name)
{
    Line *l;
    FILE *dest_file;
    char *chars;
    int k, nb_bytes;

    FILE_IO(dest_file = fopen(file_name, "w"), NULL)
    for (l = first_line; l; l = l->next) {
        chars = l->chars;
        nb_bytes = l->ml - 1;
#ifdef IGNORE_TRAILING_SPACES
        for (k = l->ml - 2; k >= 0 && chars[k] == ' '; k--)
            nb_bytes--;
#endif // IGNORE_TRAILING_SPACES
// TODO: potentially translate more than one tab
        if (lang && lang->flags & CONVERT_LEADING_SPACES
            && !strncmp(chars, "        ", settings.tab_width)) {
            FILE_IO(putc('\t', dest_file), EOF)
            chars += settings.tab_width;
            nb_bytes -= settings.tab_width;
        }
        for (k = 0; k < nb_bytes; k++)
            FILE_IO(putc(*chars++, dest_file), EOF)
        FILE_IO(putc('\n', dest_file), EOF)
    }
    FILE_IO(fclose(dest_file), EOF)
}

void *
emalloc(size_t size)
{
    void *p;

    if (!(p = malloc(size)))
        die(EXIT_FAILURE, ERR_MALLOC);
    return p;
}

int
main(int argc, char *argv[])
{
    Interface command_int, range_int, settings_int;
    Pos p;
    int m = 0, l1, l2, old_line_nb;
    struct tb_event ev;

    if (argc == 2 && !strcmp("-v", argv[1]))
        die(EXIT_FAILURE, "edit-"VERSION);
    else if (argc != 2)
        die(EXIT_FAILURE, "usage: edit filename");
    else if (strlen(argv[1]) + 1 > INTERFACE_MEM_LENGTH)
        die(EXIT_FAILURE, ERR_BUFFER_TOO_SMALL);
    INIT_INTERFACE(file_name_int, argv[1])
    INIT_INTERFACE(search_pattern, "")
    INIT_INTERFACE(replace_pattern, "")
    INIT_INTERFACE(command_int, "")
    INIT_INTERFACE(range_int, "")
    INIT_INTERFACE(settings_int, "")
    load_file(1);
    init_termbox();
    echo(WELCOME_MESSAGE);
    while (1) {
        if (has_been_invalid_resizing) {
            if (has_been_changes)
                write_file(BACKUP_FILE_NAME);
            die(EXIT_FAILURE, ERR_TERM_TOO_SMALL);
        }
        move_to_cursor();
        SET_SEL_LIST(running, compute_running_sel())
        SET_SEL_LIST(displayed, merge_sel(running, saved));
        if (in_insert_mode)
            echo(INSERT_MODE_MESSAGE);
        print_all();
        tb_present();
        tb_poll_event(&ev);
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
                    if (ev.ch == KB_QUIT && has_been_changes)
                        echo(UNSAVED_CHANGES_MESSAGE);
                    else
                        die(EXIT_SUCCESS, NULL);
                    break;
                case KB_WRITE:
                case KB_WRITE_AS:
                    if (ev.ch == KB_WRITE && !has_been_changes)
                        echo(NOTHING_TO_WRITE_MESSAGE);
                    else if (ev.ch == KB_WRITE ||
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
                    } else
                        echo(NOTHING_TO_REVERT_MESSAGE);
                    break;
                case KB_CHANGE_SETTING:
                    if (dialog(CHANGE_SETTING_PROMPT, &settings_int, 0))
                        if (parse_assign(settings_int.current))
                            echo(INVALID_ASSIGNMENT_MESSAGE);
                    break;
                case KB_RUN_MAKE:
                case KB_RUN_SHELL_COMMAND:
                    if (ev.ch == KB_RUN_MAKE
                        || dialog(COMMAND_PROMPT, &command_int, 0)) {
                        tb_shutdown();
                        system((ev.ch == KB_RUN_MAKE) ? "make" :
                            command_int.current);
                        getchar();
                        init_termbox();
                    }
                    break;
                case KB_INSERT_START_LINE:
                case KB_INSERT_END_LINE:
                case KB_INSERT_LINE_BELOW:
                case KB_INSERT_LINE_ABOVE:
                    reset_selections();
                    if (ev.ch == KB_INSERT_LINE_BELOW || ev.ch == KB_INSERT_LINE_ABOVE) {
                        y += (ev.ch == KB_INSERT_LINE_BELOW) ? 1 : 0;
                        insert_line(first_line_nb + y, 1, 0);
                    }
                    ATTRIBUTE_X((ev.ch == KB_INSERT_END_LINE) ? get_line(y)->dl : 0);
                    // fall-through
                case KB_INSERT_MODE:
                    in_insert_mode = 1;
                    break;
                case KB_MOVE_MATCHING:
                    unwrap_pos(find_matching_bracket());
                    break;
                case KB_MOVE_START_LINE:
                    ATTRIBUTE_X(0);
                    break;
                case KB_MOVE_NON_BLANK:
                    ATTRIBUTE_X(find_first_non_blank(get_line(y)));
                    break;
                case KB_MOVE_END_LINE:
                    ATTRIBUTE_X(get_line(y)->dl);
                    break;
                case KB_MOVE_END_FILE:
                    m = nb_lines;
                    // fall-through
                case KB_MOVE_SPECIFIC_LINE:
                    y = m - first_line_nb;
                    break;
                case KB_MOVE_NEXT_CHAR:
                case KB_MOVE_PREV_CHAR:
                    ATTRIBUTE_X(x + way(ev.ch == KB_MOVE_NEXT_CHAR));
                    break;
                case KB_MOVE_NEXT_LINE:
                case KB_MOVE_PREV_LINE:
                    y += way(ev.ch == KB_MOVE_NEXT_LINE);
                    break;
                case KB_MOVE_NEXT_WORD:
                case KB_MOVE_PREV_WORD:
                    unwrap_pos(find_start_of_word(way(ev.ch == KB_MOVE_NEXT_WORD)));
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
                    if (ev.ch == KB_SEL_CURSOR_LINE)
                        l1 = l2 = first_line_nb + y;
                    else if (ev.ch == KB_SEL_ALL_LINES) {
                        l1 = 1;
                        l2 = nb_lines;
                    } else {
                        l1 = find_block_delim(first_line_nb + y, -1);
                        l2 = find_block_delim(l1, m);
                    }
                    SET_SEL_LIST(saved, range_lines_sel(l1, l2, NULL))
                    break;
                case KB_SEL_CUSTOM_RANGE:
                    if (dialog(RANGE_PROMPT, &range_int, 0))
                        if (parse_range(range_int.current))
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
                    if (search_word_under_cursor())
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
                    if (lang && !(lang->flags & ONLY_RULES))
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
                    if (ev.ch == KB_CLIP_YANK_LINE || ev.ch == KB_CLIP_DELETE_LINE)
                        l1 = first_line_nb + y;
                    else {
                        l1 = find_block_delim(first_line_nb + y, -1);
                        m = find_block_delim(l1, m) - l1 + 1;
                    }
                    if (ev.ch == KB_CLIP_YANK_LINE || ev.ch == KB_CLIP_YANK_BLOCK)
                        copy_to_clip(l1, m);
                    else
                        move_to_clip(l1, m);
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
                    ATTRIBUTE_X(ev.x - LINE_NUMBERS_WIDTH);
                }
                break;
            case TB_KEY_MOUSE_WHEEL_UP:
                old_line_nb = first_line_nb + y;
                first_line_on_screen = get_line(-SCROLL_LINE_NUMBER);
                y = MIN(old_line_nb - first_line_nb, screen_height - 2 - scroll_offset);
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
