// see LICENSE file for copyright and license details

static struct {
    struct line *start;
    int nb_lines;
} clipboard;

struct line *
get_line(int delta_from_first_line_on_screen)
{
    // return the pointer to line shifted of delta_from_first_line_on_screen
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

struct line *
create_line(int line_nb, int ml, int dl)
{
    // create a new line in the memory, returns a pointer to it
    // prev and next fields are initialised to NULL
    // chars content is left undefined, but NULL-terminated

    struct line *res;

    res = (struct line *) _malloc(sizeof(struct line));
    res->prev = res->next = NULL;
    res->line_nb = line_nb;
    res->ml = ml;
    res->dl = dl;
    res->chars = (char *) _malloc(ml);
    res->chars[ml - 1] = '\0';

    return res;
}

void
link_lines(struct line *l1, struct line *l2)
{
    // ensure *l2 follows *l1 in the double linked list of lines
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
    while (l != NULL && l->line_nb < min)
        l = l->next;
    while (l != NULL && (!max || l->line_nb <= max)) {
        l->line_nb += delta;
        l = l->next;
    }
}

void
forget_lines(struct line *start)
{
    // free the memory used by the list start

    struct line *l, *next;

    l = start;
    while (l != NULL) {
        next = l->next;
        free(l->chars);
        free(l);
        l = next;
    }
}

int
replace_chars(struct line *l, struct selection *a, int start, int n,
    int new_n, int nb_bytes)
{
    // replace n characters after start by new_n characters taking nb_bytes
    // bytes (characters are not initialised)
    // return index of first inserted byte in l->chars

    int k1, k2;
    char *new_chars, *old_chars;

    // compute length of new_chars
    k1 = get_str_index(l->chars, start);
    k2 = get_str_index(l->chars, start + n);
    l->ml += nb_bytes - (k2 - k1);
    l->dl += new_n - n;

    // create new string
    new_chars = (char *) _malloc(l->ml);
    strncpy(new_chars, l->chars, k1);
    strncpy(&(new_chars[k1+nb_bytes]), &(l->chars[k2]), l->ml - (k1+nb_bytes));

    // refresh metadata
    old_chars = l->chars;
    l->chars = new_chars;
    free(old_chars);

    // move selections
    while (a != NULL && a->l < l->line_nb)
        a = a->next;
    while (a != NULL && a->l == l->line_nb) {
        if (a->x < start) {
            if (a->x + a->n <= start) {
            } else if (a->x + a->n <= start + n) {
                a->n = start - a->x;
            } else {
                a->n += new_n - n;
            }
        } else if (a->x < start + n) {
            if (a->x + a->n <= start + n) {
                a->x = start;
                a->n = new_n;
            } else {
                a->n -= start + new_n - a->x + n;
                a->x = start + new_n;
            }
        } else {
            a->x += new_n - n;
        }
        a = a->next;
    }

    // move cursor and anchor
    if (l->line_nb == first_line_nb + y && start <= x) {
        x = (x < start + n) ? (start + new_n) : (x + new_n - n);
        attribute_x = 1;
    }
    if (anchored && l->line_nb == anchor.l && start <= anchor.x)
        anchor.x = (anchor.x < start + n) ? start : (anchor.x + new_n - n);

    return k1;
}

void
break_line(struct line *l, struct selection *s, int start)
{
    // break line l at start

    struct line *new;
    char *new_chars;
    int k;

    // move selections, anchor, line numbers
    shift_sel_line_nb(s, l->line_nb + 1, 0, 1);
    move_sel_end_of_line(s, l->line_nb, start, 0);
    shift_line_nb(l, l->line_nb + 1, 0, 1);

    // create line below
    k = get_str_index(l->chars, start);
    new = create_line(l->line_nb + 1, l->ml - k, l->dl - start);
    link_lines(new, l->next);
    link_lines(l, new);
    strncpy(new->chars, &(l->chars[k]), l->ml - k);
    nb_lines++;

    // shorten current line
    new_chars = (char *) _malloc(k + 1);
    strncpy(new_chars, l->chars, k);
    new_chars[k] = '\0';
    free(l->chars);
    l->chars = new_chars;
    l->dl = start;
    l->ml = k + 1;
}

void
concatenate_line(struct line *l, struct selection *s)
{
    // move l->next (existence assumed) content at the end of l

    struct line *next;
    char *new_chars;

    // move selections, anchor, cursor
    move_sel_end_of_line(s, l->line_nb + 1, l->dl, 1);
    shift_sel_line_nb(s, l->line_nb + 1, 0, -1);
    shift_line_nb(l, l->line_nb + 1, 0, -1);

    // create new chars, refresh metadata
    next = l->next;
    new_chars = (char *) _malloc(l->ml + next->ml - 1);
    strncpy(new_chars, l->chars, l->ml - 1);
    strncpy(&(new_chars[l->ml - 1]), next->chars, next->ml);
    free(l->chars);
    l->chars = new_chars;
    l->ml += next->ml - 1;
    l->dl += next->dl;
    link_lines(l, next->next);
    free(next->chars);
    free(next);
    nb_lines--;
}

void
insert_line(int line_nb, int ml, int dl)
{
    // insert a line with number line_nb
    // chars are left uninitialised

    struct line *replaced_line, *new;

    new = create_line(line_nb, ml, dl);
    if (line_nb == nb_lines + 1) {
        link_lines(get_line(nb_lines - first_line_nb), new);
        link_lines(new, NULL);
    } else {
        // shift line_nb for lines and selections
        replaced_line = get_line(line_nb - first_line_nb);
        shift_line_nb(replaced_line, line_nb, 0, 1);
        shift_sel_line_nb(saved, line_nb, 0, 1);

        // insert the new line
        link_lines(replaced_line->prev, new);
        link_lines(new, replaced_line);
        if (is_first_line(new))
            first_line = new;
        if (replaced_line == first_line_on_screen)
            first_line_on_screen = new;
    }
    nb_lines++;
    has_been_changes = 1;
}

void
move_line(int delta)
{
    // move running selected lines from delta

    struct line *start_l, *end_l, *l;
    int start, end, nb, new_line_nb;

    // compute new line number
    start = end = first_line_nb + y;
    if (anchored) {
        if (anchor.l < start)
            start = anchor.l;
        else
            end = anchor.l;
    }
    new_line_nb = MAX(start + delta, 1);
    new_line_nb = MIN(new_line_nb, nb_lines - (end - start));
    if (new_line_nb == start)
         return;

    // reorder selections, anchor
    reorder_sel(start, nb = end - start + 1, new_line_nb);

    // reorder lines
    start_l = get_line(start - first_line_nb);
    end_l = get_line(end - first_line_nb);
    if (delta > 0) {
        l = get_line(end + new_line_nb - start - first_line_nb);
        shift_line_nb(end_l, end + 1, end + new_line_nb - start, -nb);
        if (start_l == first_line_on_screen)
            first_line_on_screen = end_l->next;
        if (is_first_line(start_l))
            first_line = end_l->next;
        link_lines(start_l->prev, end_l->next);
        link_lines(end_l, l->next);
        link_lines(l, start_l);
        shift_line_nb(start_l, start, end, new_line_nb - start);
    } else {
        l = get_line(new_line_nb - first_line_nb);
        shift_line_nb(start_l, start, end, new_line_nb - start);
        if (l == first_line_on_screen)
            first_line_on_screen = start_l;
        if (is_first_line(l))
            first_line = start_l;
        link_lines(start_l->prev, end_l->next);
        link_lines(l->prev, start_l);
        link_lines(end_l, l);
        shift_line_nb(l, new_line_nb, start - 1, nb);
    }
    has_been_changes = 1;

    // move cursor
    y += new_line_nb - start;
}

void
empty_clip(int was_defined)
{
    // empty the clipboard

    if (was_defined)
        forget_lines(clipboard.start);
    clipboard.start = NULL;
}

void
copy_to_clip(int starting_line_nb, int nb)
{
    // copy nb lines to clipboard, starting at line number starting_line_nb

    struct line *l, *cb_l, *old_cb_l;
    int i;

    // adjust number of lines
    if (starting_line_nb + nb - 1 > nb_lines)
        nb = nb_lines + 1 - starting_line_nb;
    empty_clip(1);
    clipboard.nb_lines = nb;

    l = get_line(starting_line_nb - first_line_nb);
    cb_l = old_cb_l = NULL;
    for (i = 0; i < nb; i++) {
        cb_l = create_line(i, l->ml, l->dl);
        strncpy(cb_l->chars, l->chars, l->ml);
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
    // assume cursor line is a moved line

    struct line *starting, *ending;

    // adjust number of lines
    if (starting_line_nb + nb - 1 > nb_lines)
        nb = nb_lines + 1 - starting_line_nb;

    // delimit range to be moved to clip
    starting = get_line(starting_line_nb - first_line_nb);
    ending = get_line(starting_line_nb + nb - 1 - first_line_nb);
    shift_line_nb(ending, ending->line_nb + 1, 0, -nb);
    nb_lines -= nb;
    if (starting_line_nb > first_line_nb) {
        y = starting_line_nb - first_line_nb;
    } else {
        if (is_last_line(ending)) {
            if (is_first_line(starting)) {
                // create empty line
                first_line = first_line_on_screen = create_line(1, 1, 0);
                nb_lines = 1;
            } else {
                first_line_on_screen = starting->prev;
            }
        } else {
            first_line_on_screen = ending->next;
            if (is_first_line(starting))
                first_line = first_line_on_screen;
        }
        y = 0;
    }

    // move lines to clip
    link_lines(starting->prev, ending->next);
    link_lines(NULL, starting);
    link_lines(ending, NULL);
    shift_line_nb(starting, 0, 0, -starting->line_nb);
    empty_clip(1);
    clipboard.start = starting;
    clipboard.nb_lines = nb;
    has_been_changes = 1;

    // suppress and move selections
    remove_sel_line_range(starting_line_nb, starting_line_nb + nb - 1);
    shift_sel_line_nb(saved, starting_line_nb + nb, 0, -nb);

    // move cursor
    y = starting_line_nb - first_line_nb;
}

void
insert_clip(struct line *starting_line, int below)
{
    // insert clipboard lines above or below starting line

    struct line *l, *before, *after;
    int i, first_inserted_line_nb;

    // check if clipboard is not empty
    if (clipboard.start == NULL)
        return;

    // get correct line numbers, move clipboard to main buffer
    first_inserted_line_nb = starting_line->line_nb + ((below) ? 1 : 0);
    shift_line_nb(clipboard.start, 0, 0, first_inserted_line_nb);
    shift_line_nb(starting_line, first_inserted_line_nb, 0, clipboard.nb_lines);
    shift_sel_line_nb(saved, first_inserted_line_nb, 0, clipboard.nb_lines);
    l = clipboard.start;
    for (i = 1; i < clipboard.nb_lines; i++)
        l = l->next;
    before = (below) ? starting_line : starting_line->prev;
    after = (below) ? starting_line->next : starting_line;
    link_lines(before, clipboard.start);
    link_lines(l, after);
    if (before == NULL)
        first_line = clipboard.start;

    // refresh metadata
    nb_lines += clipboard.nb_lines;
    clipboard.start = NULL;
    has_been_changes = 1;

    // copy inserted lines to clipboard
    copy_to_clip(first_inserted_line_nb, clipboard.nb_lines);

    // move cursor
    if (!below) {
        if (y == 0)
            first_line_on_screen = get_line(-clipboard.nb_lines);
        y += clipboard.nb_lines;
    }
}
