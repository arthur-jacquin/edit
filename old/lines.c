int
move_line(int delta)
{
    int new_line_nb;
    struct line *src;
    struct line *dest;

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

    if (starting_line_nb + nb - 1 > nb_lines)
        nb = nb_lines + 1 - starting_line_nb;

    ptr = get_line(starting_line_nb - first_line_on_screen->line_nb);
    for (i = 0; i < nb; i++) {
        cp_ptr = new_line(0, ptr->ml);
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

    if (starting_line_nb + nb - 1 > nb_lines)
        nb = nb_lines + 1 - starting_line_nb;
    nb_lines -= nb;

    starting = get_line(starting_line_nb - first_line_on_screen->line_nb);
    ending = get_line(starting_line_nb + nb-1 - first_line_on_screen->line_nb);

    if (ending->next == NULL) {
        if (starting->prev == NULL) {
            first_line = first_line_on_screen = new_line(1, 1);
            first_line->chars;
            first_line->prev = first_line->next = NULL;
            strcpy(first_line->chars, "");
            nb_lines = 1;
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
        shift_line_nb(ending->line_nb + 1, nb_lines + nb, -nb);
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
        shift_line_nb(old_starting_line_nb + below, nb_lines, clipboard.nb_line);
        nb_lines += clipboard.nb_line;
        
        cp_ptr = clipboard.start;
        for (i = 0; i < clipboard.nb_line; i++) {
            ptr = new_line(old_starting_line_nb + i + below, cp_ptr->ml);
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
