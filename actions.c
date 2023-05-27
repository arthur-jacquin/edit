// see LICENSE file for copyright and license details

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

    int i, k;
    char c;

    for (k = get_str_index(l->chars, i = s->x); i < s->x + s->n; i++) {
        if (('A' <= (c = l->chars[k]) && c <= 'Z') || c == (char) 0xc3)
            l->chars[k + ((c == (char) 0xc3) ? 1 : 0)] |= (1 << 5);
        k += utf8_char_length(c);
    }
}

void
upper(struct line *l, struct selection *s)
{
    // set characters to uppercase

    int i, k;
    char c;

    for (k = get_str_index(l->chars, i = s->x); i < s->x + s->n; i++) {
        if (('a' <= (c = l->chars[k]) && c <= 'z') || c == (char) 0xc3)
            l->chars[k + ((c == (char) 0xc3) ? 1 : 0)] &= ~(1 << 5);
        k += utf8_char_length(c);
    }
}

void
insert(struct line *l, struct selection *s)
{
    // insert to_insert at the beginning of selections

    int k, len;

    len = unicode_char_length(to_insert);
    k = replace_chars(l, s, s->x, 0, 1, len);
    insert_utf8(l->chars, k, len, to_insert);
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
    // indent until finding a settings.tab_width multiple

    int k, start, n;

    // ignores empty lines in normal mode
    if (!in_insert_mode && l->dl == 0)
        return;

    start = in_insert_mode ? s->x : find_first_non_blank(l);
    if (asked_indent > 0) {
        n = asked_indent*(settings.tab_width) - start%(settings.tab_width);
        k = replace_chars(l, s, start, 0, n, n);
        memset(&(l->chars[k]), ' ', n);
    } else {
        k = get_str_index(l->chars, start);
        for (n = 0; n < k && n < (-asked_indent - 1)*(settings.tab_width) + 1 +
            (start - 1)%(settings.tab_width) && l->chars[k - n - 1] == ' '; n++)
            ;
        replace_chars(l, s, start - n, n, 0, 0);
    }
}

void
comment(struct line *l, struct selection *s)
{
    // (un)comment the line according to language (assume syntax is defined)

    char *comment_syntax = *(settings.syntax->comment);
    int k, kp, syntax_length;

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
        s->x += nb_deleted; // compensate last shift
        s->n = 0;
    } else {
        if (asked_remove > 0) {
            start = s->x;
            nb_deleted = MIN(asked_remove, l->dl - start);
            if (nb_deleted == 0) {
                if (!is_last_line(l))
                    concatenate_line(l, s);
                return;
            }
        } else {
            start = MAX(s->x + asked_remove, 0);
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

    char *rp, *replaced, *new_replaced, *src;
    int k, k_chars;                 // indexes (bytes) in rp, chars
    int lrp;                        // length of rp
    int j, lj;                      // indexes (characters, bytes) in replaced
    int lr;                         // size of replaced buffer
    int mst, n, mn;                 // substring to append to replaced
    struct substring *class;        // pointer to either fields or subpatterns

    // search for fields and subpatterns
    mark_fields(l->chars, s->x, s->n);
    mark_subpatterns(l->chars, l->dl, s->x, s->x, s->n);

    // malloc then populate replaced, adjust its length dynamically
    lrp = strlen(rp = replace_pattern.current);
    replaced = (char *) _malloc(lr = DEFAULT_BUF_SIZE);
    j = lj = 0;
    for (k = 0; k < lrp;) {
        if ((k < lrp - 1) && (rp[k] == '\\' || rp[k] == '$') &&
            (isdigit(rp[k+1]))) { // field or subpattern
            class = (rp[k] == '$') ? fields : subpatterns;
            src = l->chars;
            mst = class[rp[k+1] - '0'].mst;
            n = class[rp[k+1] - '0'].n;
            mn = class[rp[k+1] - '0'].mn;
            k += 2;
        } else {
            if (k < lrp - 1 && rp[k] == '\\') // escaped character
                k++;
            src = rp;
            mst = k;
            n = 1;
            mn = utf8_char_length(rp[k]);
            k += mn;
        }

        // potentially resize replaced buffer
        if (lj + mn >= lr) {
            while (lj + mn >= lr)
                lr <<= 1;
            new_replaced = (char *) _malloc(lr);
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

#ifdef ENABLE_AUTOCOMPLETE
void
autocomplete(struct line *l, struct selection *s)
{
    // if selection end is after a word character, complete with the characters
    // that are common to all words that start identically and are strictly
    // longer

    struct line *sl;                // line pointer to go through the file
    int k;                          // index (bytes) in *sl
    int k1, k2, it, kt;             // indexes to delimit start of word in *l

    char *match, *tmp;              // pointer to string containing first match
    int ms, dl, ml, max_ml;         // start (bytes), length (characters, bytes)

    // delimit word before selection
    k1 = k2 = kt = get_str_index(l->chars, it = s->x + s->n);
    while (kt == k2 || is_word_char(l->chars[kt])) {
        k1 = kt;
        if (kt > 0)
            decrement(l->chars, &it, &kt, it - 1);
        else
            break;
    }
    if (k1 == k2)
        return;

    // search for potential completions
    sl = first_line;
    k = 0;
    match = NULL;
    while (sl != NULL) {
        // check if match
        if ((is_word_char(sl->chars[k]) && is_word_boundary(sl->chars, k)) &&
            ((k + k2 - k1) < sl->ml) &&
            (!strncmp(&(l->chars[k1]), &(sl->chars[k]), k2 - k1)) &&
            (is_word_char(sl->chars[k + k2 - k1]))) {
            k += k2 - k1;
            ml = dl = 0;
            while ((!match) ? (is_word_char(sl->chars[k + ml])) : (ml < max_ml
                && !compare_chars(match, ms + ml, sl->chars, k + ml))) {
                dl++; ml += utf8_char_length(sl->chars[k + ml]);
            }
            max_ml = ml;
            if (match == NULL) {
                match = sl->chars;
                ms = k;
            }
        }

        // move on
        if (sl->chars[k] == '\0') {
            sl = sl->next;
            k = 0;
        } else {
            k += utf8_char_length(sl->chars[k]);
        }
    }

    // replacement with longest possible substring
    if (match != NULL) {
        tmp = (char *) _malloc(ml);
        strncpy(tmp, &(match[ms]), ml);
        replace_chars(l, s, s->x + s->n, 0, dl, ml);
        strncpy(&(l->chars[k2]), tmp, ml);
        free(tmp);
    }
}
#endif // ENABLE_AUTOCOMPLETE
