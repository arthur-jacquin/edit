void
act(void (*process)(struct line *, struct selection *), int line_op)
{
    struct selection *s;
    struct line *l;
    int old_line_nb;

    s = sel;
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
insert(struct line *l, struct selection *s)
{
    char *new_chars;
    char *old_chars;
    int i;

    new_chars = (char *) malloc(l->length + 1);
    for (i = 0; i < s->x; i++)
        new_chars[i] = l->chars[i];
    new_chars[s->x] = ev.ch;
    for (i = s->x; i < l->length; i++)
        new_chars[i+1] = l->chars[i];
    old_chars = l->chars;
    l->chars = new_chars;
    free(old_chars);
    l->length++;

    // TODO: shift x for selections of the same line
}

void
split_lines(struct line *l, struct selection *s)
{
    struct line *new;
    char *new_chars;
    char *old_chars;
    int i;
    
    new = insert_line(l->line_nb + 1, l->length - s->x);
    for (i = s->x; i < l->length; i++)
        new->chars[i-s->x] = l->chars[i];
    new_chars = (char *) malloc(s->x + 1);
    for (i = 0; i < s->x; i++)
        new_chars[i] = l->chars[i];
    new_chars[s->x] = '\0';
    old_chars = l->chars;
    l->chars = new_chars;
    free(old_chars);
    l->length = s->x + 1;

    // TODO: shift selections
}

void
suppress(struct line *l, struct selection *s)
{
    char *new_chars;
    char *old_chars;
    int i, e;

    if (asked_remove > 0) {
        e = 1;
        if (s->x + asked_remove >= l->length)
            asked_remove = l->length - 1 - s->x;
    } else {
        e = -1;
        if (s->x + asked_remove < 0)
            asked_remove = -s->x;
    }

    if (asked_remove) {
        new_chars = (char *) malloc(l->length - e*asked_remove);
        if (e == 1) {
            for (i = 0; i < s->x; i++)
                new_chars[i] = l->chars[i];
        } else {
            for (i = 0; i < s->x + asked_remove; i++)
                new_chars[i] = l->chars[i];
        }
        for (; i < l->length - e*asked_remove; i++)
            new_chars[i] = l->chars[i + e*asked_remove];
        old_chars = l->chars;
        l->chars = new_chars;
        free(old_chars);
        l->length -= e*asked_remove;
    } else if (e == 1 && l->next != NULL) {
        // TODO: concatenate lines
    } else if (e == -1 && l->prev != NULL) {
        // TODO: concatenate lines
    }

    // TODO: shift x for selections of the same line
}

void
concatenate_lines(struct line *l, struct selection *s)
{
    // TODO
}

void
indent(struct line *l, struct selection *s)
{
    // TODO: change selections length, and x
    int i, j, pos;
    char *new_chars;
    char *old_chars;

    if (l->length == 1)
        return;

    if (asked_indent > 0) {
        new_chars = (char *) malloc(l->length + asked_indent);
        pos = (in_insert_mode) ? (s->x) : 0;
        for (i = 0; i < pos; i++)
            new_chars[i] = l->chars[i];
        for (j = 0; j < asked_indent; j++)
            new_chars[pos+j] = ' ';
        for (i = pos; i < l->length; i++)
            new_chars[i+asked_indent] = l->chars[i];
        l->length += asked_indent;
    } else {
        for (i = 0; i < (-asked_indent) && l->chars[i] == ' '; i++)
            ;
        new_chars = (char *) malloc(l->length - i);
        for (j = 0; j < l->length - i; j++)
            new_chars[j] = l->chars[j + i];
        l->length -= i;
    }
    old_chars = l->chars;
    l->chars = new_chars;
    free(old_chars);
}

void
comment(struct line *l, struct selection *s)
{
    // TODO: change selections length ?, and x
    int d, i, j, syntax_length;
    char *new_chars;
    char *old_chars;
    char *comment_syntax = *(syntax->comment);

    if (l->length == 1)
        return;
    
    for (i = 0; i < l->length && l->chars[i] == ' '; i++)
        ;
    if (i < l->length) {
        syntax_length = strlen(comment_syntax);

        d = -1;
        for (j = 0; j < syntax_length; j++)
            if (i + j < l->length && l->chars[i+j] != comment_syntax[j])
                d = 1;
        
        new_chars = (char *) malloc(l->length + d * syntax_length);
        for (j = 0; j < i; j++)
            new_chars[j] = ' ';
        if (d == 1) {
            for (j = 0; j < syntax_length; j++)
                new_chars[i + j] = comment_syntax[j];
            for (j = i; j < l->length; j++)
                new_chars[j + syntax_length] = l->chars[j];
        } else {
            for (j = i; j < l->length - syntax_length; j++)
                new_chars[j] = l->chars[j + syntax_length];
        }
        
        old_chars = l->chars;
        l->chars = new_chars;
        free(old_chars);
        l->length += d * syntax_length;
    }
}

void
lower(struct line *l, struct selection *s)
{
    int i;
    char c;

    for (i = s->x; i < s->x + s->n; i++) {
        if ('A' <= (c = l->chars[i]) && c <= 'Z') {
            l->chars[i] += 'a' - 'A';
        }
    }
}

void
upper(struct line *l, struct selection *s)
{
    int i;
    char c;

    for (i = s->x; i < s->x + s->n; i++) {
        if ('a' <= (c = l->chars[i]) && c <= 'z') {
            l->chars[i] += 'A' - 'a';
        }
    }
}

void
replace(struct line *l, struct selection *s)
{
    // TODO
}
