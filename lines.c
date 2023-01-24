struct line *
new_line(int line_nb, int length)
{
    struct line *res;

    res = (struct line *) malloc(sizeof(struct line));
    res->line_nb = line_nb;
    res->length = length;
    res->prev = res->next = NULL;
    res->chars = (char *) malloc(length);

    return res;
}

struct line *
get_line(int delta_from_first_line_on_screen)
{
    struct line *ptr;
    int n;

    n = delta_from_first_line_on_screen;
    ptr = first_line_on_screen;
    if (n > 0) {
        while (n--) {
            if (ptr->next == NULL)
                break;
            ptr = ptr->next;
        }
    } else {
        while (n++) {
            if (ptr->prev == NULL)
                break;
            ptr = ptr->prev;
        }
    }

    return ptr;
}

void
free_lines(struct line *starting)
{
    struct line *ptr;
    struct line *old_ptr;

    if (starting != NULL) {
        ptr = starting;
        while (ptr->next != NULL) {
            old_ptr = ptr;
            ptr = ptr->next;
            free(old_ptr->chars);
            free(old_ptr);
        }
        free(ptr->chars);
        free(ptr);
    }
}

void
link_lines(struct line *l1, struct line *l2)
{
    l1->next = l2;
    l2->prev = l1;
}

void
shift_line_nb(int starting_line_nb, int ending_line_nb, int shift)
{
    struct line *ptr;

    if (first_line != NULL) {
        ptr = first_line;
        while (ptr->next != NULL) {
            if (ptr->line_nb >= starting_line_nb && ptr->line_nb <= ending_line_nb)
                ptr->line_nb += shift;
            ptr = ptr->next;
        }
        if (ptr->line_nb >= starting_line_nb && ptr->line_nb <= ending_line_nb)
            ptr->line_nb += shift;
    }
}

struct line *
insert_line(int asked_line_nb, int length)
{
    struct line *replaced_line;
    struct line *new;

    if (asked_line_nb > nb_line + 1)
        asked_line_nb = nb_line + 1;

    if (asked_line_nb <= nb_line) {
        replaced_line = get_line(asked_line_nb - first_line_on_screen->line_nb);
        shift_line_nb(asked_line_nb, nb_line, 1);
        new = new_line(asked_line_nb, length);
        new->chars[length - 1] = '\0';
        if (replaced_line->prev != NULL) {
            link_lines(replaced_line->prev, new);
        } else {
            new->prev = NULL;
        }
        link_lines(new, replaced_line);
    } else {
        new = new_line(asked_line_nb, length);
        new->chars[length - 1] = '\0';
        new->prev = get_line(nb_line - first_line_on_screen->line_nb);
        new->next = NULL;
    }

    nb_line++;
    return new;
}

int
move_line(int delta)
{
    int new_line_nb;
    struct line *src;
    struct line *dest;

    new_line_nb = first_line_on_screen->line_nb + y + delta;
    if (new_line_nb < 1)
        new_line_nb = 1;
    if (new_line_nb > nb_line)
        new_line_nb = nb_line;
    if (new_line_nb == first_line_on_screen->line_nb + y)
        return new_line_nb;

    src = get_line(y);
    dest = get_line(new_line_nb - first_line_on_screen->line_nb);

    if (delta > 0) {
        shift_line_nb(first_line_on_screen->line_nb + y + 1, new_line_nb, -1);
        if (src == first_line_on_screen)
            first_line_on_screen = first_line_on_screen->next;
        if (src->prev != NULL) {
            link_lines(src->prev, src->next);
        } else {
            (src->next)->prev = NULL;
            first_line = src->next;
        }
        if (dest->next != NULL) {
            link_lines(src, dest->next);
        } else {
            src->next = NULL;
        }
        link_lines(dest, src);
    } else {
        shift_line_nb(new_line_nb, first_line_on_screen->line_nb + y - 1, 1);
        if (src->next != NULL) {
            link_lines(src->prev, src->next);
        } else {
            (src->prev)->next = NULL;
        }
        if (dest->prev != NULL) {
            link_lines(dest->prev, src);
        } else {
            src->prev = NULL;
        }
        link_lines(src, dest);
    }
    src->line_nb = new_line_nb;
    
    return new_line_nb;
}

