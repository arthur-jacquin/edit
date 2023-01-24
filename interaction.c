int
dialog(const char *prompt, struct interface *interf, int refresh)
{
    int dx;
    int i, n;
    int prompt_length;

    dx = prompt_length = strlen(prompt);
    strcpy(interf->current, "");
    n = strlen(interf->current);

    while (1) {
        if (refresh) {
            delete_temp_sels();
            add_running_sels(1);
            print_all();
        }
        strcpy(dialog_chars, prompt);
        strcat(dialog_chars, interf->current);
        print_dialog();
        tb_set_cursor(dx, screen_height - 1);
        tb_present();
        tb_poll_event(&ev);
        switch (ev.type) {
        case TB_EVENT_KEY:
            if (ev.ch && n < INTERFACE_WIDTH - prompt_length - 1) {
                for (i = n; i >= dx - prompt_length; i--)
                    interf->current[i+1] = interf->current[i];
                interf->current[dx - prompt_length] = ev.ch;
                dx++;
                n++;
            } else {
                switch (ev.key) {
                case TB_KEY_ESC:
                    strcpy(interf->current, interf->previous);
                    echo("");
                    return 0;
                    break;
                case TB_KEY_ENTER:
                    strcpy(interf->previous, interf->current);
                    return 1;
                    break;
                case TB_KEY_BACKSPACE:
                case TB_KEY_BACKSPACE2:
                    if (dx - prompt_length > 0) {
                        for (i = dx - prompt_length; i < n; i++)
                            interf->current[i-1] = interf->current[i];
                        dx--;
                        n--;
                    }
                    break;
                case TB_KEY_ARROW_RIGHT:
                    dx += (dx - prompt_length < n - 1) ? 1 : 0;
                    break;
                case TB_KEY_ARROW_LEFT:
                    dx -= (dx - prompt_length > 0) ? 1 : 0;
                    break;
                case TB_KEY_ARROW_UP:
                case TB_KEY_ARROW_DOWN:
                    strcpy(interf->current, (ev.key == TB_KEY_ARROW_UP) ? (interf->previous) : "");
                    n = strlen(interf->current) + 1;
                    dx = prompt_length + n - 1;
                    break;
                }
            }
            break;

        case TB_EVENT_MOUSE:
            switch (ev.key) {
            case TB_KEY_MOUSE_LEFT:
                dx = (ev.x < INTERFACE_WIDTH) ? (ev.x) : (INTERFACE_WIDTH - 1);
                dx = (dx >= prompt_length) ? (dx) : (prompt_length);
                break;
            // TODO ?
            // case TB_KEY_MOUSE_WHEEL_UP:
            // case TB_KEY_MOUSE_WHEEL_DOWN:
            }
            break;

        case TB_EVENT_RESIZE:
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

void
display_help(void)
{
    // TODO: do better
    echo("Quit the editor and run `edit --help` for complete help.");
}

int
set_parameter(char *assign)
{
    int b;
    char c;
    char s[LANG_WIDTH], old_lang[LANG_WIDTH];
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
        old_syntax = syntax;
        strcpy(settings.language, s);
        load_lang();
        if (syntax == NULL) {
            strcpy(settings.language, old_lang);
            syntax = old_syntax;
            return 0;
        }
    } else {
        return 0;
    }

    return 1;
}

int
parse_range(char *range)
{
    int l1, l2;
    char *ptr;

    ptr = strchr(range, ',');

    if (ptr == NULL)
        return 0;
    *ptr = '\0';
    ptr++;

    if (strcmp(range, "") == 0) {
        l1 = 1;
    } else if (strcmp(range, ".") == 0) {
        l1 = first_line_on_screen->line_nb + y;
    } else if (sscanf(range, "%d", &l1) == 1) {
        l1 = (l1 < 1) ? 1 : l1;
    } else {
        return 0;
    }

    if (strcmp(ptr, "") == 0) {
        l2 = nb_line;
    } else if (strcmp(ptr, ".") == 0) {
        l2 = first_line_on_screen->line_nb + y;
    } else if (sscanf(ptr, "%d", &l2) == 1) {
        l2 = (l2 > nb_line) ? nb_line : l2;
    } else {
        return 0;
    }

    if (l2 < l1) {
        return 0;
    } else {
        add_range_sels(l1, l2, 0);
        return 1;
    }
}
