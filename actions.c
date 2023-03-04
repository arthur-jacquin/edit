void
act(void (*process)(struct line *, struct selection *), int line_op)
{
    // execute process on saved selections if any, else on running selections
    // line_op can be set to 1 so that process is executed at most once per line

    struct selection *s;
    struct line *l;
    int old_line_nb;

    s = (saved != NULL) ? saved : running;
    l = get_line(s->l - first_line_nb);
    old_line_nb = 0;
    has_been_changes = 1;
    while (s != NULL) {
        while (l->line_nb < s->l)
            l = l->next;
        if (!line_op || s->l > old_line_nb)
            process(l, s);
        old_line_nb = s->l;
        s = s->next;
    }
}

void
lower(struct line *l, struct selection *s)
{
    // set characters to lowercase

    int i, k, len;
    char c;

    for (k = get_str_index(l->chars, i = s->x); i < s->x + s->n; i++) {
        len = utf8_char_length(c = l->chars[k]);
        if (len == 1 && 'A' <= c && c <= 'Z')
            l->chars[k] |= (1 << 5);
        if (len == 2 && c == 0xc3)
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

    for (k = get_str_index(l->chars, i = s->x); i < s->x + s->n; i++) {
        len = utf8_char_length(c = l->chars[k]);
        if (len == 1 && 'a' <= c && c <= 'z')
            l->chars[k] &= ~(1 << 5);
        if (len == 2 && c == 0xc3)
            l->chars[k+1] &= ~(1 << 5);
        k += len;
    }
}

void
insert(struct line *l, struct selection *s)
{
    // insert ev.ch at the beginning of selections

    int k, len;

    len = unicode_char_length(ev.ch);
    k = replace_chars(l, s, s->x, 0, 1, len);
    insert_utf8(l->chars, k, len, ev.ch);
}

void
split(struct line *l, struct selection *s)
{
    // split line at s->x

    break_line(l, s, s->x);
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
        k = replace_chars(l, s, start, 0, asked_indent, asked_indent);
        for (j = 0; j < asked_indent; j++)
            l->chars[k + j] = ' ';
    } else {
        // calibrate
        if (in_insert_mode) {
            asked_indent = -1;
            while ((s->x + asked_indent > 0) &&
                (l->chars[get_str_index(l->chars, s->x + asked_indent - 1)] == ' ') &&
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
        replace_chars(l, s, start, asked_indent, 0, 0);
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
    if (is_in(comment_syntax, l->chars, k, syntax_length - 1)) { // uncomment
        replace_chars(l, s, k, syntax_length, 0, 0);
    } else { // comment
        k = replace_chars(l, s, k, 0, syntax_length, syntax_length);
        strncpy(&(l->chars[k]), comment_syntax, syntax_length);
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
            if (nb_deleted == 0) {
                if (!is_last_line(l))
                    concatenate_line(l, s);
                return;
            }
        } else {
            start = s->x + asked_remove;
            if (start < 0)
                start = 0;
            nb_deleted = s->x - start;
            if (nb_deleted == 0) {
                if (!is_first_line(l))
                    concatenate_line(l->prev, s);
                return;
            }
        }
    }

    // delete characters
    replace_chars(l, s, start, nb_deleted, 0, 0);
}

void
replace(struct line *l, struct selection *s)
{
    // replace the selection according to search and replace patterns

    char *rp, *replaced, *new_replaced, *src; // replace pattern, replaced string
    int k, k_chars; // index in rp, chars (bytes)
    int lrp; // length of rp
    int j, lj; // index in replaced (characters, bytes)
    int lr; // size of replaced buffer
    int n, mst, mn; // substring to append to replaced
    struct substring *class; // pointing to either fields or subpatterns

    // search for fields and subpatterns
    mark_fields(l->chars, s->x, s->n);
    mark_pattern(l->chars, s->x, s->n);

    // malloc then populate replaced, adjust its length dynamically
    lrp = strlen(rp = replace_pattern.current);
    replaced = (char *) malloc(lr = DEFAULT_BUF_SIZE);
    j = lj = 0;
    for (k = 0; k < lrp;) {
        if ((k < lrp - 1) && (rp[k] == '\\' || rp[k] == '$') &&
            (isdigit(rp[k+1]))) { // field or subpattern
            class = (rp[k] == '$') ? fields : subpatterns;
            src = l->chars;
            n = class[rp[k+1] - '0'].n;
            mst = class[rp[k+1] - '0'].mst;
            mn = class[rp[k+1] - '0'].mn;
            k += 2;
        } else {
            if (k < lrp - 1 && rp[k] == '\\') // escaped character
                k++;
            src = rp;
            n = 1;
            mst = k;
            mn = utf8_char_length(rp[k]);
            k += mn;
        }

        // manage replaced length
        if (lj + mn >= lr) {
            while (lj + mn >= lr)
                lr <<= 1;
            new_replaced = (char *) malloc(lr);
            strncpy(new_replaced, replaced, lj);
            free(replaced);
            replaced = new_replaced;
        }

        // append substring
        strncpy(&(replaced[lj]), &(src[mst]), mn);
        lj += mn;
        j += n;
    }

    // do the actual replacement
    k_chars = replace_chars(l, s, s->x, s->n, j, lj);
    strncpy(&(l->chars[k_chars]), replaced, lj);

    // forget about replaced
    free(replaced);
}
