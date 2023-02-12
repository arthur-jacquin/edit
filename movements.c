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

struct pos
find_first_non_blank(void)
{
    // find the first non blank character on the cursor line

    struct line *l;
    int i, x;

    l = get_line(y);
    for (i = x = l->ml - 1; i >= 0; i--)
        if (!(is_blank(l->chars[i])))
            x = i;

    return pos_of(l->line_nb, x);
}

struct pos
find_start_of_word(int n)
{
    // find the <n>-th next beginning of word (end of word if n < 0)

    struct line *l;
    int sens, dx, current_type;
    char c, nc;

    l = get_line(y);
    dx = x;
    sens = (n > 0) ? 1 : -1;
    n *= sens;

    while (n--) {
        c = l->chars[get_str_index(l, dx)];
        while (move(&l, &dx, sens)) {
            nc = l->chars[get_str_index(l, dx)];
            if (is_blank(c)) {
                if (!is_blank(nc))
                    break;
            } else if (is_word_char(c)) {
                if (!is_word_char(nc))
                    break;
            } else if (is_digit(c)) {
                if (!is_digit(nc))
                    break;
            } else {
                if (is_blank(nc) || is_word_char(nc) || is_digit(nc))
                    break;
            }
        }
        while (is_blank(l->chars[get_str_index(l, dx)]))
            if (!move(&l, &dx, sens))
                break;
    }

    return pos_of(l->line_nb, dx);
}

struct pos
find_matching_bracket(void)
{
    // find position of bracket matching the one below cursor

    struct line *l;
    char c, goal, nc;
    int dx, sens, nb;

    l = get_line(y);
    dx = x;

    switch (c = l->chars[get_str_index(l, dx)]) {
    case '(': goal = ')'; sens = 1; break;
    case '{': goal = '}'; sens = 1; break;
    case '[': goal = ']'; sens = 1; break;
    case '<': goal = '>'; sens = 1; break;
    case ')': goal = '('; sens = -1; break;
    case '}': goal = '{'; sens = -1; break;
    case ']': goal = '['; sens = -1; break;
    case '>': goal = '<'; sens = -1; break;
    default:
        return pos_of(l->line_nb, x);
    }

    nb = 1;
    while (nb && move(&l, &dx, sens)) {
        nc = l->chars[get_str_index(l, dx)];
        if (nc == goal)
            nb--;
        if (nc == c)
            nb++;
    }

    return pos_of(l->line_nb, dx);
}
/*
struct pos
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
}
*/

int
find_start_of_block(int starting_line_nb, int nb)
{
    // find line number of first line of the nb-th block above

    struct line *l;

    l = get_line(starting_line_nb - first_line_on_screen->line_nb);

    while (nb--) {
        while (!is_first_line(l) && l->ml == 1)
            l = l->prev;
        while (!is_first_line(l) && l->ml > 1)
            l = l->prev;
    }

    return l->line_nb + 1;
}

int
find_end_of_block(int starting_line_nb, int nb)
{
    // find line number of first empty line below nb blocks

    struct line *l;

    l = get_line(starting_line_nb - first_line_on_screen->line_nb);

    while (nb--) {
        while (!is_last_line(l) && l->ml == 1)
            l = l->next;
        while (!is_last_line(l) && l->ml > 1)
            l = l->next;
    }

    return l->line_nb;
}

void
go_to(struct pos p)
{
    // move to the position closest possible to p

    int delta, n;

    // reach line
    if (p.l > nb_lines)
        p.l = nb_lines;
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
    if (p.x > (n = get_line(y)->dl))
        p.x = n;
    if (p.x < 0)
        p.x = 0;
    x = p.x;
}
