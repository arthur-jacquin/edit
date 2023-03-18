#include "globals.h"
#include "languages.h"
#define TB_IMPL
#include "termbox.h"

#include "actions.c"
#include "file.c"
#include "graphical.c"
#include "interaction.c"
#include "lines.c"
#include "marks.c"
#include "movements.c"
#include "selections.c"
#include "utils.c"

#define VERSION                     "alpha"
#define HELP_MESSAGE                "Help available at https://jacquin.xyz/edit"
#define INIT_INTERFACE(I, S)        strcpy(I.current, S); strcpy(I.previous, S);
#define MOVE_SEL_LIST(A, B)         forget_sel_list(B); B = A; A = NULL;
#define echo(MESSAGE)               strcpy(message, MESSAGE)
#define echof(PATTERN, INTEGER)     sprintf(message, PATTERN, INTEGER)
#define way(DIRECT_CONDITION)       ((DIRECT_CONDITION) ? m : -m)

int
main(int argc, char *argv[])
{
    struct tb_event ev;             // struct to retrieve events
    struct interface range_int;     // interface for custom range of lines
    struct interface settings_int;  // interface for changing a setting
    struct interface command_int;   // interface for running a shell command
    int m;                          // multiplier

    int l1, l2, old_line_nb;
    struct pos p;


    // INIT VARIABLES **********************************************************

    y = x = 0; attribute_x = 1;
    saved = running = displayed = NULL;
    m = anchored = has_been_invalid_resizing = in_insert_mode = 0;
    empty_clip(0);

    settings.case_sensitive = CASE_SENSITIVE;
    settings.field_separator = FIELD_SEPARATOR;
    settings.highlight_selections = HIGHLIGHT_SELECTIONS;
    settings.syntax_highlight = SYNTAX_HIGHLIGHT;
    settings.tab_width = TAB_WIDTH;

    INIT_INTERFACE(search_pattern, "")
    INIT_INTERFACE(replace_pattern, "")
    INIT_INTERFACE(range_int, "")
    INIT_INTERFACE(settings_int, "")
    INIT_INTERFACE(command_int, "")

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
        INIT_INTERFACE(file_name_int, argv[1])
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

        // go to correct position, compute new displayed selections
        move_to_cursor();
        forget_sel_list(running);
        running = running_sel();
        forget_sel_list(displayed);
        displayed = merge_sel(running, saved);

        // refresh screen and wait for input
        print_all();
        tb_present();
        tb_poll_event(&ev);

        // process input
        if (!in_insert_mode && m == 0)
            echo("");
        switch (ev.type) {
        case TB_EVENT_KEY:
            if (ev.ch && in_insert_mode) {
                to_insert = ev.ch;
                act(insert, 0);
                break;
            } else if ((m && ev.ch == '0') || ('1' <= ev.ch && ev.ch <= '9')) {
                m = 10*m + ev.ch - '0';
                echof(MULTIPLIER_MESSAGE_PATTERN, m);
                break;
            }
            if (m == 0)
                m = 1;
            if (ev.ch) {
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
                case KB_WRITE_AS:
                    if (ev.ch == KB_WRITE && !has_been_changes) {
                        echo(NOTHING_TO_WRITE_MESSAGE);
                    } else if (ev.ch == KB_WRITE ||
                        dialog(SAVE_AS_PROMPT, &file_name_int, 0)) {
                        write_file(file_name_int.current);
                        if (ev.ch == KB_WRITE_AS)
                            parse_lang(file_name_int.current);
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
                case KB_CHANGE_SETTING:
                    if (dialog(CHANGE_SETTING_PROMPT, &settings_int, 0))
                        if (!parse_assign(settings_int.current))
                            echo(INVALID_ASSIGNMENT_MESSAGE);
                    break;
                case KB_RUN_SHELL_COMMAND:
                    if (dialog(COMMAND_PROMPT, &command_int, 0)) {
                        tb_shutdown();
                        system(command_int.current);
                        getchar();
                        init_termbox();
                    }
                    break;
                case KB_INSERT_START_LINE:
                case KB_INSERT_END_LINE:
                case KB_INSERT_LINE_BELOW:
                case KB_INSERT_LINE_ABOVE:
                    reset_selections();
                    if (ev.ch == KB_INSERT_LINE_BELOW ||
                        ev.ch == KB_INSERT_LINE_ABOVE) {
                        y += (ev.ch == KB_INSERT_LINE_BELOW) ? 1 : 0;
                        insert_line(first_line_nb + y, 1, 0);
                    }
                    x = (ev.ch == KB_INSERT_END_LINE) ? get_line(y)->dl : 0;
                    attribute_x = 1;
                    // fall-through
                case KB_INSERT_MODE:
                    in_insert_mode = 1;
                    echo(INSERT_MODE_MESSAGE);
                    break;
                case KB_MOVE_MATCHING:
                    unwrap_pos(find_matching_bracket());
                    break;
                case KB_MOVE_START_LINE:
                    x = 0; attribute_x = 1;
                    break;
                case KB_MOVE_NON_BLANK:
                    x = find_first_non_blank(); attribute_x = 1;
                    break;
                case KB_MOVE_END_LINE:
                    x = get_line(y)->dl; attribute_x = 1;
                    break;
                case KB_MOVE_END_FILE:
                    m = nb_lines;
                    // fall-through
                case KB_MOVE_SPECIFIC_LINE:
                    y = m - first_line_nb;
                    break;
                case KB_MOVE_NEXT_CHAR:
                case KB_MOVE_PREV_CHAR:
                    x += way(ev.ch == KB_MOVE_NEXT_CHAR); attribute_x = 1;
                    break;
                case KB_MOVE_NEXT_LINE:
                case KB_MOVE_PREV_LINE:
                    y += way(ev.ch == KB_MOVE_NEXT_LINE);
                    break;
                case KB_MOVE_NEXT_WORD:
                case KB_MOVE_PREV_WORD:
                    unwrap_pos(find_start_of_word(
                        way(ev.ch == KB_MOVE_NEXT_WORD)));
                    break;
                case KB_MOVE_NEXT_BLOCK:
                case KB_MOVE_PREV_BLOCK:
                    y = find_block_delim(first_line_nb + y,
                        way(ev.ch == KB_MOVE_NEXT_BLOCK)) - first_line_nb;
                    break;
                case KB_MOVE_NEXT_SEL:
                case KB_MOVE_PREV_SEL:
                    p = find_next_selection(way(ev.ch == KB_MOVE_NEXT_SEL));
                    if (p.l)
                        unwrap_pos(p);
                    else
                        echo((ev.ch == KB_MOVE_NEXT_SEL) ?
                            NO_SEL_DOWN_MESSAGE : NO_SEL_UP_MESSAGE);
                    break;
                case KB_MOVE_JUMP_TO_NEXT:
                    MOVE_SEL_LIST(saved, running)
                    saved = range_lines_sel(first_line_nb + y, nb_lines, NULL);
                    if (dialog(SEARCH_PATTERN_PROMPT, &search_pattern, 1)) {
                        MOVE_SEL_LIST(displayed, saved);
                        if ((p = find_next_selection(m)).l)
                            unwrap_pos(p);
                        else
                            echo(NO_SEL_DOWN_MESSAGE);
                    }
                    MOVE_SEL_LIST(running, saved)
                    break;
                case KB_SEL_ANCHOR:
                    if (!anchored)
                        anchor = pos_of_cursor();
                    anchored = 1 - anchored;
                    break;
                case KB_SEL_COLUMN:
                    if (anchored && anchor.l != first_line_nb + y)
                        echo(COLUMN_SEL_ERROR_MESSAGE);
                    else
                        unwrap_pos(column_sel(m));
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
                        l1 = find_block_delim(first_line_nb + y, -1);
                        l2 = find_block_delim(l1, m);
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
                    if (ev.ch == KB_SEL_APPEND)
                        anchored = 0;
                    if (ev.ch == KB_SEL_APPEND ||
                        dialog(SEARCH_PATTERN_PROMPT, &search_pattern, 1)) {
                        MOVE_SEL_LIST(displayed, saved)
                    }
                    break;
                case KB_SEL_CURSOR_WORD:
                    if (!search_word_under_cursor())
                        echo(NO_WORD_CURSOR_MESSAGE);
                    break;
                case KB_ACT_SUPPRESS:
                    asked_remove = m;
                    act(suppress, 0);
                    break;
                case KB_ACT_LOWERCASE:
                    act(lower, 0);
                    break;
                case KB_ACT_UPPERCASE:
                    act(upper, 0);
                    break;
                case KB_ACT_INCREASE_INDENT:
                case KB_ACT_DECREASE_INDENT:
                    asked_indent = settings.tab_width *
                        way(ev.ch == KB_ACT_INCREASE_INDENT);
                    act(indent, 1);
                    break;
                case KB_ACT_COMMENT:
                    if (settings.syntax != NULL &&
                        settings.syntax->highlight_elements)
                        act(comment, 1);
                    break;
                case KB_ACT_REPLACE:
                    if (dialog(REPLACE_PATTERN_PROMPT, &replace_pattern, 0))
                        act(replace, 0);
                    break;
                case KB_CLIP_YANK_LINE:
                case KB_CLIP_YANK_BLOCK:
                case KB_CLIP_DELETE_LINE:
                case KB_CLIP_DELETE_BLOCK:
                    if (ev.ch == KB_CLIP_YANK_LINE ||
                        ev.ch == KB_CLIP_DELETE_LINE) {
                        l1 = first_line_nb + y;
                    } else {
                        l1 = find_block_delim(first_line_nb + y, -1);
                        m = find_block_delim(l1, m) - l1 + 1;
                    }
                    if (ev.ch == KB_CLIP_YANK_LINE ||
                        ev.ch == KB_CLIP_YANK_BLOCK) {
                        copy_to_clip(l1, m);
                    } else {
                        move_to_clip(l1, m);
                    }
                    break;
                case KB_CLIP_PASTE_AFTER:
                case KB_CLIP_PASTE_BEFORE:
                    while (m--)
                        insert_clip(get_line(y), ev.ch == KB_CLIP_PASTE_AFTER);
                    break;
                }
            } else if (ev.key) {
                switch (ev.key) {
                case TB_KEY_ENTER:
                    if (in_insert_mode)
                        act(split, 0);
                    else
                        y += m;
                    break;
                case TB_KEY_ARROW_RIGHT:
                case TB_KEY_ARROW_LEFT:
                    x += way(ev.key == TB_KEY_ARROW_RIGHT); attribute_x = 1;
                    break;
                case TB_KEY_ARROW_DOWN:
                case TB_KEY_ARROW_UP:
                    if (ev.mod == TB_MOD_SHIFT)
                        move_line(way(ev.key == TB_KEY_ARROW_DOWN));
                    else
                        y += way(ev.key == TB_KEY_ARROW_DOWN);
                    break;
                case TB_KEY_ESC:
                    if (in_insert_mode)
                        in_insert_mode = 0;
                    else
                        reset_selections();
                    echo("");
                    break;
                case TB_KEY_BACKSPACE:
                case TB_KEY_BACKSPACE2:
                case TB_KEY_DELETE:
                    asked_remove = way(ev.key == TB_KEY_DELETE);
                    act(suppress, 0);
                    break;
                case TB_KEY_TAB:
                case TB_KEY_BACK_TAB:
                    asked_indent = settings.tab_width*way(ev.key == TB_KEY_TAB);
                    act(indent, 1);
                    break;
#ifdef ENABLE_AUTOCOMPLETE
                case KB_ACT_AUTOCOMPLETE:
                    act(autocomplete, 0);
                    break;
#endif // ENABLE_AUTOCOMPLETE
                }
            }
            m = 0;
            break;

#ifdef MOUSE_SUPPORT
        case TB_EVENT_MOUSE:
            switch (ev.key) {
            case TB_KEY_MOUSE_LEFT:
                if (ev.y < screen_height - 1) {
                    y = ev.y;
                    x = ev.x - LINE_NUMBERS_WIDTH; attribute_x = 1;
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
