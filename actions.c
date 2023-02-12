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

//void
//insert(struct line *l, struct selection *s)
//{
//    char *new_chars, *old_chars;
//    int len, clr, i, j, k;
//    uint32_t c = ev.ch, first;
//
//    // compute length of inserted character
//    if (c < 0x80) {
//        len = 1;
//    } else if (c < 0x800) {
//        len = 2;
//    } else if (c < 0x10000) {
//        len = 3;
//    } else if (c < 0x200000) {
//        len = 4;
//    } else { // Unicode error
//        return;
//    }
//
//    // create new string
//    new_chars = (char *) malloc(l->ml + len);
//    insert_unicode(l->chars, new_chars, s->x, ev.ch);
//    old_chars = l->chars;
//    l->chars = new_chars;
//    free(old_chars);
//    l->ml += len;
//    l->dl++;
//
//    // TODO: shift x for selections of the same line
//}
//
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
//
//void
//suppress(struct line *l, struct selection *s)
//{
//    char *new_chars, *old_chars;
//    int i, e, k1, k2, start, len;
//
//    // calibrate removal
//    if (asked_remove > 0) {
//        e = 1;
//        if (s->x + asked_remove >= l->dl)
//            asked_remove = l->dl - 1 - s->x;
//    } else {
//        e = -1;
//        if (s->x + asked_remove < 0)
//            asked_remove = -s->x;
//    }
//
//    if (asked_remove) {
//        // delimits bytes to suppress
//        start = (e == 1) ? (s->x) : (s->x + asked_remove); 
//        for (k1 = 0, i = 0; i < start; i++)
//            k1 += utf8_char_length(l->chars[k1]);
//        for (k2 = k1, i = 0; i < e*asked_remove; i++)
//            k2 += utf8_char_length(l->chars[k2]);
//
//        // create new string
//        new_chars = (char *) malloc(l->ml - (k2 - k1));
//        for (i = 0; i < k1; i++)
//            new_chars[i] = l->chars[i];
//        for (i = k2; i < l->ml; i++)
//            new_chars[i - (k2 - k1)] = l->chars[i];
//
//        // refresh metadata, free old string
//        old_chars = l->chars;
//        l->chars = new_chars;
//        free(old_chars);
//        l->ml -= k2 - k1;
//        l->dl -= e*asked_remove;
//        
//        // TODO: shift x for selections of the same line
//
//    } else if ((e == 1 && l->next != NULL) || (e == -1 && l->prev != NULL)) {
//        // get on first line to concatenate with the next
//        if (e == -1)
//            l = l->prev;
//
//        // concatenate next line at the end
//        new_chars = (char *) malloc(l->ml + (l->next)->ml - 1);
//        for (i = 0; i < l->ml - 1; i++)
//            new_chars[i] = l->chars[i];
//        for (i = 0; i < (l->next)->ml; i++)
//            new_chars[i + (l->ml - 1)] = (l->next)->chars[i];
//
//        // refresh metadata, free old string
//        l->ml += (l->next)->ml - 1;
//        l->dl += (l->next)->dl;
//        old_chars = l->chars;
//        l->chars = new_chars;
//        free(old_chars);
//
//        // link lines, refresh line numbers, free next line
//        if ((l->next)->next != NULL) {
//            link_lines(l, (l->next)->next);
//            nb_lines--;
//            shift_line_nb(l->line_nb + 1, nb_lines, -1);
//        } else {
//            l->next = NULL;
//        }
//        free((l->next)->chars);
//        free(l->next);
//
//        // TODO: shift selections
//    }
//}
//
//void
//indent(struct line *l, struct selection *s)
//{
//    char *new_chars, *old_chars;
//    int i, j, k, pos;
//
//    if (l->ml == 1)
//        return;
//
//    if (asked_indent > 0) {
//        if (in_insert_mode) {
//            for (k = 0, i = 0; i < s->x; i++)
//                k += utf8_char_length(l->chars[k]);
//            pos = k;
//            asked_indent = 1;
//            while ((s->x + asked_indent)%(settings.tab_width))
//                asked_indent++;
//        } else {
//            pos = 0;
//        }
//        new_chars = (char *) malloc(l->ml + asked_indent);
//        for (i = 0; i < pos; i++)
//            new_chars[i] = l->chars[i];
//        for (j = 0; j < asked_indent; j++)
//            new_chars[pos + j] = ' ';
//        for (i = pos; i < l->ml; i++)
//            new_chars[i + asked_indent] = l->chars[i];
//        l->ml += asked_indent;
//        l->dl += asked_indent;
//    } else {
//        if (in_insert_mode) {
//            for (k = 0, i = 0; i < s->x; i++)
//                k += utf8_char_length(l->chars[k]);
//            pos = k;
//            asked_indent = -1;
//            while ((s->x + asked_indent)%(settings.tab_width))
//                asked_indent--;
//        } else {
//            for (i = 0; i < (-asked_indent) && l->chars[i] == ' '; i++)
//                ;
//            asked_indent = i;
//            pos = 0;
//        }
//        new_chars = (char *) malloc(l->ml - asked_indent);
//        for (i = 0; i < pos; i++)
//            new_chars[i] = l->chars[i];
//        for (i = pos + asked_indent; i < l->ml; i++)
//            new_chars[i - asked_indent] = l->chars[i];
//        l->ml += asked_indent;
//        l->dl += asked_indent;
//    }
//    old_chars = l->chars;
//    l->chars = new_chars;
//    free(old_chars);
//
//    // TODO: shift selections
//}
//
//void
//comment(struct line *l, struct selection *s)
//{
//    // TODO: change selections length ?, and x
//    int d, i, j, syntax_length;
//    char *new_chars, *old_chars;
//    char *comment_syntax = *(syntax->comment);
//
//    if (l->ml == 1)
//        return;
//    
//    for (i = 0; i < l->ml && l->chars[i] == ' '; i++)
//        ;
//    if (i < l->ml) {
//        syntax_length = strlen(comment_syntax);
//
//        d = -1;
//        if (i + syntax_length >= l->ml) {
//            d = 1;
//        } else {
//            for (j = 0; j < syntax_length; j++)
//                if (l->chars[i + j] != comment_syntax[j])
//                    d = 1;
//        }
//        
//        new_chars = (char *) malloc(l->ml + d*syntax_length);
//        for (j = 0; j < i; j++)
//            new_chars[j] = ' ';
//        if (d == 1) {
//            for (j = 0; j < syntax_length; j++)
//                new_chars[i + j] = comment_syntax[j];
//            i += syntax_length;
//        }
//        for (; i < l->ml + d*syntax_length; i++)
//            new_chars[i] = l->chars[i - d*syntax_length];
//        
//        old_chars = l->chars;
//        l->chars = new_chars;
//        free(old_chars);
//        l->ml += d*syntax_length;
//        l->dl += d*syntax_length;
//    }
//}

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

//void
//replace(struct line *l, struct selection *s)
//{
//    // TODO
//}
