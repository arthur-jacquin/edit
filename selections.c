// see LICENSE file for copyright and license details

struct selection *
create_sel(int l, int x, int n, struct selection *next)
{
    // create a selection with given fields

    struct selection *res;

    res = (struct selection *) _malloc(sizeof(struct selection));
    res->l = l;
    res->x = x;
    res->n = n;
    res->next = next;

    return res;
}

int
nb_sel(struct selection *a)
{
    // compute the number of selections in the list a

    int res;

    for (res = 0; a != NULL; res++)
        a = a->next;

    return res;
}

void
forget_sel_list(struct selection *a)
{
    // free the memory used by the list a

    struct selection *next;

    while (a != NULL) {
        next = a->next;
        free(a);
        a = next;
    }
}

void
reset_selections(void)
{
    // reset selections

    forget_sel_list(saved);
    saved = NULL;
    anchored = 0;
}

int
is_inf(struct pos p1, struct pos p2)
{
    // return 1 if p1 < p2, else 0

    return (p1.l < p2.l) || ((p1.l == p2.l) && (p1.x < p2.x));
}

struct pos
pos_of_sel(struct selection *s)
{
    // create a selection from a position, to be compared using is_inf

    struct pos res;

    res.l = s->l;
    res.x = s->x;

    return res;
}

struct pos
pos_of_cursor(void)
{
    // create a selection corresponding to cursor

    struct pos res;

    res.l = first_line_nb + y;
    res.x = x;

    return res;
}

int
index_closest_after_cursor(struct selection *a)
{
    // find index of the first selection > than cursor (-1 if does not exist)

    int res;
    struct selection *s;
    struct pos cursor;

    res = 0;
    cursor = pos_of_cursor();

    while (1) {
        if (a == NULL)
            return -1;
        if (is_inf(cursor, pos_of_sel(a)))
            return res;
        a = a->next;
        res++;
    }
}

struct pos
get_pos_of_sel(struct selection *a, int index)
{
    // extract the position of corresponding selection, NULL if does not exist

    int count;

    for (count = 0; count < index; count++) {
        if (a == NULL)
            return pos_of(0, 0);
        else
            a = a->next;
    }

    return pos_of(a->l, a->x);
}

int
column_sel(int n)
{
    // append running selection and duplicates on followning n-1 lines to saved
    // return number of selections created
    // will not work if running selection is multiline

    struct line *l;
    struct selection *last, *tmp;
    struct pos cursor;
    int i, delta, wx, wn;

    // calibrate number of selections and portion of the lines [wx; wx + wn[
    cursor = pos_of_cursor();
    if (anchored && anchor.l != cursor.l)
        return 0;
    if (cursor.l + n - 1 > nb_lines)
        n = nb_lines - cursor.l + 1;
    if (anchored) {
        delta = cursor.x - anchor.x;
        wx = (delta > 0) ? anchor.x : cursor.x;
        wn = (delta > 0) ? delta : -delta;
    } else {
        wx = x;
        wn = 0;
    }

    // extract selections
    last = NULL;
    l = get_line(y + n - 1);
    for (i = 0; i < n; i++) {
        if (l->dl >= wx)
            last = create_sel(l->line_nb, wx, MIN(wn, l->dl - wx), last);
        l = l->prev;
    }

    // save selections
    tmp = merge_sel(saved, last);
    forget_sel_list(saved);
    saved = tmp;

    // refresh anchor
    if (anchored) {
        if (cursor.l + n > nb_lines || anchor.x > get_line(y + n)->dl)
            anchored = 0;
        else
            anchor.l = cursor.l + n;
    }

    return n;
}

struct selection *
merge_sel(struct selection *a, struct selection *b)
{
    // create a list of selections containing elements from list a and b

    struct selection *start, *last, *to_add, *new;

    start = last = NULL;
    while (1) {
        if (a != NULL && b != NULL) {
            if (is_inf(pos_of_sel(a), pos_of_sel(b))) {
                to_add = a;
                a = a->next;
            } else {
                to_add = b;
                b = b->next;
            }
        } else if (a != NULL) {
            to_add = a;
            a = a->next;
        } else if (b != NULL) {
            to_add = b;
            b = b->next;
        } else {
            return start;
        }

        new = create_sel(to_add->l, to_add->x, to_add->n, NULL);
        if (last == NULL) {
            start = last = new;
        } else if (new->l == last->l && last->x + last->n > new->x) {
            // covering detected: forget about this one
            free(new);
        } else {
            last->next = new;
            last = new;
        }
    }
}

