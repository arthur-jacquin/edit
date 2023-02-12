struct selection *
create_sel(int l, int x, int n, struct selection *next)
{
    // create a selection with given fields

    struct selection *res;

    res = (struct selection *) malloc(sizeof(struct selection));
    res->l = l;
    res->x = x;
    res->n = n;
    res->next = next;

    return res;
}

int
is_inf(struct pos p1, struct pos p2)
{
    // 1 if p1 < p2, else 0

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

    res.l = first_line_on_screen->line_nb + y;
    res.x = x;

    return res;
}

int
index_closest_after_nb(struct selection *a)
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

/*int
get_sel(struct selection *a, int index, struct pos *p)
{
    // try to extract position of corresponding selection, 0 if not exist

}*/

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
shift_sel(struct selection *a, struct pos starting, struct pos ending, struct selection delta)
{
    // shift selections of list a between starting and ending (exluded)
    // ignores delta.x and delta.n if delta.l not null
    
    struct pos cursor;

    cursor = pos_of_cursor();

    while (a != NULL && !is_inf(starting, pos_of_sel(a)))
        a = a->next;

    while (a != NULL && is_inf(pos_of_sel(a), ending)) {
        if (delta.l) {
            a->l += delta.l;
        } else {
            a->x += delta.x;
            a->n += delta.n;
        }
        a = a->next;
    }
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

    int i;
    struct line *l;
    struct selection *res, *last, *new;

    l = get_line(start - first_line_on_screen->line_nb);
    res = last = NULL;
    for (i = start; i <= end; i++) {
        new = create_sel(i, 0, l->dl, next);
        if (last == NULL) {
            res = last = new;
        } else {
            last->next = new;
            last = new;
        }
        l = l->next;
    }

    return res;
}

struct selection *
running_sel(void)
{
    // create selections corresponding to characters between anchor and cursor

    struct pos cursor, begin, end;
    struct selection *res, *begin_sel, *medium_sel, *end_sel;
    int line_delta;

    cursor = pos_of_cursor();
    res = NULL;
    if (anchored) {
        line_delta = cursor.l - anchor.l;
        if (line_delta) {
            if (line_delta > 0) {
                begin = anchor;
                end = cursor;
            } else {
                begin = cursor;
                end = anchor;
            }
            end_sel = create_sel(end.l, 0, end.x, NULL);
            medium_sel = (begin.l + 1 > end.l - 1) ? end_sel : 
                range_lines_sel(begin.l + 1, end.l - 1, end_sel);
            res = create_sel(begin.l, begin.x, get_line(begin.l -
                first_line_on_screen->line_nb)->dl - begin.x, medium_sel);
        } else {
            res = create_sel(cursor.l, anchor.x, cursor.x - anchor.x, NULL);
            if (res->n < 0) {
                res->x = cursor.x;
                res->n *= -1;
            }
        }
    } else {
        res = create_sel(cursor.l, cursor.x, 0, NULL);
    }

    return res;
}

void
search(struct selection *a)
{
    // TODO
}
