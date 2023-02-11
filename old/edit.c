#include "globals.h"
#include "utils.c"
#include "lines.c"
#include "file.c"
#include "movements.c"
#include "selections.c"
#include "actions.c"
#include "graphical.c"
#include "interaction.c"

int
main(int argc, char *argv[])
{
    int l1, old_line_nb;
    uint32_t c;
    struct pos p;


    // PARSING ARGUMENTS *******************************************************

    if (argc == 1) {
        printf("Refer to https://jacquin.xyz/edit for complete help.\n");
        return 0;
    } else if (argc == 2) {
        if (!(strcmp(argv[1], "--version") && strcmp(argv[1], "-v"))) {
            printf("%s\n", VERSION);
            return 0;
        } else if (!(strcmp(argv[1], "--help") && strcmp(argv[1], "-h"))) {
            printf("Refer to https://jacquin.xyz/edit for complete help.\n");
            return 0;
        } else {
            strcpy(file_name_int.current, argv[1]);
            strcpy(file_name_int.previous, file_name_int.current);
            get_extension();
            load_lang();
            read_only = 0;
        }
    } else if (argc == 3) {
        if (!(strcmp(argv[2], "--read-only") && strcmp(argv[1], "-r"))) {
            strcpy(file_name_int.current, argv[2]);
            strcpy(file_name_int.previous, file_name_int.current);
            get_extension();
            load_lang();
            read_only = 1;
        }
    } else {
        return ERR_BAD_ARGUMENTS;
    }


    // INIT VARIABLES **********************************************************

    // default settings
    settings.autoindent             = AUTOINDENT; // TODO
    settings.syntax_highlight       = SYNTAX_HIGHLIGHT;
    settings.highlight_selections   = HIGHLIGHT_SELECTIONS;
    settings.case_sensitive         = CASE_SENSITIVE; // TODO
    settings.field_separator        = FIELD_SEPARATOR;
    settings.tab_width              = TAB_WIDTH;
 
    // editor variables
    m = in_insert_mode = anchored = 0;
    strcpy(search_pattern.current, "");
    strcpy(search_pattern.previous, "");
    strcpy(replace_pattern.current, "");
    strcpy(replace_pattern.previous, "");
    strcpy(settings_int.current, "");
    strcpy(settings_int.previous, "");
    strcpy(range_int.current, "");
    strcpy(range_int.previous, "");
    sel = NULL;
    clipboard.start = NULL;
    is_bracket = 0;

    // initialise termbox
    x = y = 0;
    tb_init();
    tb_set_output_mode(OUTPUT_MODE);
    if (MOUSE_SUPPORT)
        tb_set_input_mode(TB_INPUT_ESC | TB_INPUT_MOUSE);
    if (resize(tb_width(), tb_height()))
        return ERR_TERM_NOT_BIG_ENOUGH;
    echo("Welcome to edit!");

    // load file
    load_file(file_name_int.current, 1);


    // MAIN LOOP ***************************************************************

    while (1) {
        delete_temp_sels();
        add_running_sels(1);
        print_all();
        tb_present();
        tb_poll_event(&ev);
        switch (ev.type) {
        case TB_EVENT_KEY:
            if (ev.ch && in_insert_mode) {
                act(insert, 0);
                has_been_changes = 1;
                go_to(pos_of(first_line_on_screen->line_nb + y, x + 1));
            } else if (ev.ch && !in_insert_mode) {
                if ((m && ev.ch == '0') || ('1' <= ev.ch && ev.ch <= '9')) {
                    m = 10*m + ev.ch - '0';
                    sprintf(dialog_chars, "Multiplier: %d", m);
                    break;
                } else {
                if (m == 0)
                    m = 1;
                switch (ev.ch) {
                case KB_HELP:
                    display_help();
                    break;
                case KB_QUIT:
                case KB_FORCE_QUIT:
                    if (ev.ch == KB_QUIT && has_been_changes) {
                        echo("There are unsaved changes.");
                    } else {
                        tb_shutdown();
                        return 0;
                    }
                    break;
                case KB_WRITE:
                    if (has_been_changes) {
                        write_file(file_name_int.current);
                        has_been_changes = 0;
                        echo("File saved.");
                    } else {
                        echo("No changes to write.");
                    }
                    break;
                case KB_WRITE_AS:
                    if (dialog("Save as: ", &file_name_int, 0)) {
                        write_file(file_name_int.current);
                        get_extension();
                        load_lang();
                        has_been_changes = 0;
                        echo("File saved.");
                    }
                    break;
                case KB_RELOAD:
                    if (has_been_changes) {
                        empty_sels();
                        old_line_nb = first_line_on_screen->line_nb + y;
                        load_file(file_name_int.current, first_line_on_screen->line_nb);
                        go_to(pos_of(old_line_nb, x));
                        has_been_changes = 0;
                        echo("File reloaded.");
                    } else {
                        echo("No changes to revert.");
                    }
                    break;
                case KB_INSERT_MODE:
                    in_insert_mode = 1;
                    echo("INSERT (ESC to exit)");
                    break;
                case KB_CHANGE_SETTING:
                    if (dialog("Change setting: ", &settings_int, 0))
                        if (!set_parameter(settings_int.current))
                            echo("Invalid assignment.");
                    break;
                case KB_INSERT_START_LINE:
                    empty_sels();
                    go_to(pos_of(first_line_on_screen->line_nb + y, 0));
                    in_insert_mode = 1;
                    echo("INSERT (ESC to exit)");
                    break;
                case KB_INSERT_END_LINE:
                    empty_sels();
                    go_to(pos_of(first_line_on_screen->line_nb + y,
                        get_line(y)->dl));
                    in_insert_mode = 1;
                    echo("INSERT (ESC to exit)");
                    break;
                case KB_INSERT_LINE_BELOW:
                    empty_sels();
                    insert_line(first_line_on_screen->line_nb + y + 1, 1);
                    go_to(pos_of(first_line_on_screen->line_nb + y + 1, 0));
                    in_insert_mode = 1;
                    has_been_changes = 1;
                    echo("INSERT (ESC to exit)");
                    break;
                case KB_INSERT_LINE_ABOVE:
                    empty_sels();
                    insert_line(first_line_on_screen->line_nb + y, 1);
                    go_to(pos_of(first_line_on_screen->line_nb + y, 0));
                    in_insert_mode = 1;
                    has_been_changes = 1;
                    echo("INSERT (ESC to exit)");
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
                    while (m--)
                        insert_clip(get_line(y), 1);
                    has_been_changes = 1;
                    break;
                case KB_CLIP_PASTE_BEFORE:
                    while (m--)
                        insert_clip(get_line(y), 0);
                    has_been_changes = 1;
                    break;
                case KB_MOVE_MATCHING:
                    go_to(find_matching_bracket());
                    break;
                case KB_MOVE_START_LINE:
                    go_to(pos_of(first_line_on_screen->line_nb + y, 0));
                    break;
                case KB_MOVE_NON_BLANCK:
                    go_to(find_first_non_blanck());
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
                    // if ((p = find_next_selection(m)).l) {
                    //     go_to(p);
                    // } else {
                    //     echo("No more selections downwards.");
                    // }
                    break;
                case KB_MOVE_PREV_SEL:
                    // if ((p = find_next_selection(-m)).l) {
                    //     go_to(p);
                    // } else {
                    //     echo("No more selections upwards.");
                    // }
                    break;
                case KB_SEL_DISPLAY_COUNT:
                    sprintf(dialog_chars, "%d selections.", nb_sels());
                    break;
                case KB_SEL_CURSOR_LINE:
                    add_range_sels(first_line_on_screen->line_nb + y,
                        first_line_on_screen->line_nb + y, 0);
                    break;
                case KB_SEL_CUSTOM_RANGE:
                    if (dialog("Lines range: ", &range_int, 0))
                        if (!parse_range(range_int.current))
                            echo("Invalid range.");
                    break;
                case KB_SEL_ALL_LINES:
                    add_range_sels(1, nb_lines, 0);
                    break;
                case KB_SEL_LINES_BLOCK:
                    l1 = find_start_of_block(first_line_on_screen->line_nb + y, 1);
                    add_range_sels(l1, find_end_of_block(l1, m), 0);
                    break;
                case KB_SEL_FIND:
                case KB_SEL_SEARCH:
                    // TODO
                    if (dialog("Search pattern: ", &search_pattern, 0)) {
                    }
                    break;
                case KB_SEL_ANCHOR:
                    if (anchored) {
                        anchored = 0;
                    } else {
                        anchor = pos_of_curs();
                        anchored = 1;
                    }
                    break;
                case KB_SEL_APPEND:
                    add_running_sels(0);
                    anchored = 0;
                    break;
                case KB_SEL_COLUMN:
                    // TODO
                    break;
                case KB_ACT_INCREASE_INDENT:
                    asked_indent = m * settings.tab_width;
                    act(indent, 1);
                    break;
                case KB_ACT_DECREASE_INDENT:
                    asked_indent = - m * settings.tab_width;
                    act(indent, 1);
                    break;
                case KB_ACT_COMMENT:
                    if (strcmp(settings.language, "none"))
                        act(comment, 1);
                    break;
                case KB_ACT_SUPPRESS:
                    asked_remove = m;
                    act(suppress, 1);
                    break;
                case KB_ACT_REPLACE:
                    // TODO
                    if (dialog("Replace pattern: ", &replace_pattern, 0)) {
                    }
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
                        empty_sels();
                        anchored = 0;
                    }
                    echo("");
                    break;
                case TB_KEY_ENTER:
                    if (in_insert_mode) {
                        empty_sels();
                        anchored = 0;
                        add_running_sels(1);
                        act(split_lines, 0);
                        go_to(pos_of(first_line_on_screen->line_nb + y + 1, 0));
                    } else {
                        go_to(pos_of(first_line_on_screen->line_nb + y + m, x));
                    }
                    break;
                case TB_KEY_BACKSPACE:
                case TB_KEY_BACKSPACE2:
                    asked_remove = -m;
                    act(suppress, 1);
                    go_to(pos_of(first_line_on_screen->line_nb + y, x + asked_remove));
                    break;
                case TB_KEY_DELETE:
                    asked_remove = m;
                    act(suppress, 1);
                    break;
                case TB_KEY_TAB:
                    asked_indent = m * settings.tab_width;
                    act(indent, 1);
                    if (in_insert_mode)
                        go_to(pos_of(first_line_on_screen->line_nb + y, x + asked_indent));
                    break;
                case TB_KEY_BACK_TAB:
                    asked_indent = - m * settings.tab_width;
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
                if (old_line_nb - first_line_on_screen->line_nb > screen_height - 2) {
                    go_to(pos_of(first_line_on_screen->line_nb + screen_height - 2, x));
                } else {
                    go_to(pos_of(old_line_nb, x));
                }
                break;
            case TB_KEY_MOUSE_WHEEL_DOWN:
                old_line_nb = first_line_on_screen->line_nb + y;
                first_line_on_screen = get_line(SCROLL_LINE_NUMBER);
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
