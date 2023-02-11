int
is_inf(struct selection *s1, struct selection *s2)
{
    // 1 if s1 < s2, else 0
    return (s1->l < s2->l || (s1->l == s2->l && s1->x < s2->x));
}

struct selection *
sel_of_pos(struct pos p, int temp)
{
    struct selection *res;

    res = (struct selection *) malloc(sizeof(struct selection));
    res->l = p.l;
    res->x = p.x;
    res->n = 0;
    res->temp = temp;
    res->next = NULL;

    return res;
}

struct pos
pos_of_curs(void)
{
    struct pos res;

    res.l = first_line_on_screen->line_nb + y;
    res.x = x;

    return res;
}

int
closest_after_nb(void)
{
    // only consider non temporary selections
    // TODO
}

struct pos
get_pos_sel(int nb)
{
    // only consider non temporary selections
    int i;
    struct selection *ptr;

    ptr = sel;
    while (ptr->temp)
        ptr = ptr->next;
    while (nb--) {
        ptr = ptr->next;
        while (ptr->temp)
            ptr = ptr->next;
    }

    return pos_of(ptr->l, ptr->x);
}

int
nb_sels(void)
{
    int nb;
    struct selection *ptr;

    if (sel != NULL) {
        nb = 1;
        ptr = sel;
        while (ptr->next != NULL) {
            ptr = ptr->next;
            nb++;
        }
        return nb;
    } else {
        return 0;
    }
}

void
empty_sels(void)
{
    struct selection *ptr;
    struct selection *old_ptr;

    if (sel != NULL) {
        ptr = sel;
        while (ptr->next != NULL) {
            old_ptr = ptr;
            ptr = ptr->next;
            free(old_ptr);
        }
        free(ptr);
    }

    sel = NULL;
}

void
merge_sels(struct selection *starting)
{
    // TODO: check for covering
    struct selection *new_sel;
    struct selection *old;
    struct selection *new;

    if (sel == NULL) {
        sel = starting;
    } else if (starting != NULL) {
        new_sel = NULL;
        old = sel;
        new = starting;

        if (is_inf(old, new)) {
            new_sel = sel = old;
            old = old->next;
        } else {
            new_sel = sel = new;
            new = new->next;
        }

        while (1) {
            if (new == NULL) {
                break;
            } else if (old == NULL) {
                break;
            } else if (is_inf(old, new)) {
                new_sel->next = old;
                old = old->next;
            } else {
                new_sel->next = new;
                new = new->next;
            }
            new_sel = new_sel->next;
        }

        if (new == NULL) {
            new_sel->next = old;
        } else {
            new_sel->next = new;
        }
    }
}

void
add_running_sels(int temp)
{
    struct pos cursor_pos;
    struct selection *anchor_sel;
    struct selection *cursor_sel;
    int line_delta;

    cursor_pos = pos_of_curs();

    if (anchored) {
        anchor_sel = sel_of_pos(anchor, temp);
        cursor_sel = sel_of_pos(cursor_pos, temp);

        line_delta = cursor_pos.l - anchor.l;

        if (line_delta < 0) {
            if (line_delta < -1)
                add_range_sels(cursor_pos.l + 1, anchor.l - 1, temp);
            cursor_sel->n = get_line(y)->dl - x;
            cursor_sel->next = anchor_sel;
            anchor_sel->n = anchor.x;
            anchor_sel->x = 0;
            merge_sels(cursor_sel);
        } else if (line_delta > 0) {
            if (line_delta > 1)
                add_range_sels(anchor.l + 1, cursor_pos.l - 1, temp);
            anchor_sel->n = get_line(anchor.l -
                first_line_on_screen->line_nb)->dl - anchor.x;
            anchor_sel->next = cursor_sel;
            cursor_sel->n = x;
            cursor_sel->x = 0;
            merge_sels(anchor_sel);
        } else {
            if (cursor_pos.x <= anchor.x) {
                cursor_sel->n = anchor.x - cursor_pos.x;
                free(anchor_sel);
                merge_sels(cursor_sel);
            } else {
                anchor_sel->n = cursor_pos.x - anchor.x;
                free(cursor_sel);
                merge_sels(anchor_sel);
            }
        }
    } else {
        merge_sels(sel_of_pos(cursor_pos, temp));
    }
}

void
add_range_sels(int start, int end, int temp)
{
    int i;
    struct line *line;
    struct selection *new;
    struct selection *old;

    line = get_line(start - first_line_on_screen->line_nb);

    for (i = start; i <= end; i++) {
        new = (struct selection *) malloc(sizeof(struct selection));
        new->l = i;
        new->x = 0;
        new->n = line->dl;
        new->temp = temp;

        if (i == start) {
            sel = new;
        } else {
            old->next = new;
        }
        old = new;

        if (line->next != NULL)
            line = line->next;
    }
    new->next = NULL;
}

void
shift_sels(struct pos starting, struct pos ending, struct pos delta)
{
    struct selection *s;
    struct selection *start;
    struct selection *end;

    start = sel_of_pos(starting, 1); 
    end = sel_of_pos(ending, 1);

    s = sel;
    while (s) {
        if (is_inf(start, s) && is_inf(s, end)) {
            s->l += delta.l;
            s->x += delta.x;
        }
        s = s->next;
    }

    free(start);
    free(end);
}

void
delete_temp_sels(void)
{
    struct selection *old;
    struct selection *new;
    
    old = NULL;

    new = sel;
    while (new != NULL) {
        if (new->temp == 0) {
            if (old == NULL) {
                old = sel = new;
            } else {
                old->next = new;
                old = new;
            }
        }
        new = new->next;
    }
        
    if (old == NULL)
        sel = NULL;
}

void
search(void)
{
    // TODO
}
