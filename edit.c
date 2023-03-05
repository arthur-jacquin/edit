#include "globals.h"
#include "utils.c"
#include "file.c"
#include "movements.c"
#include "selections.c"
#include "lines.c"
#include "actions.c"
#include "search_and_replace.c"
#include "interaction.c"
#include "graphical.c"

int
main(int argc, char *argv[])
{
    int l1, l2, old_line_nb;
    struct pos p;

    // INIT VARIABLES **********************************************************

    // settings and interfaces
    settings.syntax_highlight = SYNTAX_HIGHLIGHT;
    settings.highlight_selections = HIGHLIGHT_SELECTIONS;
    settings.case_sensitive = CASE_SENSITIVE;
    settings.field_separator = FIELD_SEPARATOR;
    settings.tab_width = TAB_WIDTH;
    init_interface(range_int, "");
    init_interface(settings_int, "");
    init_interface(search_pattern, "");
    init_interface(replace_pattern, "");

    // editor variables
    y = x = 0;
    m = in_insert_mode = anchored = is_bracket = has_been_invalid_resizing = 0;
    saved = running = displayed = NULL;
    clipboard.start = NULL;

    // initialise termbox
    init_termbox();
    echo(WELCOME_MESSAGE);


    // PARSING ARGUMENTS *******************************************************

    if (argc < 2 || !(strcmp(argv[1], "--help") && strcmp(argv[1], "-h"))) {
        printf("%s\n", HELP_MESSAGE);
        return 0;
    } else if (!(strcmp(argv[1], "--version") && strcmp(argv[1], "-v"))) {
        printf("%s\n", VERSION);
        return 0;
    } else {
        init_interface(file_name_int, argv[1]);
        load_file(1);
    }


    // MAIN LOOP ***************************************************************

    while (1) {
        // quit if has been invalid resizing
        if (has_been_invalid_resizing) {
            if (has_been_changes)
                write_file(BACKUP_FILE_NAME);
            tb_shutdown();
            return ERR_TERM_NOT_BIG_ENOUGH;
        }

        // compute new displayed selections
        forget_sel_list(running);
        running = running_sel();
        forget_sel_list(displayed);
        displayed = merge_sel(running, saved);

        // go to correct position, refresh screen and wait for input
        move_to_cursor();
        print_all();
        tb_present();
        tb_poll_event(&ev);

        // process input
        switch (ev.type) {
        case TB_EVENT_KEY:
            if (ev.ch && in_insert_mode) {
                act(insert, 0);
            } else if (ev.ch && !in_insert_mode) {
                if ((m && ev.ch == '0') || ('1' <= ev.ch && ev.ch <= '9')) {
                    m = 10*m + ev.ch - '0';
                    echof(MULTIPLIER_MESSAGE_PATTERN, m);
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
                        old_line_nb = first_line_nb + y;
                        load_file(first_line_nb);
                        y = old_line_nb - first_line_nb;
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
                case KB_RUN_SHELL_COMMAND:
                    tb_shutdown();
                    system(SHELL_COMMAND);
                    getchar();
                    init_termbox();
                    break;
                case KB_INSERT_START_LINE:
                case KB_INSERT_END_LINE:
                    reset_selections();
                    x = (ev.ch == KB_INSERT_END_LINE) ? get_line(y)->dl : 0;
                    in_insert_mode = 1;
                    echo(INSERT_MODE_MESSAGE);
                    break;
                case KB_INSERT_LINE_BELOW:
                case KB_INSERT_LINE_ABOVE:
                    reset_selections();
                    in_insert_mode = 1;
                    echo(INSERT_MODE_MESSAGE);
                    y += (ev.ch == KB_INSERT_LINE_BELOW) ? 1 : 0;
                    insert_line(first_line_nb + y, 1, 0);
                    break;
                case KB_CLIP_YANK_LINE:
                    copy_to_clip(first_line_nb + y, m);
                    break;
                case KB_CLIP_YANK_BLOCK:
                    l1 = find_start_of_block(first_line_nb + y, 1);
                    copy_to_clip(l1, find_end_of_block(l1, m) - l1 + 1);
                    break;
                case KB_CLIP_DELETE_LINE:
                    move_to_clip(first_line_nb + y, m);
                    break;
                case KB_CLIP_DELETE_BLOCK:
                    l1 = find_start_of_block(first_line_nb + y, 1);
                    move_to_clip(l1, find_end_of_block(l1, m) - l1 + 1);
                    break;
                case KB_CLIP_PASTE_AFTER:
                case KB_CLIP_PASTE_BEFORE:
                    while (m--)
                        insert_clip(get_line(y), ev.ch == KB_CLIP_PASTE_AFTER);
                    break;
                case KB_MOVE_MATCHING:
                    unwrap_pos(find_matching_bracket());
                    break;
                case KB_MOVE_START_LINE:
                    x = 0;
                    break;
                case KB_MOVE_NON_BLANK:
                    x = find_first_non_blank();
                    break;
                case KB_MOVE_END_LINE:
                    x = get_line(y)->dl;
                    break;
                case KB_MOVE_END_FILE:
                    m = nb_lines;
                    // fall-through
                case KB_MOVE_SPECIFIC_LINE:
                    y = m - first_line_nb;
                    break;
                case KB_MOVE_NEXT_CHAR:
                    x += m;
                    break;
                case KB_MOVE_PREV_CHAR:
                    x -= m;
                    break;
                case KB_MOVE_NEXT_LINE:
                    y += m;
                    break;
                case KB_MOVE_PREV_LINE:
                    y -= m;
                    break;
                case KB_MOVE_NEXT_WORD:
                case KB_MOVE_PREV_WORD:
                    unwrap_pos(find_start_of_word(
                        (ev.ch == KB_MOVE_NEXT_WORD) ? m: -m));
                    break;
                case KB_MOVE_NEXT_BLOCK:
                    y = find_end_of_block(first_line_nb + y, m)
                        - first_line_nb;
                    break;
                case KB_MOVE_PREV_BLOCK:
                    y = find_start_of_block(first_line_nb + y, m)
                        - first_line_nb;
                    break;
                case KB_MOVE_NEXT_SEL:
                    if ((p = find_next_selection(m)).l)
                        unwrap_pos(p);
                    else
                        echo(NO_SEL_DOWN_MESSAGE);
                    break;
                case KB_MOVE_PREV_SEL:
                    if ((p = find_next_selection(-m)).l)
                        unwrap_pos(p);
                    else
                        echo(NO_SEL_UP_MESSAGE);
                    break;
                case KB_SEL_DISPLAY_COUNT:
                    echof(SELECTIONS_MESSAGE_PATTERN, nb_sel(saved));
                    break;
                case KB_SEL_CURSOR_LINE:
                case KB_SEL_ALL_LINES:
                case KB_SEL_LINES_BLOCK:
                    if (ev.ch == KB_SEL_CURSOR_LINE) {
                        l1 = l2 = first_line_nb + y;
                    } else if (ev.ch == KB_SEL_ALL_LINES) {
                        l1 = 1;
                        l2 = nb_lines;
                    } else {
                        l1 = find_start_of_block(first_line_nb + y, 1);
                        l2 = find_end_of_block(l1, m);
                    }
                    forget_sel_list(saved);
                    saved = range_lines_sel(l1, l2, NULL);
                    break;
                case KB_SEL_CUSTOM_RANGE:
                    if (dialog(RANGE_PROMPT, &range_int, 0))
                        if (!parse_range(range_int.current))
                            echo(INVALID_RANGE_MESSAGE);
                    break;
                case KB_SEL_FIND:
                case KB_SEL_SEARCH:
                case KB_SEL_APPEND:
                    if (ev.ch == KB_SEL_APPEND ||
                        dialog(SEARCH_PATTERN_PROMPT, &search_pattern, 1)) {
                        forget_sel_list(saved);
                        saved = displayed;
                        displayed = NULL;
                        anchored = 0;
                    }
                    break;
                case KB_SEL_CURSOR_WORD:
                    if (!search_word_under_cursor())
                        echo(NO_WORD_CURSOR_MESSAGE);
                    break;
                case KB_SEL_ANCHOR:
                    if (anchored) {
                        anchored = 0;
                    } else {
                        anchor = pos_of_cursor();
                        anchored = 1;
                    }
                    break;
                case KB_SEL_COLUMN:
                    if (anchored && anchor.l != pos_of_cursor().l)
                        echo(COLUMN_SEL_ERROR_MESSAGE);
                    else
                        unwrap_pos(column_sel(m));
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
                    x += m;
                    break;
                case TB_KEY_ARROW_LEFT:
                    x -= m;
                    break;
                case TB_KEY_ARROW_DOWN:
                    if (ev.mod == TB_MOD_SHIFT)
                        move_line(m);
                    else
                        y += m;
                    break;
                case TB_KEY_ARROW_UP:
                    if (ev.mod == TB_MOD_SHIFT)
                        move_line(-m);
                    else
                        y -= m;
                    break;
                case TB_KEY_ESC:
                    if (in_insert_mode)
                        in_insert_mode = 0;
                    else
                        reset_selections();
                    echo("");
                    break;
                case TB_KEY_ENTER:
                    if (in_insert_mode)
                        act(split, 0);
                    else
                        y += m;
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

#ifdef MOUSE_SUPPORT
        case TB_EVENT_MOUSE:
            switch (ev.key) {
            case TB_KEY_MOUSE_LEFT:
                if (ev.y < screen_height - 1) {
                    y = ev.y;
                    x = ev.x - LINE_NUMBERS_WIDTH;
                }
                break;
            case TB_KEY_MOUSE_WHEEL_UP:
                old_line_nb = first_line_nb + y;
                first_line_on_screen = get_line(-SCROLL_LINE_NUMBER);
                y = MIN(old_line_nb - first_line_nb,
                    screen_height - 2 - scroll_offset);
                break;
            case TB_KEY_MOUSE_WHEEL_DOWN:
                old_line_nb = first_line_nb + y;
                first_line_on_screen = get_line(SCROLL_LINE_NUMBER);
                y = MAX(old_line_nb - first_line_nb, scroll_offset);
                break;
            }
            break;
#endif // MOUSE_SUPPORT

        case TB_EVENT_RESIZE:
            if (resize(ev.w, ev.h))
                has_been_invalid_resizing = 1;
            break;
        }
    }
}

