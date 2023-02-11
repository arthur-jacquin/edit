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
        new->prev = get_line(nb_lines - first_line_on_screen->line_nb);
        new->next = NULL;
    } else {
        replaced_line = get_line(asked_line_nb - first_line_on_screen->line_nb);
        shift_line_nb((asked_line_nb >= first_line_on_screen->line_nb) ?
            first_line_on_screen : first_line, asked_line_nb, 0, 1);
        new = create_line(asked_line_nb, ml, dl);
        new->chars[ml - 1] = '\0';
        if (is_first_line(replaced_line))
            new->prev = NULL;
        else
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
        while (res != NULL && delta_from_first_line_on_screen--)
            res = res->next;
    } else {
        while (res != NULL && delta_from_first_line_on_screen++)
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
    // free the memory used by start list

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

    l1->next = l2;
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
