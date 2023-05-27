// see LICENSE file for copyright and license details

#define MIN_HEIGHT                  2
#if LINE_NUMBERS_WIDTH < 1
    #define LINE_NUMBERS_MODULUS    1
#elif LINE_NUMBERS_WIDTH == 2
    #define LINE_NUMBERS_MODULUS    10
#elif LINE_NUMBERS_WIDTH == 3
    #define LINE_NUMBERS_MODULUS    100
#elif LINE_NUMBERS_WIDTH == 4
    #define LINE_NUMBERS_MODULUS    1000
#else
    #define LINE_NUMBERS_MODULUS    10000
#endif

#define IS_TYPE(TYPE, L)            is_in(*(syntax->TYPE), l->chars, k, L)

int scroll_offset;                  // minimum number of lines around cursor
int screen_height, screen_width;    // terminal dimensions
char message[INTERFACE_MEM_LENGTH]; // what is printed in the INTERFACE area

static int is_bracket;
static struct pos matching_bracket;

void
init_termbox(void)
{
    // initialise the interface

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

int
resize(int width, int height)
{
    // try to resize terminal to the given width and height

    if ((screen_width = width) < MIN_WIDTH ||
        (screen_height = height) < MIN_HEIGHT)
        return ERR_TERM_NOT_BIG_ENOUGH;

    scroll_offset = MIN(SCROLL_OFFSET, (screen_height >> 1) - 1);

    return 0;
}

struct selection *
print_line(const struct line *l, struct selection *s, int screen_line)
{
    // print line *l to screen, on line screen_line
    // return selection queue after this line

    // variables
    uint32_t *ch = _malloc((l->dl) * sizeof(uint32_t)); // Unicode codepoints
    uint16_t *fg = _malloc((l->dl) * sizeof(uint16_t)); // foreground attributes
    uint16_t *bg = _malloc((l->dl) * sizeof(uint16_t)); // background attributes
    struct lang *syntax = settings.syntax;
    struct rule *r;
    int i, k;                       // indexes (characters, bytes) in l->chars
    int j, dk, len, color, nb_to_color, underline;
    char c, nc;

#ifdef UNDERLINE_CURSOR_LINE
    underline = (screen_line == y) ? TB_UNDERLINE : 0;
#else
    underline = 0;
#endif // UNDERLINE_CURSOR_LINE

    // decompress UTF-8, initialise foreground and background
    for (i = k = 0; i < l->dl; i++, k += len) {
        ch[i] = unicode(l->chars, k, len = utf8_char_length(l->chars[k]));
        fg[i] = COLOR_DEFAULT;
        bg[i] = COLOR_BG_DEFAULT;
    }

    // foreground
    if (settings.syntax_highlight && syntax != NULL) {
        // ignore blank characters at the beginning of the line
        i = k = find_first_non_blank(l);

        // detect a matching rule
        for (r = *(syntax->rules); r->mark[0]; r++)
            if (!strncmp(&(l->chars[((r->start_of_line) ? 0 : k)]),
                r->mark, strlen(r->mark)))
                break;

        if (r->mark[0]) {
            // RULE
            if (r->start_of_line)
                i = 0;
            for (j = 0; j < strlen(r->mark); j++)
                fg[i++] = r->color_mark;
            while (i < l->dl)
                fg[i++] = r->color_end_of_line;
        } else if (syntax->highlight_elements) {
            while (i < l->dl) {
                // WORD
                if (is_word_char(c = l->chars[k])) {
                    for (j = dk = 0; is_word_char(nc = l->chars[k + dk]) ||
                        isdigit(nc); j++, dk += utf8_char_length(nc))
                        ;
                    color = COLOR_DEFAULT;
                    if (j != dk) {
                        // non-ASCII character, do not look for match
                    } else if (IS_TYPE(keywords, j)) {
                        color = COLOR_KEYWORD;
                    } else if (IS_TYPE(flow_control, j)) {
                        color = COLOR_FLOW_CONTROL;
                    } else if (IS_TYPE(built_ins, j)) {
                        color = COLOR_BUILT_IN;
                    }
                    nb_to_color = j;
                    k += dk;

                // NUMBER
                } else if (isdigit(c) || (k+1 < l->ml && (c == '-' || c == '.')
                    && (isdigit(l->chars[k+1]) || l->chars[k+1] == '.'))) {
                    for (j = 1; isdigit(c = l->chars[k+j]) || c == '.'; j++)
                        ;
                    color = COLOR_NUMBER;
                    nb_to_color = j;
                    k += j;

                // STRING
                } else if (c == '"' || c == '\'') {
                    k++; j = 1;
                    while (1) {
                        if (l->chars[k] == c) {
                            k += utf8_char_length(l->chars[k]); j++;
                            break;
                        } else if (l->chars[k] == '\\') {
                            k++; j++;
                        }
                        if (l->chars[k] == '\0')
                            break;
                        k += utf8_char_length(l->chars[k]); j++;
                    }
                    color = COLOR_STRING;
                    nb_to_color = j;

                // COMMENT
                } else if (IS_TYPE(comment, strlen(*(syntax->comment)) - 1)) {
                    color = COLOR_COMMENT;
                    nb_to_color = l->dl - i;
                    k = l->ml;

                // SOMETHING ELSE
                } else {
                    color = COLOR_DEFAULT;
                    nb_to_color = 1;
                    k += utf8_char_length(c);
                }

                for (j = 0; j < nb_to_color; j++)
                    fg[i++] = color;
            }
        }
    }

    // background
    if (settings.highlight_selections) {
        while (s != NULL && s->l < l->line_nb)
            s = s->next;
        while (s != NULL && s->l == l->line_nb) {
            for (i = 0; i < s->n && s->x + i < l->dl; i++)
                bg[s->x + i] = COLOR_BG_SELECTIONS;
            s = s->next;
        }
    }
#ifdef HIGHLIGHT_MATCHING_BRACKET
    if (is_bracket) {
        if (l->line_nb == first_line_nb + y)
            bg[x] = COLOR_BG_MATCHING;
        if (l->line_nb == matching_bracket.l)
            bg[matching_bracket.x] = COLOR_BG_MATCHING;
    }
#endif // HIGHLIGHT_MATCHING_BRACKET

    // actual printing
    if (LINE_NUMBERS_WIDTH > 0)
        tb_printf(0, screen_line, COLOR_LINE_NUMBERS, COLOR_BG_DEFAULT,
            "%*d ", LINE_NUMBERS_WIDTH - 1, (l->line_nb)%LINE_NUMBERS_MODULUS);
    for (i = 0; i < l->dl; i++)
        tb_set_cell(i + LINE_NUMBERS_WIDTH, screen_line, ch[i],
            fg[i] | underline, bg[i]);
    for (i += LINE_NUMBERS_WIDTH; i < screen_width; i++)
        tb_set_cell(i, screen_line, ' ', COLOR_DEFAULT, COLOR_BG_DEFAULT);
#ifdef VISUAL_COLUMN
    tb_set_cell(VISUAL_COLUMN + LINE_NUMBERS_WIDTH, screen_line,
        (VISUAL_COLUMN < l->dl) ? (ch[VISUAL_COLUMN]) : ' ',
        (VISUAL_COLUMN < l->dl) ? (fg[VISUAL_COLUMN] | underline) :
            COLOR_DEFAULT, COLOR_BG_COLUMN);
#endif // VISUAL_COLUMN

    // forget buffers
    free(ch);
    free(fg);
    free(bg);

    return s;
}

void
print_dialog(void)
{
    // display the dialog line

    int len, i, j, k;
    char c;

    // decompress UTF-8 and print
    for (i = k = 0; c = message[k]; i++, k += len)
        tb_set_cell(i, screen_height - 1,
            unicode(message, k, len = utf8_char_length(c)),
            COLOR_DIALOG, COLOR_BG_DEFAULT);

    // erase end of line
    while (i < screen_width - RULER_WIDTH)
        tb_set_cell(i++, screen_height-1, ' ', COLOR_DEFAULT, COLOR_BG_DEFAULT);
}

void
print_ruler(void)
{
    // display the ruler

    int i;

    // erase
    for (i = screen_width - RULER_WIDTH; i < screen_width; i++)
        tb_set_cell(i, screen_height - 1, ' ', COLOR_DEFAULT, COLOR_BG_DEFAULT);

    // print
    tb_printf(screen_width - RULER_WIDTH, screen_height - 1,
        COLOR_RULER, COLOR_BG_DEFAULT, RULER_PATTERN, first_line_nb + y, x);
}

void
print_all(void)
{
    // clear and display all the elements

    struct selection *s;
    struct line *l;
    int sc_line;
    char c;

#ifdef HIGHLIGHT_MATCHING_BRACKET
    l = get_line(y);
    c = l->chars[get_str_index(l->chars, x)];
    if (is_bracket = (c == '{' || c == '}' || c == '[' || c == ']'
        || c == '(' || c == ')' || c == '<' || c == '>'))
        matching_bracket = find_matching_bracket();
#endif // HIGHLIGT_MATCHING_BRACKET

    s = displayed;
    l = first_line_on_screen;
    tb_clear();
    for (sc_line = 0; l != NULL && sc_line < screen_height - 1; sc_line++) {
        s = print_line(l, s, sc_line);
        l = l->next;
    }
    tb_set_cursor(x + LINE_NUMBERS_WIDTH, y);
    print_dialog();
    print_ruler();
}
