// TODO: manage order
#include "globals.h"
#include "utils.c"
#include "file.c"
#include "lines.c"
#include "interaction.c"
#include "graphical.c"
#include "movements.c"
#include "selections.c"
#include "actions.c"
#include "search_and_replace.c"

int
main(int argc, char *argv[])
{
    int l1, old_line_nb;
    struct pos p;

    // PARSING ARGUMENTS *******************************************************

    if (argc < 2 || !(strcmp(argv[1], "--help") && strcmp(argv[1], "-h"))) {
        printf("%s\n", HELP_MESSAGE);
        return 0;
    } else if (!(strcmp(argv[1], "--version") && strcmp(argv[1], "-v"))) {
        printf("%s\n", VERSION);
        return 0;
    } else {
        init_interface(&file_name_int, argv[1]);
    }

    // check if file can be accessed, and loads it if possible
    if (load_file(file_name_int.current, 1) == ERR_FILE_CONNECTION) {
        printf("%s\n", FILE_CONNECTION_MESSAGE);
        return ERR_FILE_CONNECTION;
    }


    // INIT VARIABLES **********************************************************

    // settings
    settings.autoindent = AUTOINDENT;
    settings.tab_width = TAB_WIDTH;
    init_interface(&settings_int, "");
    load_lang(file_name_int.current);

    // editor variables
    m = in_insert_mode = anchored = is_bracket = 0;

    // selections
    saved = temp = displayed = NULL;
    init_interface(&range_int, "");

    // search and replace engine
    settings.case_sensitive = CASE_SENSITIVE;
    settings.field_separator = FIELD_SEPARATOR;
    init_interface(&search_pattern, "");
    init_interface(&replace_pattern, "");

    // clipboard
    clipboard.start = NULL;

    // initialise termbox
    settings.syntax_highlight = SYNTAX_HIGHLIGHT;
    settings.highlight_selections = HIGHLIGHT_SELECTIONS;
    x = y = 0;
    tb_init();
    tb_set_output_mode(OUTPUT_MODE);
    if (MOUSE_SUPPORT)
        tb_set_input_mode(TB_INPUT_ESC | TB_INPUT_MOUSE);
    if (resize(tb_width(), tb_height()))
        return ERR_TERM_NOT_BIG_ENOUGH;
    echo(WELCOME_MESSAGE);

    // main loop
    while (1) {
        // compute new displayed selections
        forget_sel_list(temp);
        temp = running_sel();
        forget_sel_list(displayed);
        displayed = merge_sel(temp, saved);

        // refresh screen and wait for input
        print_all();
        tb_present();
        tb_poll_event(&ev);

        // process input
        switch (ev.type) {
        case TB_EVENT_KEY:
            if (ev.ch && in_insert_mode) {
                act(insert, 0);
                has_been_changes = 1;
            } else if (ev.ch && !in_insert_mode) {
                if ((m && ev.ch == '0') || ('1' <= ev.ch && ev.ch <= '9')) {
                    m = 10*m + ev.ch - '0';
                    sprintf(dialog_chars, MULTIPLIER_MESSAGE_PATTERN, m);
                    break;
                } else {
                if (m == 0)
                    m = 1;
                switch (ev.ch) {
                case KB_HELP:
                    echo(HELP_MESSAGE);
                    break;
                case KB_QUIT:
                case KB_FORCE_QUIT:
                    if (ev.ch == KB_QUIT && has_been_changes) {
                        echo(UNSAVED_CHANGES_MESSAGE);
                    } else {
                        tb_shutdown();
                        return 0;
                    }
                    break;
                case KB_WRITE:
                    if (has_been_changes) {
                        write_file(file_name_int.current);
                        has_been_changes = 0;
                        echo(FILE_SAVED_MESSAGE);
                    } else {
                        echo(NOTHING_TO_WRITE_MESSAGE);
                    }
                    break;
                case KB_WRITE_AS:
                    if (dialog(SAVE_AS_PROMPT, &file_name_int, 0)) {
                        write_file(file_name_int.current);
                        load_lang(file_name_int.current);
                        has_been_changes = 0;
                        echo(FILE_SAVED_MESSAGE);
                    }
                    break;
                case KB_RELOAD:
                    if (has_been_changes) {
                        reset_selections();
                        old_line_nb = first_line_on_screen->line_nb + y;
                        load_file(file_name_int.current, first_line_on_screen->line_nb);
                        go_to(pos_of(old_line_nb, x));
                        has_been_changes = 0;
                        echo(FILE_RELOADED_MESSAGE);
                    } else {
                        echo(NOTHING_TO_REVERT_MESSAGE);
                    }
                    break;
                case KB_INSERT_MODE:
                    in_insert_mode = 1;
                    echo(INSERT_MODE_MESSAGE);
                    break;
                case KB_CHANGE_SETTING:
                    if (dialog(CHANGE_SETTING_PROMPT, &settings_int, 0))
                        if (!set_parameter(settings_int.current))
                            echo(INVALID_ASSIGNMENT_MESSAGE);
                    break;
                case KB_INSERT_START_LINE:
                case KB_INSERT_END_LINE:
                    reset_selections();
                    go_to(pos_of(first_line_on_screen->line_nb + y,
                        (ev.ch == KB_INSERT_END_LINE) ? get_line(y)->dl : 0));
                    in_insert_mode = 1;
                    echo(INSERT_MODE_MESSAGE);
                    break;
                case KB_INSERT_LINE_BELOW:
                case KB_INSERT_LINE_ABOVE:
                    //reset_selections(); // XXX ?
                    //anchored = 0; // XXX ?
                    l1 = first_line_on_screen->line_nb + y +
                        ((ev.ch == KB_INSERT_LINE_BELOW) ? 1 : 0);
                    insert_line(l1, 1, 0);
                    go_to(pos_of(l1, 0));
                    in_insert_mode = 1;
                    has_been_changes = 1;
                    echo(INSERT_MODE_MESSAGE);
                    break;
                case KB_CLIP_YANK_LINE:
                    copy_to_clip(first_line_on_screen->line_nb + y, m);
                    break;
                case KB_CLIP_YANK_BLOCK:
                    l1 = find_start_of_block(first_line_on_screen->line_nb + y, 1);
                    copy_to_clip(l1, find_end_of_block(l1, m) - l1 + 1);
                    break;
                case KB_CLIP_DELETE_LINE:
                    move_to_clip(first_line_on_screen->line_nb + y, m);
                    go_to(pos_of(first_line_on_screen->line_nb + y, x));
                    has_been_changes = 1;
                    break;
                case KB_CLIP_DELETE_BLOCK:
                    l1 = find_start_of_block(first_line_on_screen->line_nb + y, 1);
                    move_to_clip(l1, find_end_of_block(l1, m) - l1 + 1);
                    go_to(pos_of(l1, x));
                    has_been_changes = 1;
                    break;
                case KB_CLIP_PASTE_AFTER:
                case KB_CLIP_PASTE_BEFORE:
                    while (m--)
                        insert_clip(get_line(y), ev.ch == KB_CLIP_PASTE_AFTER);
                    has_been_changes = 1;
                    break;
                case KB_MOVE_MATCHING:
                    go_to(find_matching_bracket());
                    break;
                case KB_MOVE_START_LINE:
                    go_to(pos_of(first_line_on_screen->line_nb + y, 0));
                    break;
                case KB_MOVE_NON_BLANK:
                    go_to(find_first_non_blank());
                    break;
                case KB_MOVE_END_LINE:
                    go_to(pos_of(first_line_on_screen->line_nb + y,
                        get_line(y)->dl));
                    break;
                case KB_MOVE_SPECIFIC_LINE:
                    go_to(pos_of(m, x));
                    break;
                case KB_MOVE_END_FILE:
                    go_to(pos_of(nb_lines, x));
                    break;
                case KB_MOVE_NEXT_CHAR:
                    go_to(pos_of(first_line_on_screen->line_nb + y, x + m));
                    break;
                case KB_MOVE_PREV_CHAR:
                    go_to(pos_of(first_line_on_screen->line_nb + y, x - m));
                    break;
                case KB_MOVE_NEXT_LINE:
                    go_to(pos_of(first_line_on_screen->line_nb + y + m, x));
                    break;
                case KB_MOVE_PREV_LINE:
                    go_to(pos_of(first_line_on_screen->line_nb + y - m, x));
                    break;
                case KB_MOVE_NEXT_WORD:
                    go_to(find_start_of_word(m));
                    break;
                case KB_MOVE_PREV_WORD:
                    go_to(find_start_of_word(-m));
                    break;
                case KB_MOVE_NEXT_BLOCK:
                    go_to(pos_of(find_end_of_block(
                        first_line_on_screen->line_nb + y, m), x));
                    break;
                case KB_MOVE_PREV_BLOCK:
                    go_to(pos_of(find_start_of_block(
                        first_line_on_screen->line_nb + y, m), x));
                    break;
                case KB_MOVE_NEXT_SEL:
                    if ((p = find_next_selection(m)).l)
                        go_to(p);
                    else
                        echo(NO_SEL_DOWN_MESSAGE);
                    break;
                case KB_MOVE_PREV_SEL:
                    if ((p = find_next_selection(-m)).l)
                        go_to(p);
                    else
                        echo(NO_SEL_UP_MESSAGE);
                    break;
                case KB_SEL_DISPLAY_COUNT:
                    sprintf(dialog_chars, SELECTIONS_MESSAGE_PATTERN, nb_sel(saved));
                    break;
                case KB_SEL_CURSOR_LINE:
                    forget_sel_list(saved);
                    saved = range_lines_sel(first_line_on_screen->line_nb + y,
                        first_line_on_screen->line_nb + y, NULL);
                    break;
                case KB_SEL_CUSTOM_RANGE:
                    if (dialog(RANGE_PROMPT, &range_int, 0))
                        if (!parse_range(range_int.current))
                            echo(INVALID_RANGE_MESSAGE);
                    break;
                case KB_SEL_ALL_LINES:
                    forget_sel_list(saved);
                    saved = range_lines_sel(1, nb_lines, NULL);
                    break;
                case KB_SEL_LINES_BLOCK:
                    forget_sel_list(saved);
                    l1 = find_start_of_block(first_line_on_screen->line_nb + y, 1);
                    saved = range_lines_sel(l1, find_end_of_block(l1, m), NULL);
                    break;
                case KB_SEL_FIND:
                case KB_SEL_SEARCH:
                    if (dialog(SEARCH_PATTERN_PROMPT, &search_pattern, 1)) {
                        forget_sel_list(saved);
                        saved = displayed;
                        displayed = NULL;
                        anchored = 0;
                    }
                    break;
                case KB_SEL_ANCHOR:
                    if (anchored) {
                        anchored = 0;
                    } else {
                        anchor = pos_of_cursor();
                        anchored = 1;
                    }
                    break;
                case KB_SEL_APPEND:
                    forget_sel_list(saved);
                    saved = displayed;
                    displayed = NULL;
                    anchored = 0;
                    break;
                case KB_SEL_COLUMN:
                    if (anchored && anchor.l != pos_of_cursor().l)
                        echo(COLUMN_SEL_ERROR_MESSAGE);
                    else
                        go_to(column_sel(m));
                    break;
                case KB_ACT_INCREASE_INDENT:
                case KB_ACT_DECREASE_INDENT:
                    asked_indent = m * settings.tab_width *
                        ((ev.ch == KB_ACT_DECREASE_INDENT) ? -1 : 1);
                    act(indent, 1);
                    break;
                case KB_ACT_COMMENT:
                    if (settings.syntax != NULL)
                        act(comment, 1);
                    break;
                case KB_ACT_SUPPRESS:
                    asked_remove = m;
                    act(suppress, 0);
                    break;
                case KB_ACT_REPLACE:
                    if (dialog(REPLACE_PATTERN_PROMPT, &replace_pattern, 0))
                        act(replace, 0);
                    break;
                case KB_ACT_LOWERCASE:
                    act(lower, 0);
                    break;
                case KB_ACT_UPPERCASE:
                    act(upper, 0);
                    break;
                }
                m = 0;
                }
            } else if (ev.key) {
                if (m == 0)
                    m = 1;
                switch (ev.key) {
                case TB_KEY_ARROW_RIGHT:
                    go_to(pos_of(first_line_on_screen->line_nb + y, x + m));
                    break;
                case TB_KEY_ARROW_LEFT:
                    go_to(pos_of(first_line_on_screen->line_nb + y, x - m));
                    break;
                case TB_KEY_ARROW_DOWN:
                    if (ev.mod == TB_MOD_SHIFT) {
                        go_to(pos_of(move_line(m), x));
                        has_been_changes = 1;
                    } else {
                        go_to(pos_of(first_line_on_screen->line_nb + y + m, x));
                    }
                    break;
                case TB_KEY_ARROW_UP:
                    if (ev.mod == TB_MOD_SHIFT) {
                        go_to(pos_of(move_line(-m), x));
                        has_been_changes = 1;
                    } else {
                        go_to(pos_of(first_line_on_screen->line_nb + y - m, x));
                    }
                    break;
                case TB_KEY_ESC:
                    if (in_insert_mode) {
                        in_insert_mode = 0;
                    } else {
                        reset_selections();
                    }
                    echo("");
                    break;
                case TB_KEY_ENTER:
                    if (in_insert_mode) {
                        act(split, 0);
                    } else {
                        go_to(pos_of(first_line_on_screen->line_nb + y + m, x));
                    }
                    break;
                case TB_KEY_BACKSPACE:
                case TB_KEY_BACKSPACE2:
                case TB_KEY_DELETE:
                    asked_remove = (ev.key == TB_KEY_DELETE) ? m : -m;
                    act(suppress, 0);
                    break;
                case TB_KEY_TAB:
                case TB_KEY_BACK_TAB:
                    asked_indent = m * settings.tab_width *
                        ((ev.key == TB_KEY_BACK_TAB) ? -1 : 1);
                    act(indent, 1);
                    break;
                }
                m = 0;
            }
            break;

        case TB_EVENT_MOUSE:
            switch (ev.key) {
            case TB_KEY_MOUSE_LEFT:
                if (ev.y < screen_height - 1)
                    go_to(pos_of(first_line_on_screen->line_nb + ev.y, ev.x));
                break;
            case TB_KEY_MOUSE_WHEEL_UP:
                old_line_nb = first_line_on_screen->line_nb + y;
                first_line_on_screen = get_line(-SCROLL_LINE_NUMBER);
                // TODO: replace by a min
                if (old_line_nb > first_line_on_screen->line_nb + screen_height - 2) {
                    go_to(pos_of(first_line_on_screen->line_nb + screen_height - 2, x));
                } else {
                    go_to(pos_of(old_line_nb, x));
                }
                break;
            case TB_KEY_MOUSE_WHEEL_DOWN:
                old_line_nb = first_line_on_screen->line_nb + y;
                first_line_on_screen = get_line(SCROLL_LINE_NUMBER);
                // TODO: replace by a max
                if (old_line_nb < first_line_on_screen->line_nb) {
                    go_to(pos_of(first_line_on_screen->line_nb, x));
                } else {
                    go_to(pos_of(old_line_nb, x));
                }
                break;
            }
            break;

        case TB_EVENT_RESIZE:
            if (resize(ev.w, ev.h)) {
                if (has_been_changes)
                    write_file(BACKUP_FILE_NAME);
                return ERR_TERM_NOT_BIG_ENOUGH;
            } else {
                go_to(pos_of(first_line_on_screen->line_nb + y, x));
            }
            break;
        }
    }
}
