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
    x = p.x;
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
    int sens, dx, current_type;
    char c, nc;

    l = get_line(y);
    dx = x;
    sens = (n > 0) ? 1 : -1;
    n *= sens;

    while (n--) {
        c = l->chars[get_str_index(l->chars, dx)];
        while (move(&l, &dx, sens)) {
            nc = l->chars[get_str_index(l->chars, dx)];
            if (c == ' ') {
                if (nc != ' ')
                    break;
            } else if (is_word_char(c)) {
                if (!is_word_char(nc))
                    break;
            } else if (isdigit(c)) {
                if (!isdigit(nc))
                    break;
            } else {
                if ((nc == ' ') || is_word_char(nc) || isdigit(nc))
                    break;
            }
        }
        while (l->chars[get_str_index(l->chars, dx)] == ' ')
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

    switch (c = l->chars[get_str_index(l->chars, dx)]) {
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
        nc = l->chars[get_str_index(l->chars, dx)]; // XXX: not great
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
        asked_number = closest + delta - 1;
        if (asked_number > nb - 1)
            asked_number = nb - 1;
    } else {
        if (closest == 0)
            return pos_of(0, 0); // none before
        // get last position that verify p <= cursor
        last_strictly_before = ((closest == -1) ? nb : closest) - 1;
        if (!is_inf(get_pos_of_sel(saved, last_strictly_before), pos_of_cursor()))
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
find_start_of_block(int starting_line_nb, int nb)
{
    // find line number of first line of the nb-th block above

    struct line *l;

    l = get_line(starting_line_nb - first_line_nb);

    while (nb--) {
        while (!is_first_line(l) && l->ml == 1)
            l = l->prev;
        while (!is_first_line(l) && l->ml > 1)
            l = l->prev;
    }

    return l->line_nb + ((is_first_line(l) && l->ml > 1) ? 0 : 1);
}

int
find_end_of_block(int starting_line_nb, int nb)
{
    // find line number of first empty line below nb blocks

    struct line *l;

    l = get_line(starting_line_nb - first_line_nb);

    while (nb--) {
        while (!is_last_line(l) && l->ml == 1)
            l = l->next;
        while (!is_last_line(l) && l->ml > 1)
            l = l->next;
    }

    return l->line_nb;
}

void
move_to_cursor(void)
{
    // move to the closest possible position

    int nl, nx, delta;

    // identify ideal coordinates
    nl = first_line_nb + y;
    nx = x;

    // adjust asked line number
    if (nl > nb_lines)
        nl = nb_lines;
    if (nl < 1)
        nl = 1;

    // compute new first_line_on_screen and y
    delta = nl - first_line_nb;
    if (delta < scrolloff)
        y = MIN(scrolloff, nl - 1);
    else
        y = MIN(delta, screen_height - 2 - scrolloff);
    first_line_on_screen = get_line(delta - y);

    // adjust x
    if (x > (nx = get_line(y)->dl))
        x = nx;
    if (x < 0)
        x = 0;
}