struct selection *
range_lines_sel(int start, int end, struct selection *next)
{
    // create a list of selections containing wholes lines of number between
    // start and end (assume 1 <= start <= end <= nb_lines)

    struct line *l;
    int i;

    l = get_line(end - first_line_nb);
    for (i = end; i >= start; i--) {
        next = create_sel(i, 0, l->dl, next);
        l = l->prev;
    }

    return next;
}

struct selection *
running_sel(void)
{
    // create selections corresponding to characters between anchor and cursor

    struct selection *medium_sel, *end_sel;
    struct pos cursor, begin, end;

    cursor = pos_of_cursor();
    if (anchored) {
        if (cursor.l != anchor.l) {
            begin = (cursor.l > anchor.l) ? anchor : cursor;
            end = (cursor.l > anchor.l) ? cursor : anchor;
            end_sel = create_sel(end.l, 0, end.x, NULL);
            medium_sel = (begin.l + 1 > end.l - 1) ? end_sel :
                range_lines_sel(begin.l + 1, end.l - 1, end_sel);
            return create_sel(begin.l, begin.x, get_line(begin.l -
                first_line_nb)->dl - begin.x, medium_sel);
        } else {
            return create_sel(cursor.l, MIN(cursor.x, anchor.x),
                ABS(cursor.x - anchor.x), NULL);
        }
    } else {
        return create_sel(cursor.l, cursor.x, 0, NULL);
    }
}

struct selection *
search(struct selection *a)
{
    // return a selections list of substrings contained in saved that matches
    // the search pattern (assume all selections are valid)

    struct selection *res, *last, *new;
    struct line *l;
    int k, len;

    // check the existence of selections
    if (a == NULL)
        return NULL;

    l = get_line(a->l - first_line_nb);
    res = last = NULL;
    while (a != NULL) {
        // get correct line
        while (l->line_nb < a->l)
            l = l->next;

        // try to match pattern at any starting point
        k = 0;
        while (k < a->n) {
            if (len = mark_subpatterns(l->chars, l->dl, a->x, a->x+k, a->n-k)) {
                new = create_sel(l->line_nb, a->x + k, len, NULL);
                if (last == NULL) {
                    res = last = new;
                } else {
                    last->next = new;
                    last = new;
                }
                k += len;
            } else {
                k++;
            }
        }
        a = a->next;
    }

    return res;
}

int
search_word_under_cursor(void)
{
    // search for the word under cursor padded with \b
    // return 1 on success

    struct line *l;
    int i, k;                       // indexes (characters, bytes) in l->chars
    int k1, k2;                     // delimiting the word in memory

    l = get_line(y);
    k = get_str_index(l->chars, i = x);

    if (!is_word_char(l->chars[k]))
        return 0;

    // delimit the word
    while (is_word_char(l->chars[k])) {
        k1 = k;
        if (k > 0)
            decrement(l->chars, &i, &k, i - 1);
        else
            break;
    }
    k2 = k1;
    while (is_word_char(l->chars[k2]))
        k2 += utf8_char_length(l->chars[k2]);

    // compute and search for the word
    if (k2 - k1 + 5 > INTERFACE_MEM_LENGTH)
        return 0; // word is too long to be searched
    search_pattern.current[0] = search_pattern.current[k2 - k1 + 2] = '\\';
    search_pattern.current[1] = search_pattern.current[k2 - k1 + 3] = 'b';
    search_pattern.current[k2 - k1 + 4] = '\0';
    strncpy(&(search_pattern.current[2]), &(l->chars[k1]), k2 - k1);
    strcpy(search_pattern.previous, search_pattern.current);
    forget_sel_list(displayed);
    displayed = search(saved);
    forget_sel_list(saved);
    saved = displayed;
    displayed = NULL;

    return 1;
}

