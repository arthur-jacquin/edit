int
resize(int width, int height)
{
    // try to resize terminal to the given width and height

    if ((screen_width = width) < MIN_WIDTH ||
        (screen_height = height) < MIN_HEIGHT)
        return ERR_TERM_NOT_BIG_ENOUGH;

    return 0;
}

void
echo(const char *chars)
{
    // set dialog line to chars

    strcpy(dialog_chars, chars);
}

struct selection *
print_line(struct line *l, struct selection *s, int screen_line)
{
    // print line *l to screen, on line screen_line
    // return selection queue after this line

    // variables
    int k, i, j, len, color, nb_to_color, underline;
    char c, nc;
    struct tb_cell *buf = malloc((l->dl) * sizeof(struct tb_cell));
    struct lang *syntax = settings.syntax;

    // underline current line
    underline = (screen_line == y) ? TB_UNDERLINE : 0;

    // decompress UTF-8
    for (k = i = 0; i < l->dl; i++) {
        len = utf8_char_length(l->chars[k]);
        buf[i].ch = l->chars[k++] & masks[len-1];
        for (j = 1; j < len; j++) {
            buf[i].ch <<= 6;
            buf[i].ch |= 0x3f & (l->chars[k++]);
        }
    }

    // foreground
    for (i = 0; i < l->dl; i++)
        buf[i].fg = COLOR_DEFAULT;
    if (settings.syntax_highlight && strcmp(settings.language, "none")) {
        k = i = 0;
        while (i < l->dl) {
            color = COLOR_DEFAULT;
            nb_to_color = 1;
            c = l->chars[k];
            if (utf8_char_length(c) > 1) {
                k += utf8_char_length(c);
            } else if (is_word_char(c)) {
                for (j = 0; is_word_char(nc = l->chars[k+j]) || is_digit(nc); j++)
                    ;
                if (is_in(*((syntax)->keywords), l->chars, k, j)) {
                    color = COLOR_KEYWORD;
                } else if (is_in(*(syntax->flow_control), l->chars, k, j)) {
                    color = COLOR_FLOW_CONTROL;
                } else if (is_in(*(syntax->built_ins), l->chars, k, j)) {
                    color = COLOR_BUILT_IN;
                }
                nb_to_color = j;
                k += j;
            } else if (is_digit(c) || (k+1 < l->ml && (c == '-' || c == '.') &&
                (is_digit(nc = l->chars[k+1]) || nc == '.'))) {
                for (j = 1; is_digit(nc = l->chars[k+j]) || nc == '.'; j++)
                    ;
                color = COLOR_NUMBER;
                nb_to_color = j;
                k += j;
            } else if (c == '"' || c == '\'') {
                k++;
                for (j = 2; k < l->ml && !(l->chars[k] == c && l->chars[k-1] != '\\'); j++)
                    k += utf8_char_length(l->chars[k]);
                k++;
                color = COLOR_STRING;
                nb_to_color = j;
            } else if (is_in(*(syntax->comment), l->chars, k, strlen(*(syntax->comment)) - 1)) {
                color = COLOR_COMMENT;
                nb_to_color = l->dl - i;
                k = l->ml;
            } else {
                k++;
            }

            for (j = 0; j < nb_to_color; j++)
                buf[i++].fg = color | underline;
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
    if (is_bracket) {
        if (l->line_nb == first_line_on_screen->line_nb + y)
            buf[x].bg = COLOR_BG_MATCHING;
        if (l->line_nb == matching_bracket.l)
            buf[matching_bracket.x].bg = COLOR_BG_MATCHING;
    }

    // actual printing
    for (i = 0; i < l->dl; i++)
        tb_set_cell(i, screen_line, buf[i].ch, buf[i].fg, buf[i].bg);
    for (; i < screen_width; i++)
        tb_set_cell(i, screen_line, ' ', COLOR_DEFAULT, COLOR_BG_DEFAULT);

    // forget buffer
    free(buf);

    return s;
}

void
print_dialog(void)
{
    // display the dialog line

    int len, i, j, k;
    uint32_t c, nc;

    // decompress UTF-8 and print
    for (i = k = 0; nc = dialog_chars[k++]; i++) {
        len = utf8_char_length(nc);
        c = nc & masks[len-1];
        for (j = 1; j < len; j++) {
            c <<= 6;
            c |= 0x3f & dialog_chars[k++];
        }
        tb_set_cell(i, screen_height-1, c, COLOR_DIALOG, COLOR_BG_DEFAULT);
    }

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
        "%d:%d", first_line_on_screen->line_nb + y, x);
}

void
print_all(void)
{
    // clear and display all the elements

    struct selection *s;
    struct line *l;
    int sc_line;
    char c;

    // clear the interface
    tb_clear();

    l = get_line(y);
    c = l->chars[get_str_index(l, x)];
    is_bracket = (c == '{' || c == '}' || c == '[' || c == ']'
        || c == '(' || c == ')' || c == '<' || c == '>');
    if (is_bracket)
        matching_bracket = find_matching_bracket();

    s = displayed;
    l = first_line_on_screen;
    for (sc_line = 0; l != NULL && sc_line < screen_height - 1; sc_line++) {
        s = print_line(l, s, sc_line);
        l = l->next;
    }

    tb_set_cursor(x, y);
    print_dialog();
    print_ruler();
}
