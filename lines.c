int
is_first_line(const struct line *l)
{
    // check if *l is the first line of the list it belongs to

    return (l->prev == NULL);
}

int
is_last_line(const struct line *l)
{
    // check if *l is the last line of the list it belongs to

    return (l->next == NULL);
}

struct line *
create_line(int line_nb, int ml, int dl)
{
    // create a new line in the memory, returns a pointer to it
    // prev and next fields are initialised to NULL
    // chars content is left undefined

    struct line *res;

    res = (struct line *) malloc(sizeof(struct line));
    res->prev = res->next = NULL;
    res->line_nb = line_nb;
    res->ml = ml;
    res->dl = dl;
    res->chars = (char *) malloc(ml);
    res->chars[ml - 1] = '\0'; // TODO: check for simplification elsewhere

    return res;
}

struct line *
insert_line(int asked_line_nb, int ml, int dl)
{
    // insert a line with the line_nb closest possible to asked_line_nb

    struct line *replaced_line, *new;

    if (asked_line_nb >= nb_lines + 1) {
        new = create_line(nb_lines + 1, ml, dl);
        new->chars[ml - 1] = '\0';
        link_lines(get_line(nb_lines - first_line_on_screen->line_nb), new);
        link_lines(new, NULL);
    } else {
        replaced_line = get_line(asked_line_nb - first_line_on_screen->line_nb);
        shift_line_nb((asked_line_nb >= first_line_on_screen->line_nb) ?
            first_line_on_screen : first_line, asked_line_nb, 0, 1);
        new = create_line(asked_line_nb, ml, dl);
        new->chars[ml - 1] = '\0';
        link_lines(replaced_line->prev, new);
        link_lines(new, replaced_line);
    }

    nb_lines++;
    return new;
}

struct line *
get_line(int delta_from_first_line_on_screen)
{
    // returns the pointer to line shifted of delta_from_first_line_on_screen
    // from first_line_on_screen

    struct line *res;

    res = first_line_on_screen;
    if (delta_from_first_line_on_screen > 0) {
        while (res->next != NULL && delta_from_first_line_on_screen--)
            res = res->next;
    } else {
        while (res->prev != NULL && delta_from_first_line_on_screen++)
            res = res->prev;
    }

    return res;
}

void
forget_line(struct line *l)
{
    // free the memory used by the line *l

    free(l->chars);
    free(l);
}

void
forget_lines_list(struct line *start)
{
    // free the memory used by the list start

    struct line *l, *next;

    l = start;
    while (l != NULL) {
        next = l->next;
        forget_line(l);
        l = next;
    }
}

void
link_lines(struct line *l1, struct line *l2)
{
    // ensures *l2 follows *l1 in the double linked list of lines
    // manage NULL pointers

    if (l1 != NULL)
        l1->next = l2;
    if (l2 != NULL)
        l2->prev = l1;
}

void
shift_line_nb(struct line *start, int min, int max, int delta)
{
    // shift the line_nb field of delta for lines of start queue if the current
    // value is between min and max (included)
    // comparison with max is ignored if max == 0

    struct line *l;

    l = start;
    while (l != NULL) {
        if (max && l->line_nb > max)
            break;
        else if (l->line_nb >= min)
            l->line_nb += delta;
        l = l->next;
    }
}

int
move_line(int delta)
{
    // move cursor line, return new line_nb

    int new_line_nb;
    struct line *src, *dest;

    // compute new line number
    new_line_nb = first_line_on_screen->line_nb + y + delta;
    if (new_line_nb < 1)
        new_line_nb = 1;
    if (new_line_nb > nb_lines)
        new_line_nb = nb_lines;
    if (new_line_nb == first_line_on_screen->line_nb + y)
        return new_line_nb;

    src = get_line(y);
    dest = get_line(new_line_nb - first_line_on_screen->line_nb);

    if (delta > 0) {
        // TODO: better start for shift ?
        shift_line_nb(first_line, first_line_on_screen->line_nb + y + 1, new_line_nb, -1);
        if (src == first_line_on_screen)
            first_line_on_screen = first_line_on_screen->next;
        if (is_first_line(src))
            first_line = src->next;
        link_lines(src->prev, src->next);
        link_lines(src, dest->next);
        link_lines(dest, src);
    } else {
        // TODO: better start for shift ?
        shift_line_nb(first_line, new_line_nb, first_line_on_screen->line_nb + y - 1, 1);
        if (dest == first_line_on_screen)
            first_line_on_screen = src;
        if (is_first_line(dest))
            first_line = src;
        link_lines(src->prev, src->next);
        link_lines(dest->prev, src);
        link_lines(src, dest);
    }
    src->line_nb = new_line_nb;

    return new_line_nb;
}

