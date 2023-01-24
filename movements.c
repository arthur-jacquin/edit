struct pos
pos_of(int l, int x)
{
    struct pos res;

    res.l = l;
    res.x = x;

    return res;
}

struct pos
find_first_non_blanck(void)
{
    int i, x;
    struct line *cursor_line;

    cursor_line = get_line(y);
    for (i = x = cursor_line->length - 1; i >= 0; i--)
        if (!(is_blank(cursor_line->chars[i])))
            x = i;

    return pos_of(cursor_line->line_nb, x);
}

struct pos
find_matching_bracket(void)
{
    // TODO

}

/*struct pos
find_next_selection(int delta)
{
    int asked_nb, closest, n;
    struct selection *cursor;
    struct pos res;
        
    if (sel == NULL) {
        return pos_of(0, 0);
    } else {
        cursor = (struct selection *) malloc(sizeof(struct selection));
        cursor->l = first_line_on_screen->line_nb + y;
        cursor->x = x;
        closest = closest_nb(cursor);
        n = nb_sels();

        if ((closest == 0 && delta < 0)
            || (closest == n && delta > 0)) { // n - 1 ?
            free(cursor);
            return pos_of(0, 0);
        } else {
            asked_nb = closest + delta - 1*(delta > 0);
            if (asked_nb < 0)
                asked_nb = 0;
            if (asked_nb >= n)
                asked_nb = n - 1;
            cursor = get_sel(asked_nb);
            res = pos_of(cursor->l, cursor->x);
            free(cursor);
            return res;
        }
    }
}*/

int
find_start_of_block(int starting_line_nb, int nb)
{
    int l;
    struct line *ptr;

    l = starting_line_nb;
    ptr = get_line(l - first_line_on_screen->line_nb);

    while (nb--) {
        while (ptr->length == 1) {
            if (ptr->prev != NULL) {
                ptr = ptr->prev;
                l--;
            } else {
                return l;
            }
        }
        while (ptr->length > 1) {
            if (ptr->prev != NULL) {
                ptr = ptr->prev;
                l--;
            } else {
                return l;
            }
        }
    }

    return l + 1; 
}

int
find_end_of_block(int starting_line_nb, int nb)
{
    int l;
    struct line *ptr;

    l = starting_line_nb;
    ptr = get_line(l - first_line_on_screen->line_nb);

    while (nb--) {
        while (ptr->length == 1) {
            if (ptr->next != NULL) {
                ptr = ptr->next;
                l++;
            } else {
                return l;
            }
        }
        while (ptr->length > 1) {
            if (ptr->next != NULL) {
                ptr = ptr->next;
                l++;
            } else {
                return l;
            }
        }
    }

    return l; 
}

void
go_to(struct pos p)
{
    int delta, n;

    // reach line
    if (p.l > nb_line)
        p.l = nb_line;
    if (p.l < 1)
        p.l = 1;

    delta = p.l - first_line_on_screen->line_nb;
    if (0 <= delta && delta < screen_height - 1) {
        y = delta;
    } else if (delta >= screen_height - 1) {
        first_line_on_screen = get_line(delta - (screen_height - 2));
        y = screen_height - 2;
    } else {
        first_line_on_screen = get_line(delta);
        y = 0;
    }

    // adjust x
    if (p.x >= (n = get_line(y)->length))
        p.x = n - 1;
    if (p.x < 0)
        p.x = 0;
    x = p.x;
}
