// see LICENSE file for copyright and license details

#define DECLARE_BRACKETS(A, B) \
    case (A): goal = (B); e = 1; break; \
    case (B): goal = (A); e = -1; break;

int
move(struct line **l, int *dx, int sens)
{
    // try to move by one in given direction, return 1 on success

    if (sens > 0) {
        if (*dx + 1 <= (*l)->dl) {
            (*dx)++;
        } else if (is_last_line(*l)) {
            return 0;
        } else {
            *l = (*l)->next;
            *dx = 0;
        }
    } else {
        if (*dx > 0) {
            (*dx)--;
        } else if (is_first_line(*l)) {
            return 0;
        } else {
            *l = (*l)->prev;
            *dx = (*l)->dl;
        }
    }

    return 1;
}

struct pos
pos_of(int l, int x)
{
    // create a position from coordinates

    struct pos res;

    res.l = l;
    res.x = x;

    return res;
}

void
unwrap_pos(struct pos p)
{
    // set cursor to pos p

    y = p.l - first_line_nb;
    x = p.x; attribute_x = 1;
}

int
find_first_non_blank(void)
{
    // find the first non blank character on the cursor line

    struct line *l;
    int i, nx;

    l = get_line(y);
    for (i = nx = l->ml - 1; i >= 0; i--)
        if (l->chars[i] != ' ')
            nx = i;

    return nx;
}

struct pos
find_start_of_word(int n)
{
    // find the <n>-th next beginning of word (end of word if n < 0)

    struct line *l;
    int dx, e;

    l = get_line(y);
    dx = x;
    e = (n > 0) ? 1 : -1;
    n *= e;

    while (n--) {
        while (move(&l, &dx, e))
            if (!is_word_char(l->chars[get_str_index(l->chars, dx)]))
                break;
        while (move(&l, &dx, e))
            if (is_word_char(l->chars[get_str_index(l->chars, dx)]))
                break;
    }
    if (e == -1) {
        while (move(&l, &dx, e)) {
            if (!is_word_char(l->chars[get_str_index(l->chars, dx)])) {
                move(&l, &dx, -e);
                break;
            }
        }
    }

    return pos_of(l->line_nb, dx);
}

struct pos
find_matching_bracket(void)
{
    // find position of bracket matching the one below cursor

    struct line *l;
    int dx, e;
    char c, goal, nc;               // initial bracket, associated one, new char
    int nb;                         // nesting depth

    l = get_line(y);
    dx = x;

    switch (c = l->chars[get_str_index(l->chars, dx)]) {
    DECLARE_BRACKETS('(', ')')
    DECLARE_BRACKETS('{', '}')
    DECLARE_BRACKETS('[', ']')
    DECLARE_BRACKETS('<', '>')
    default:
        return pos_of(l->line_nb, x);
    }

    nb = 1;
    while (nb && move(&l, &dx, e)) {
        nc = l->chars[get_str_index(l->chars, dx)];
        if (nc == goal)
            nb--;
        if (nc == c)
            nb++;
    }

    return pos_of(l->line_nb, dx);
}

struct pos
find_next_selection(int delta)
{
    // find position to move through saved selections
    // return null position if there is no selection in the asked direction

    int nb, closest, asked_number, last_strictly_before;

    // check the existence of selections
    if ((nb = nb_sel(saved)) == 0)
        return pos_of(0, 0);

    // calibrate asked_number
    closest = index_closest_after_cursor(saved);
    if (delta > 0) {
        if (closest == -1)
            return pos_of(0, 0); // none after
        asked_number = MIN(closest + delta - 1, nb - 1);
    } else {
        if (closest == 0)
            return pos_of(0, 0); // none before
        // get last position that verify p <= cursor
        last_strictly_before = ((closest == -1) ? nb : closest) - 1;
        if (!is_inf(get_pos_of_sel(saved, last_strictly_before),
            pos_of_cursor()))
            last_strictly_before--;
        if (last_strictly_before < 0)
            return pos_of(0, 0); // none strictly before
        asked_number = last_strictly_before + delta + 1;
        if (asked_number < 0)
            asked_number = 0;
    }

    return get_pos_of_sel(saved, asked_number);
}

int
find_block_delim(int starting_line_nb, int nb)
{
    // if nb < 0: find line number of first line of the nb-th block above
    // if nb > 0: find line number of first empty line below nb blocks

    struct line *l;

    l = get_line(starting_line_nb - first_line_nb);

    if (nb < 0) {
        while (nb++) {
            while (!is_first_line(l) && !(l->dl))
                l = l->prev;
            while (!is_first_line(l) && l->dl)
                l = l->prev;
        }
    } else {
        while (nb--) {
            while (!is_last_line(l) && !(l->dl))
                l = l->next;
            while (!is_last_line(l) && l->dl)
                l = l->next;
        }
    }

    return l->line_nb + ((nb == 1 && !(is_first_line(l) && l->dl)) ? 1 : 0);
}

void
move_to_cursor(void)
{
    // move to the closest possible position

    static int saved_x;             // if !attribute_x, this value defines x
    int nl, nx, delta;

    // adjust asked line number
    nl = MAX(first_line_nb + y, 1);
    nl = MIN(nl, nb_lines);

    // compute new first_line_on_screen and y
    delta = nl - first_line_nb;
    if (delta < scroll_offset)
        y = MIN(scroll_offset, nl - 1);
    else
        y = MIN(delta, screen_height - 2 - scroll_offset);
    first_line_on_screen = get_line(delta - y);

    // adjust x
#ifdef REMEMBER_CURSOR_COLUMN
    if (attribute_x)
        saved_x = x;
    x = saved_x;
#endif // REMEMBER_CURSOR_COLUMN
    attribute_x = 0;
    if (x > (nx = MIN(get_line(y)->dl, screen_width - 1 - LINE_NUMBERS_WIDTH)))
        x = nx;
    if (x < 0)
        x = 0;
}