void
copy_to_clip(int starting_line_nb, int nb)
{
    // copy nb lines to clipboard, starting at line number starting_line_nb

    int i;
    struct line *l, *cb_l, *old_cb_l;

    // empty clipboard
    forget_lines_list(clipboard.start);
    clipboard.start = NULL;

    // adjust number of lines
    if (starting_line_nb + nb - 1 > nb_lines)
        nb = nb_lines + 1 - starting_line_nb;
    clipboard.nb_lines = nb;

    l = get_line(starting_line_nb - first_line_on_screen->line_nb);
    cb_l = old_cb_l = NULL;
    for (i = 0; i < nb; i++) {
        cb_l = create_line(i, l->ml, l->dl);
        strcpy(cb_l->chars, l->chars);
        link_lines(old_cb_l, cb_l);
        if (i == 0)
            clipboard.start = cb_l;
        old_cb_l = cb_l;
        l = l->next;
    }
    link_lines(cb_l, NULL);
}

void
move_to_clip(int starting_line_nb, int nb)
{
    // copy nb lines to clipboard, starting at line number starting_line_nb

    struct line *starting, *ending;

    // empty clipboard
    forget_lines_list(clipboard.start);
    clipboard.start = NULL;

    // adjust number of lines
    if (starting_line_nb + nb - 1 > nb_lines)
        nb = nb_lines + 1 - starting_line_nb;

    starting = get_line(starting_line_nb - first_line_on_screen->line_nb);
    ending = get_line(starting_line_nb + nb-1 - first_line_on_screen->line_nb);

    if (is_last_line(ending)) {
        if (is_first_line(starting)) {
            // create empty line
            first_line = first_line_on_screen = create_line(1, 1, 0);
            first_line->prev = first_line->next = NULL;
            strcpy(first_line->chars, "");
            nb_lines = 1;
        }
    } else {
        if (is_first_line(starting))
            first_line = first_line_on_screen = ending->next;
        shift_line_nb(ending, ending->line_nb + 1, 0, -nb);
    }

    link_lines(starting->prev, ending->next);
    link_lines(NULL, starting);
    link_lines(ending, NULL);
    shift_line_nb(starting, 0, 0, -starting->line_nb);
    clipboard.start = starting;
    clipboard.nb_lines = nb;
    nb_lines -= nb;
}


void
insert_clip(struct line *starting_line, int below)
{
    // insert clipboard lines above or below starting line

    int i, first_inserted_line_nb;
    struct line *l, *before, *after;

    // check if clipboard is not empty
    if (clipboard.start == NULL)
        return;

    // get correct line numbers, move clipboard to main buffer
    first_inserted_line_nb = starting_line->line_nb + ((below) ? 1 : 0);
    shift_line_nb(clipboard.start, 0, 0, first_inserted_line_nb);
    shift_line_nb(starting_line, first_inserted_line_nb, 0, clipboard.nb_lines);
    l = clipboard.start;
    for (i = 1; i < clipboard.nb_lines; i++)
        l = l->next;
    before = (below) ? starting_line : starting_line->prev;
    after = (below) ? starting_line->next : starting_line;
    link_lines(before, clipboard.start);
    link_lines(l, after);
    if (!below && is_first_line(starting_line))
        first_line = clipboard.start;

    // refresh metadata
    nb_lines += clipboard.nb_lines;
    clipboard.start = NULL;

    // copy inserted lines to clipboard
    copy_to_clip(first_inserted_line_nb, clipboard.nb_lines);
}