void
shift_sel_line_nb(struct selection *a, int min, int max, int delta)
{
    // shift the l field of delta for selections of list a if the current
    // value is between min and max (included)
    // comparison with max is ignored if max == 0

    while (a != NULL && a->l < min)
        a = a->next;
    while (a != NULL && (!max || a->l <= max)) {
        a->l += delta;
        a = a->next;
    }

    // move anchor
    if (anchored && min <= anchor.l && (!max || anchor.l <= max))
        anchor.l += delta;
}

void
move_sel_end_of_line(struct selection *a, int l, int i, int concatenate)
{
    // if concatenate, move selections of line l to the previous line that is i
    // characters long, else move selections of line l that is i characters long
    // to next line

    int e;

    e = (concatenate) ? 1 : -1;
    while (a != NULL && (a->l < l || (a->l == l && !concatenate && a->x < i)))
        a = a->next;
    while (a != NULL && a->l == l) {
        a->l -= 1*e;
        a->x += i*e;
        a = a->next;
    }

    // move anchor
    if (anchored && anchor.l == l && (concatenate || anchor.x >= i)) {
        anchor.l -= 1*e;
        anchor.x += i*e;
    }

    // move cursor
    if (first_line_nb + y == l) {
        y -= 1*e;
        x += i*e; attribute_x = 1;
    } else if (first_line_nb + y > l) {
        y -= 1*e;
    }
}

void
remove_sel_line_range(int min, int max)
{
    // remove saved selections within specified line range

    struct selection *old, *next, *a;

    if (saved == NULL) {
        return;
    } else if (saved->l < min) {
        old = a = saved;
        while (a != NULL && a->l < min) {
            old = a;
            a = a->next;
        }
        while (a != NULL && a->l <= max) {
            next = a->next;
            free(a);
            a = next;
        }
        old->next = a;
    } else {
        a = saved;
        while (a != NULL && a->l <= max) {
            next = a->next;
            free(a);
            a = next;
        }
        saved = a;
    }

    // move anchor
    if (anchored && min <= anchor.l && anchor.l <= max)
        anchored = 0;
}

void
reorder_sel(int l, int nb, int new_l)
{
    // reorder selections to adjust to move_line

    struct selection *s, *last;
    struct selection *last_before, *first, *last_first, *second, *last_second;
    int first_start, first_end, second_end;

    // compute delimiters of ranges to invert
    first_start = MIN(l, new_l);
    first_end = l + ((l < new_l) ? nb : 0) - 1;
    second_end = MAX(l, new_l) + nb - 1;

    // move anchor
    if (anchored) {
        if (first_start <= anchor.l && anchor.l <= first_end) {
            anchor.l += ((l < new_l) ? (new_l - l) : nb);
        } else if (first_end < anchor.l && anchor.l <= second_end) {
            anchor.l -= ((l < new_l) ? nb : (l - new_l));
        }
    }

    // skip selections before ranges, identify last_before
    s = last = saved;
    if (s == NULL)
        return;
    if (s->l < first_start) {
        while (s != NULL && s->l < first_start) {
            last = s;
            s = s->next;
        }
        last_before = last;
        if (s == NULL)
            return;
    } else {
        last_before = NULL;
    }

    // shift selections of the first range, identify first and last_first
    if (s->l <= first_end) {
        first = s;
        while (s != NULL && s->l <= first_end) {
            s->l += (l < new_l) ? (new_l - l) : nb;
            last = s;
            s = s->next;
        }
        last_first = last;
        if (s == NULL)
            return;
    } else {
        first = last_first = NULL;
    }

    // shift selections of the second range, identify second and last_second
    if (s->l <= second_end) {
        second = s;
        while (s != NULL && s->l <= second_end) {
            s->l -= (l < new_l) ? nb : (l - new_l);
            last = s;
            s = s->next;
        }
        last_second = last;
    } else {
        return;
    }

    // last_before->next = second
    if (last_before == NULL) {
        saved = second;
    } else {
        last_before->next = second;
    }

    // last_second->next = first
    // last_first->next = s
    if (first == NULL) {
        last_second->next = s;
    } else {
        last_second->next = first;
        last_first->next = s;
    }
}
