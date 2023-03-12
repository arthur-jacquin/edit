#define IS_TYPE(TYPE, L)        is_in(*(syntax->TYPE), l->chars, k, (L))

void
init_termbox(void)
{
    // initialise the interface

    tb_init();
    tb_set_output_mode(OUTPUT_MODE);
#ifdef MOUSE_SUPPORT
    tb_set_input_mode(TB_INPUT_ESC | TB_INPUT_MOUSE);
#else
    tb_set_input_mode(TB_INPUT_ESC);
#endif // MOUSE_SUPPORT
    if (resize(tb_width(), tb_height()))
        has_been_invalid_resizing = 1;
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
    int k, i, j, len, color, nb_to_color, underline;
    char c, nc;
    struct printable *buf = _malloc((l->dl) * sizeof(struct printable));
    struct lang *syntax = settings.syntax;
    struct rule *r;
    int a;

#ifdef UNDERLINE_CURSOR_LINE
    underline = (screen_line == y) ? TB_UNDERLINE : 0;
#else
    underline = 0;
#endif // UNDERLINE_CURSOR_LINE

    // decompress UTF-8
    for (i = k = 0; i < l->dl; i++, k += len)
        buf[i].ch = unicode(l->chars, k, len = utf8_char_length(l->chars[k]));

    // foreground
    for (i = 0; i < l->dl; i++)
        buf[i].fg = COLOR_DEFAULT;
    if (settings.syntax_highlight && settings.syntax != NULL) {
        // ignores blank characters at the beginning of the line
        for (i = k = 0; l->chars[k] == ' '; i++, k++)
            ;

        // detect a matching rule
        for (r = *(syntax->rules); r->mark[0]; r++)
            if (((i == 0) || !(r->start_of_line)) &&
                (!strncmp(r->mark, &(l->chars[k]), strlen(r->mark))))
                    break;

        if (r->mark[0]) {
            // RULE
            for (j = 0; j < strlen(r->mark); j++)
                buf[i++].fg = r->color_mark;
            while (i < l->dl)
                buf[i++].fg = r->color_end_of_line;
        } else if (syntax->highlight_elements) {
            while (i < l->dl) {
                color = COLOR_DEFAULT;
                nb_to_color = 1;
                c = l->chars[k];

                // NON ASCII CHARACTER
                if (utf8_char_length(c) > 1) {
                    k += utf8_char_length(c);
                    while (is_word_char(l->chars[k])) {
                        k += utf8_char_length(l->chars[k]);
                        nb_to_color++;
                    }

                // WORD
                } else if (is_word_char(c)) {
                    for (j = 0; is_word_char(nc = l->chars[k+j]) ||
                        isdigit(nc); j++)
                        ;
                    if (IS_TYPE(keywords, j)) {
                        color = COLOR_KEYWORD;
                    } else if (IS_TYPE(flow_control, j)) {
                        color = COLOR_FLOW_CONTROL;
                    } else if (IS_TYPE(built_ins, j)) {
                        color = COLOR_BUILT_IN;
                    }
                    nb_to_color = j;
                    k += j;

                // NUMBER
                } else if (isdigit(c) || (k+1 < l->ml && (c == '-' || c == '.')
                    && (isdigit(nc = l->chars[k+1]) || nc == '.'))) {
                    for (j = 1; isdigit(nc = l->chars[k+j]) || nc == '.'; j++)
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
                    k++;
                }

                for (j = 0; j < nb_to_color; j++)
                    buf[i++].fg = color;
            }
        }
    }

    // background
    for (i = 0; i < l->dl; i++)
        buf[i].bg = COLOR_BG_DEFAULT;
    if (settings.highlight_selections) {
        while (s != NULL && s->l < l->line_nb)
            s = s->next;
        while (s != NULL && s->l == l->line_nb) {
            for (i = 0; i < s->n && s->x + i < l->dl; i++)
                buf[s->x + i].bg = COLOR_BG_SELECTIONS;
            s = s->next;
        }
    }
#ifdef HIGHLIGHT_MATCHING_BRACKET
    if (is_bracket) {
        if (l->line_nb == first_line_nb + y)
            buf[x].bg = COLOR_BG_MATCHING;
        if (l->line_nb == matching_bracket.l)
            buf[matching_bracket.x].bg = COLOR_BG_MATCHING;
    }
#endif // HIGHLIGHT_MATCHING_BRACKET

    // actual printing
    if (LINE_NUMBERS_WIDTH > 0)
        tb_printf(0, screen_line, COLOR_LINE_NUMBERS, COLOR_BG_DEFAULT,
            "%*d ", LINE_NUMBERS_WIDTH - 1, (l->line_nb)%LINE_NUMBERS_MODULUS);
    for (i = 0; i < l->dl; i++)
        tb_set_cell(i + LINE_NUMBERS_WIDTH, screen_line, buf[i].ch,
            buf[i].fg | underline, buf[i].bg);
    for (i += LINE_NUMBERS_WIDTH; i < screen_width; i++)
        tb_set_cell(i, screen_line, ' ', COLOR_DEFAULT, COLOR_BG_DEFAULT);
#ifdef VISUAL_COLUMN
    tb_set_cell(VISUAL_COLUMN + LINE_NUMBERS_WIDTH, screen_line,
        (VISUAL_COLUMN < l->dl) ? (buf[VISUAL_COLUMN].ch) : ' ',
        (VISUAL_COLUMN < l->dl) ? (buf[VISUAL_COLUMN].fg | underline) :
            COLOR_DEFAULT, COLOR_BG_COLUMN);
#endif // VISUAL_COLUMN

    // forget buffer
    free(buf);

    return s;
}

void
print_dialog(void)
{
    // display the dialog line

    int len, i, j, k;
    char nc;

    // decompress UTF-8 and print
    for (i = k = 0; nc = message[k]; i++, k += len)
        tb_set_cell(i, screen_height - 1,
            unicode(message, k, len = utf8_char_length(nc)),
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

    for (i = screen_width - RULER_WIDTH; i < screen_width; i++)
        tb_set_cell(i, screen_height - 1, ' ', COLOR_DEFAULT, COLOR_BG_DEFAULT);
    tb_printf(screen_width - RULER_WIDTH, screen_height - 1,
        COLOR_RULER, COLOR_BG_DEFAULT,
        "%d:%d", first_line_nb + y, x);
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