void
copy_to_clip(int starting_line_nb, int nb)
{
    int i;
    struct line *ptr;
    struct line *cp_ptr;
    struct line *old_cp_ptr;

    free_lines(clipboard.start);
    clipboard.start = NULL;

    if (starting_line_nb + nb - 1 > nb_line)
        nb = nb_line + 1 - starting_line_nb;

    ptr = get_line(starting_line_nb - first_line_on_screen->line_nb);
    for (i = 0; i < nb; i++) {
        cp_ptr = new_line(0, ptr->length);
        strcpy(cp_ptr->chars, ptr->chars);
        if (i == 0) {
            cp_ptr->prev = NULL;
            clipboard.start = cp_ptr;
        } else {
            link_lines(old_cp_ptr, cp_ptr);
        }
        old_cp_ptr = cp_ptr;
        if (ptr->next != NULL)
            ptr = ptr->next;
    }
    cp_ptr->next = NULL;
    clipboard.nb_line = nb;
}

void
move_to_clip(int starting_line_nb, int nb)
{
    struct line *starting;
    struct line *ending;

    free_lines(clipboard.start);
    clipboard.start = NULL;

    if (starting_line_nb + nb - 1 > nb_line)
        nb = nb_line + 1 - starting_line_nb;
    nb_line -= nb;

    starting = get_line(starting_line_nb - first_line_on_screen->line_nb);
    ending = get_line(starting_line_nb + nb-1 - first_line_on_screen->line_nb);

    if (ending->next == NULL) {
        if (starting->prev == NULL) {
            first_line = first_line_on_screen = new_line(1, 1);
            first_line->chars;
            first_line->prev = first_line->next = NULL;
            strcpy(first_line->chars, "");
            nb_line = 1;
        } else {
            (starting->prev)->next = NULL;
        }
    } else {
        if (starting->prev == NULL) {
            first_line = first_line_on_screen = ending->next;
            (ending->next)->prev = NULL;
        } else {
            link_lines(starting->prev, ending->next);
        }
        shift_line_nb(ending->line_nb + 1, nb_line + nb, -nb);
    }

    starting->prev = ending->next = NULL;
    clipboard.start = starting;
    clipboard.nb_line = nb;
}

void
insert_clip(struct line *starting_line, int below)
{
    int i, old_starting_line_nb;
    struct line *cp_ptr;
    struct line *ptr;
    struct line *old_ptr;
    struct line *next_starting_line;

    if (clipboard.start != NULL) {
        if (below) {
            next_starting_line = starting_line->next;
        } else {
            next_starting_line = starting_line->prev;
        }

        old_starting_line_nb = starting_line->line_nb;
        shift_line_nb(old_starting_line_nb + below, nb_line, clipboard.nb_line);
        nb_line += clipboard.nb_line;
        
        cp_ptr = clipboard.start;
        for (i = 0; i < clipboard.nb_line; i++) {
            ptr = new_line(old_starting_line_nb + i + below, cp_ptr->length);
            strcpy(ptr->chars, cp_ptr->chars);
            if (i == 0) {
                if (below) {
                    link_lines(starting_line, ptr);
                } else if (next_starting_line != NULL) {
                    link_lines(next_starting_line, ptr);
                } else {
                    ptr->prev = NULL;
                }
            } else {
                link_lines(old_ptr, ptr);
            }
            old_ptr = ptr;
            if (cp_ptr->next != NULL)
                cp_ptr = cp_ptr->next;
        }
        
        if (below) {
            if (next_starting_line != NULL) {
                link_lines(ptr, next_starting_line);
            } else {
                ptr->next = NULL;
            }
        } else {
            link_lines(ptr, starting_line);
        }
    }
}
