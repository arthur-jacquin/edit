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
    int i, j, color, nb_to_color, underline;
    char c, nc;
    int *fg;
    int *bg;
    fg = (int *) malloc(length * sizeof(int));
    bg = (int *) malloc(length * sizeof(int));

    // underline current line
    underline = (screen_line == y) ? TB_UNDERLINE : 0;

    // foreground
    if (settings.syntax_highlight && strcmp(settings.language, "none")) {
        i = 0;
        while (i < length) {
            color = COLOR_DEFAULT;
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
                fg[i++] = color | underline;
        }
    } else {
        for (i = 0; i < length; i++)
            fg[i] = COLOR_DEFAULT;
    }
    
    // background
    for (i = 0; i < length; i++)
        bg[i] = COLOR_BG_DEFAULT;
    if (settings.highlight_selections) {
        while (s != NULL && s->l < line_nb)
            s = s->next;
        while (s != NULL && s->l == line_nb) {
            for (i = 0; i < s->n && s->x + i < length; i++)
                bg[s->x + i] = COLOR_BG_SELECTIONS;
            s = s->next;
        } 
    }
    if (is_bracket) {
        if (line_nb == first_line_on_screen->line_nb + y)
            bg[x] = COLOR_BG_MATCHING;
        if (line_nb == matching_bracket.l)
            bg[matching_bracket.x] = COLOR_BG_MATCHING;
    }

    // actual printing
    fg[length - 1] = COLOR_DEFAULT;
    bg[length - 1] = COLOR_BG_DEFAULT;
    for (i = 0; i < length; i++)
        tb_set_cell(i, screen_line, chars[i], fg[i], bg[i]);
    for (; i < screen_width; i++)
        tb_set_cell(i, screen_line, ' ', COLOR_DEFAULT, COLOR_BG_DEFAULT);

    // forget about fg, bg
    free(fg);
    free(bg);

    return s;
}

void
print_dialog(void)
{
    int i;

    tb_print(0, screen_height - 1, COLOR_DIALOG, COLOR_BG_DEFAULT, dialog_chars);
    for (i = strlen(dialog_chars); i < screen_width - RULER_WIDTH; i++)
        tb_set_cell(i, screen_height - 1, ' ', COLOR_DEFAULT, COLOR_BG_DEFAULT);
}

void
print_ruler(void)
{
    int i;

    for (i = screen_width - RULER_WIDTH; i < screen_width; i++)
        tb_set_cell(i, screen_height - 1, ' ', COLOR_DEFAULT, COLOR_BG_DEFAULT);
    tb_printf(screen_width - RULER_WIDTH, screen_height - 1,
        COLOR_RULER, COLOR_BG_DEFAULT,
        "%d,%d", first_line_on_screen->line_nb + y, x);
}

void
print_all(void)
{
    struct selection *s;
    struct line *ptr;
    int sc_line;
    char c;

    tb_clear();

    c = get_line(y)->chars[x];
    is_bracket = (c == '{' || c == '}' || c == '[' || c == ']' || c == '(' || c == ')');
    if (is_bracket)
        matching_bracket = find_matching_bracket();

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
