void
act(void (*process)(struct line *, struct selection *), int line_op)
{
    // execute process on saved selections if any, else on temp selections
    // line_op can be set to 1 so that process is executed at most once per line

    struct selection *s;
    struct line *l;
    int old_line_nb;

    s = (saved != NULL) ? saved : temp;
    l = get_line(s->l - first_line_on_screen->line_nb);
    old_line_nb = 0;
    while (s != NULL) {
        while (l->line_nb < s->l)
            l = l->next;
        if (!line_op || s->l > old_line_nb) {
            process(l, s);
        }
        old_line_nb = s->l;
        s = s->next;
    }
    
    has_been_changes = 1;
}

void
lower(struct line *l, struct selection *s)
{
    // set characters to lowercase

    int i, k, len;
    char c;

    for (k = get_str_index(l, i = s->x); i < s->x + s->n; i++) {
        len = utf8_char_length(c = l->chars[k]);
        if (len == 1 && 'A' <= c && c <= 'Z')
            l->chars[k] |= (1 << 5);
        if (len == 2 && c == (char) 0xc3)
            l->chars[k+1] |= (1 << 5);
        k += len;
    }
}

void
upper(struct line *l, struct selection *s)
{
    // set characters to uppercase

    int i, k, len;
    char c;

    for (k = get_str_index(l, i = s->x); i < s->x + s->n; i++) {
        len = utf8_char_length(c = l->chars[k]);
        if (len == 1 && 'a' <= c && c <= 'z')
            l->chars[k] &= ~(1 << 5);
        if (len == 2 && c == (char) 0xc3)
            l->chars[k+1] &= ~(1 << 5);
        k += len;
    }
}
 
void
insert(struct line *l, struct selection *s)
{
    // insert ev.ch at the beginning of selections

    int k, j, len;
    uint32_t c;

    len = unicode_char_length(c = ev.ch);
    k = insert_characters(l, s, s->x, 1, len);
    for (j = len - 1; j > 0; j--) {
        l->chars[k + j] = (c & 0x3f) | 0x80;
        c >>= 6;
    }
    l->chars[k] = c | utf8_start[len - 1];
}

void
indent(struct line *l, struct selection *s)
{
    // indent, until finding a settings.tab_width multiple in insert_mode,
    // else of up to asked_indent

    int i, j, k, start;

    // ignores empty lines
    if (l->ml == 1)
        return;

    if (asked_indent > 0) {
        // calibrate
        if (in_insert_mode) {
            asked_indent = 1;
            while ((s->x + asked_indent)%(settings.tab_width))
                asked_indent++;
            start = s->x;
        } else {
            start = 0;
        }

        // insert indent
        k = insert_characters(l, s, start, asked_indent, asked_indent);
        for (j = 0; j < asked_indent; j++)
            l->chars[k + j] = ' ';
    } else {
        // calibrate
        if (in_insert_mode) {
            asked_indent = -1;
            while ((s->x + asked_indent > 0) &&
                (l->chars[get_str_index(l, s->x + asked_indent - 1)] == ' ') &&
                (s->x + asked_indent)%(settings.tab_width))
                asked_indent--;
            start = s->x + asked_indent;
            if (start < 0)
                return;
            asked_indent = -asked_indent;
        } else {
            for (k = 0; k < (-asked_indent) && l->chars[k] == ' '; k++)
                ;
            asked_indent = k;
            start = 0;
        }

        // suppress characters
        delete_characters(l, s, start, asked_indent);
    }
}

void
comment(struct line *l, struct selection *s)
{
    // [un]comment the line according to language (assume syntax is defined)

    int k, kp, syntax_length;
    char *comment_syntax = *((settings.syntax)->comment);

    // detect first character of the line, ignores empty lines
    for (k = 0; l->chars[k] == ' '; k++)
        ;
    if (l->chars[k] == '\0')
        return;

    syntax_length = strlen(comment_syntax);
    if (is_in(comment_syntax, l->chars, k, syntax_length - 1)) {
        // uncomment
        delete_characters(l, s, k, syntax_length);
    } else {
        // comment
        k = insert_characters(l, s, k, syntax_length, syntax_length);
        for (kp = 0; kp < syntax_length; kp++)
            l->chars[k + kp] = comment_syntax[kp];
    }
}

void
suppress(struct line *l, struct selection *s)
{
    // if s->n, supress selected characters, else use asked_remove

    int start, nb_deleted;

    // calibrate removal
    if (s->n) {
        start = s->x;
        nb_deleted = s->n;
        s->x += nb_deleted; // to compensate last shift
        s->n = 0;
    } else {
        if (asked_remove > 0) {
            start = s->x;
            nb_deleted = asked_remove;
            if (nb_deleted > l->dl - start)
                nb_deleted = l->dl - start;
        } else {
            start = s->x + asked_remove;
            if (start < 0)
                start = 0;
            nb_deleted = s->x - start;
        }
        if (nb_deleted == 0)
            return;
    }

    // delete characters
    delete_characters(l, s, start, nb_deleted);
}

void
replace(struct line *l, struct selection *s)
{
    // replace the selection according to search and replace patterns

}

//void
//split_lines(struct line *l, struct selection *s)
//{
//    struct line *new;
//    char *new_chars, *old_chars;
//    int i, k;
//
//    // get memory index of split
//    k = get_str_index(l, s->x);
//
//    // shorten string
//    new_chars = (char *) malloc(k + 1);
//    for (i = 0; i < k; i++)
//        new_chars[i] = l->chars[i];
//    new_chars[k] = '\0';
//
//    // create new line below
//    new = insert_line(l->line_nb + 1, l->ml - k);
//    for (i = k; i < l->ml; i++)
//        new->chars[i - k] = l->chars[i];
//
//    // refresh metadata, free old string
//    old_chars = l->chars;
//    l->chars = new_chars;
//    free(old_chars);
//    l->ml = k + 1;
//    new->dl = l->dl - s->x; 
//    l->dl = s->x;
//
//    // TODO: shift selections
//}
