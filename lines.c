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
 
int
insert_characters(struct line *l, struct selection *a, int start, int n,
    int nb_bytes)
{
    // insert nb_bytes bytes (corresponding to n characters) after start
    // displayable characters (bytes are not initialised)
    // return index of first inserted byte in l->chars

    int i, j, k, k1, len;
    char *new_chars, *old_chars;

    // create new string
    new_chars = (char *) malloc(l->ml + nb_bytes);

    // copy bytes before insertion
    for (k = i = 0; i < start; i++) {
        len = utf8_char_length(l->chars[k]);
        for (j = 0; j < len; j++)
            new_chars[k + j] = l->chars[k + j];
        k += len;
    }

    // copy bytes after insertion
    for (k1 = k; k < l->ml; k++)
        new_chars[k + nb_bytes] = l->chars[k];

    // refresh metadata
    old_chars = l->chars;
    l->chars = new_chars;
    free(old_chars);
    l->ml += nb_bytes;
    l->dl += n;

    // move selections
    while (a != NULL && a->l < l->line_nb)
        a = a->next;
    while (a != NULL && a->l == l->line_nb) {
        if (start <= a->x + a->n) {
            if (start <= a->x)
                a->x += n;
            else
                a->n += n;
        }
        a = a->next;
    }

    // move cursor and anchor
    if (l->line_nb == first_line_on_screen->line_nb + y && start <= x)
        x += n;
    if (anchored && l->line_nb == anchor.l && start <= anchor.x)
        anchor.x += n;

    return k1;
}

void
delete_characters(struct line *l, struct selection *a, int start, int n)
{
    // delete n characters after start

    int k, k1, k2;
    char *new_chars, *old_chars;

    // compute length of new_chars
    k1 = get_str_index(l, start);
    k2 = get_str_index(l, start + n);
    l->ml -= k2 - k1;
    l->dl -= n;

    // create new string
    new_chars = (char *) malloc(l->ml);
    for (k = 0; k < k1; k++)
        new_chars[k] = l->chars[k];
    for (; k < l->ml; k++)
        new_chars[k] = l->chars[k + (k2 - k1)];

    // refresh metadata
    old_chars = l->chars;
    l->chars = new_chars;
    free(old_chars);

    // move selections
    while (a != NULL && a->l < l->line_nb)
        a = a->next;
    while (a != NULL && a->l == l->line_nb) {
        if (start <= a->x + a->n) {
            if (start <= a->x)
                a->x -= n;
            else
                a->n += start - a->x;
        }
        a = a->next;
    }

    // move cursor and anchor
    if (l->line_nb == first_line_on_screen->line_nb + y && start <= x)
        x = (x - n >= start) ? (x - n) : start;
    if (anchored && l->line_nb == anchor.l && start <= anchor.x)
        anchor.x = (anchor.x - n >= start) ? (anchor.x - n) : start;
}
/*
void
replace_characters(struct line *l, struct selection *a, int start, int n,
    int new_n, int nb_bytes)
{
    // replace n characters after start by new_n characters taking nb_bytes
    // bytes (characters are not initialised)

    int i, k, k1, k2;
    char *new_chars, *old_chars;

    // compute length of new_chars
    k1 = get_str_index(l, start);
    k2 = get_str_index(l, start + n);
    l->ml += nb_bytes - (k2 - k1);
    l->dl += new_n - n;

    // create new string
    new_chars = (char *) malloc(l->ml);
    for (k = 0; k < k1; k++)
        new_chars[k] = l->chars[k];
    for (k = k1 + nb_bytes; k < l->ml; k++)
        new_chars[k] = l->chars[k - (nb_bytes - (k2 - k1))];

    // refresh metadata
    old_chars = l->chars;
    l->chars = new_chars;
    free(old_chars);

    // move selections
    while (a != NULL && a->l < l->line_nb)
        a = a->next;
    while (a != NULL && a->l == l->line_nb) {
        if (start <= a->x + a->n) {
            if (start <= a->x)
                a->x += new_n - n;
            else
        //        a->n += start - a->x;
        }
        a = a->next;
    }

    // move cursor and anchor
    if (l->line_nb == first_line_on_screen->line_nb + y && start <= x) {
        if (start + n <= x)
            x += new_n - n;
        else
            //x = (x - n >= start) ? (x - n) : start;
    }
    if (anchored && l->line_nb == anchor.l && start <= anchor.x) {
        if (start + n <= anchor.x)
            anchor.x += new_n - n;
        else
            //x = (x - n >= start) ? (x - n) : start;
    }
}*/

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
        shift_sel_line_nb(saved, asked_line_nb, 0, 1);
        new = create_line(asked_line_nb, ml, dl);
        new->chars[ml - 1] = '\0';
        link_lines(replaced_line->prev, new);
        link_lines(new, replaced_line);
    }

    // TODO anchor ? cursor ?
    if (first_line_on_screen->line_nb + y >= asked_line_nb)
        y++;

    nb_lines++;
    return new;
}

