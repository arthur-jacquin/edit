void
display_help(void)
{
    // tell the user how to get more complete help

    echo(HELP_MESSAGE);
}

void
init_interface(struct interface *interf, const char *chars)
{
    // copy chars in current and previous fields of interf

    strcpy(interf->current, chars);
    strcpy(interf->previous, chars);
}

int
dialog(const char *prompt, struct interface *interf, int refresh)
{
    // enable a user input on the dialog line
    // return 0 if user cancelled, 1 if user confirmed
    // if refresh is set, display search results on each keystroke

    int dpl, dx, n, i, j, k, len;
    uint32_t c;

    for (k = i = 0; prompt[k]; i++)
        k += utf8_char_length(prompt[k]);
    dpl = i;
    strcpy(interf->current, "");
    dx = n = 0;

    while (1) {
        if (refresh) {
            // compute newly displayed selections
            forget_sel_list(displayed);
            displayed = search(saved);
            print_all();
        }
        strcpy(dialog_chars, prompt);
        strcat(dialog_chars, interf->current);
        print_dialog();
        tb_set_cursor(dx + dpl, screen_height - 1);
        tb_present();
        tb_poll_event(&ev);
        switch (ev.type) {
        case TB_EVENT_KEY:
            if (ev.ch && n < INTERFACE_WIDTH - dpl - 1) {
                // insert unicode codepoint ev.ch in interf->current

                // compute number of untouched bytes
                for (k = 0, i = 0; i < dx; i++)
                    k += utf8_char_length(interf->current[k]);

                // copy bytes after new character
                len = unicode_char_length(c = ev.ch);
                for (j = strlen(interf->current); j >= k; j--) // copy NULL
                    interf->current[j + len] = interf->current[j];

                // write ev.ch
                for (j = len - 1; j > 0; j--) {
                    interf->current[k + j] = (c & 0x3f) | 0x80;
                    c >>= 6;
                }
                interf->current[k] = c | utf8_start[len - 1];

                // refresh metadata
                dx++;
                n++;
            } else {
                switch (ev.key) {
                case TB_KEY_ESC:
                    strcpy(interf->current, interf->previous);
                    echo("");
                    return 0;
                case TB_KEY_ENTER:
                    strcpy(interf->previous, interf->current);
                    return 1;
                case TB_KEY_BACKSPACE:
                case TB_KEY_BACKSPACE2:
                    if (dx > 0) {
                        // delete dx-th displayable character

                        // compute number of untouched bytes
                        for (k = 0, i = 0; i < dx - 1; i++)
                            k += utf8_char_length(interf->current[k]);

                        // copy remaining characters
                        len = utf8_char_length(interf->current[k]);
                        while (interf->current[k] = interf->current[k + len])
                            k++;

                        // refresh metadata
                        dx--;
                        n--;
                    }
                    break;
                case TB_KEY_ARROW_RIGHT:
                    if (dx < n)
                        dx++;
                    break;
                case TB_KEY_ARROW_LEFT:
                    if (dx > 0)
                        dx--;
                    break;
                case TB_KEY_ARROW_UP:
                case TB_KEY_ARROW_DOWN:
                    strcpy(interf->current,
                        (ev.key == TB_KEY_ARROW_UP) ? (interf->previous) : "");
                    for (k = i = 0; interf->current[k]; i++)
                        k += utf8_char_length(interf->current[k]);
                    dx = n = i;
                    break;
                }
            }
            break;

        case TB_EVENT_MOUSE:
            switch (ev.key) {
            case TB_KEY_MOUSE_LEFT:
                dx = ev.x - dpl;
                dx = (dx < 0) ? 0 : dx;
                dx = (dx > n) ? n : dx;
                break;
            }
            break;

        case TB_EVENT_RESIZE:
            // TODO: int resize_to_be_treated
            if (resize(ev.w, ev.h)) {
                if (has_been_changes)
                    write_file(BACKUP_FILE_NAME);
                // TODO: find a way to quit
                // return ERR_TERM_NOT_BIG_ENOUGH;
            } else {
                // TODO: what to do ?
                // go_to(pos_of(first_line_on_screen->line_nb + y, x));
            }
            break;
        }
    }
}

int
set_parameter(const char *assign)
{
    // process a settings modification

    int b;
    char c, s[LANG_WIDTH], old_lang[LANG_WIDTH];
    struct lang *old_syntax;

    if (strchr(assign, '=') == NULL) {
        return 0;
    } else if (sscanf(assign, "autoindent=%d", &b) == 1
            || sscanf(assign, "i=%d", &b) == 1) {
        settings.autoindent = b;
    } else if (sscanf(assign, "syntax_highlight=%d", &b) == 1
            || sscanf(assign, "sh=%d", &b) == 1) {
        settings.syntax_highlight = b;
    } else if (sscanf(assign, "highlight_selections=%d", &b) == 1
            || sscanf(assign, "h=%d", &b) == 1) {
        settings.highlight_selections = b;
    } else if (sscanf(assign, "case_sensitive=%d", &b) == 1
            || sscanf(assign, "c=%d", &b) == 1) {
        settings.case_sensitive = b;
    } else if (sscanf(assign, "field_separator=%c", &c) == 1
            || sscanf(assign, "fs=%c", &c) == 1) {
        settings.field_separator = c;
    } else if (sscanf(assign, "tab_width=%d", &b) == 1
            || sscanf(assign, "tw=%d", &b) == 1) {
        settings.tab_width = (b >= 0) ? b : TAB_WIDTH;
    } else if (sscanf(assign, "language=%s", s) == 1
            || sscanf(assign, "l=%s", s) == 1) {
        strcpy(old_lang, settings.language);
        old_syntax = settings.syntax;
        strcpy(settings.language, s);
        load_lang();
        if (settings.syntax == NULL) {
            strcpy(settings.language, old_lang);
            settings.syntax = old_syntax;
            return 0;
        }
    } else {
        return 0;
    }

    return 1;
}

int
parse_range(const char *range)
{
    // process a custom range of lines selection

    int l1, l2;
    char *ptr;

    // split range in two strings: range and ptr
    ptr = strchr(range, ',');
    if (ptr == NULL)
        return 0;
    *ptr = '\0';
    ptr++;

    // parse range
    if (strcmp(range, "") == 0) {
        l1 = 1;
    } else if (strcmp(range, ".") == 0) {
        l1 = first_line_on_screen->line_nb + y;
    } else if (sscanf(range, "%d", &l1) == 1) {
        l1 = (l1 < 1) ? 1 : l1;
    } else {
        return 0;
    }

    // parse ptr
    if (strcmp(ptr, "") == 0) {
        l2 = nb_lines;
    } else if (strcmp(ptr, ".") == 0) {
        l2 = first_line_on_screen->line_nb + y;
    } else if (sscanf(ptr, "%d", &l2) == 1) {
        l2 = (l2 > nb_lines) ? nb_lines : l2;
    } else {
        return 0;
    }

    // add selections
    if (l2 < l1) {
        return 0;
    } else {
        forget_sel_list(saved);
        saved = range_lines_sel(l1, l2, NULL);
        return 1;
    }
}
