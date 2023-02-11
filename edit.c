#include "globals.h"
#include "utils.c"
#include "file.c"
#include "lines.c"
#include "interaction.c"
#include "graphical.c"

int
main(int argc, char *argv[])
{

    // PARSING ARGUMENTS *******************************************************
    // TODO: better error detection, starting with -

    if (argc < 2 || !(strcmp(argv[1], "--help") && strcmp(argv[1], "-h"))) {
        printf("Refer to https://jacquin.xyz/edit for complete help.\n");
        return 0;
    } else if (!(strcmp(argv[1], "--version") && strcmp(argv[1], "-v"))) {
        printf("%s\n", VERSION);
        return 0;
    } else {
        init_interface(&file_name_int, argv[1]);
        get_extension();
        load_lang();
    }


    // INIT VARIABLES **********************************************************

    // settings
    settings.autoindent = AUTOINDENT;
    settings.tab_width = TAB_WIDTH;
    init_interface(&settings_int, "");
 
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
    echo("Welcome to edit!");

    // load file
    load_file(file_name_int.current, 1);

    // display test
    print_all();
    tb_present();
    tb_poll_event(&ev);
    tb_shutdown();

    return 0;
}