/*void TODO suppress
delete_line(int asked_line_nb, int ml, int dl)
{
    // TODO

    struct line *l;

    l = get_line(asked_line_nb - first_line_on_screen->line_nb);
    if (is_last_line(l)) {
        if (is_first_line(l)) {
            // create empty line
            first_line = first_line_on_screen = create_line(1, 1, 0);
            first_line->prev = first_line->next = NULL;
            strcpy(first_line->chars, "");
            nb_lines = 1;
        }
    } else {
        if (is_first_line(l))
            first_line = first_line_on_screen = l->next;
        shift_line_nb(l, l->line_nb + 1, 0, -1);
    }
    link_lines(l->prev, l->next);
    nb_lines--;

    // suppress and move selections
    remove_sel_line_range(asked_line_nb, asked_line_nb);
    shift_sel_line_nb(saved, asked_line_nb, 0, -1);

    // TODO cursor ? anchor ?
}*/

void
concatenate_line(struct line *l, struct selection *s)
{
    // move l->next (existence assumed) content at the end of l

    char *new_chars;

    // move selections
    move_sel_end_of_line(s, l->line_nb, l->dl, 1);
    shift_sel_line_nb(s, l->line_nb + 1, 0, -1);
    // TODO cursor ? anchor ?
    
    // create new chars, refresh metadata
    new_chars = (char *) malloc(l->ml + l->next->ml - 1);
    strncpy(new_chars, l->chars, l->ml - 1);
    strncpy(&(new_chars[l->ml - 1]), l->next->chars, l->next->ml);
    free(l->chars);
    l->chars = new_chars;
    l->ml += l->next->ml - 1;
    l->dl += l->next->dl;
    link_lines(l, l->next->next);
    forget_line(l->next);
    shift_line_nb(l, l->line_nb + 1, 0, -1);
    // TODO move line numbers
    nb_lines--;
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
    while (l != NULL && l->line_nb < min)
        l = l->next;
    while (l != NULL && (!max || l->line_nb <= max)) {
        l->line_nb += delta;
        l = l->next;
    }
}

int
move_line(int delta)
{
    // move cursor line, return new line_nb

    int cursor_line, new_line_nb;
    struct line *src, *dest;
 
    // compute new line number
    cursor_line = first_line_on_screen->line_nb + y;
    new_line_nb = cursor_line + delta;
    if (new_line_nb < 1)
        new_line_nb = 1;
    if (new_line_nb > nb_lines)
        new_line_nb = nb_lines;
    if (new_line_nb == cursor_line)
         return new_line_nb;

    src = get_line(y);
    dest = get_line(new_line_nb - first_line_on_screen->line_nb);

    // TODO: selection shifting
    // shift_sel_line_nb(saved, cursor_line, cursor_line, -cursor_line);
    if (delta > 0) {
        //shift_sel_line_nb(saved, cursor_line + 1, new_line_nb, -1);
        shift_line_nb(first_line, cursor_line + 1, new_line_nb, -1);
        if (src == first_line_on_screen)
            first_line_on_screen = first_line_on_screen->next;
        if (is_first_line(src))
            first_line = src->next;
        link_lines(src->prev, src->next);
        link_lines(src, dest->next);
        link_lines(dest, src);
    } else {
        //shift_sel_line_nb(saved, new_linecursor_line + 1, new_line_nb, -1);
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
    // shift_sel_line_nb(saved, -1, -1, -cursor_line);

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
        } else {
            nb_lines -= nb;
        }
    } else {
        if (is_first_line(starting))
            first_line = first_line_on_screen = ending->next;
        shift_line_nb(ending, ending->line_nb + 1, 0, -nb);
        nb_lines -= nb;
    }

    link_lines(starting->prev, ending->next);
    link_lines(NULL, starting);
    link_lines(ending, NULL);
    shift_line_nb(starting, 0, 0, -starting->line_nb);
    clipboard.start = starting;
    clipboard.nb_lines = nb;

    // suppress and move selections
    remove_sel_line_range(starting_line_nb, starting_line_nb + nb - 1);
    shift_sel_line_nb(saved, starting_line_nb, 0, -nb);
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
    shift_sel_line_nb(saved, first_inserted_line_nb, 0, clipboard.nb_lines);
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
