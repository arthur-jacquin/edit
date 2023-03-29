// see LICENSE file for copyright and license details

// MESSAGES AND PROMPTS ********************************************************

#define WELCOME_MESSAGE             "Welcome to edit!"
#define RULER_PATTERN               "%d:%d"
#define FILE_CONNECTION_MESSAGE     "The file can not be accessed."
#define UNSAVED_CHANGES_MESSAGE     "There are unsaved changes."
#define SAVE_AS_PROMPT              "Save as: "
#define FILE_SAVED_MESSAGE          "File saved."
#define NOTHING_TO_WRITE_MESSAGE    "No changes to write."
#define FILE_RELOADED_MESSAGE       "File reloaded."
#define NOTHING_TO_REVERT_MESSAGE   "No changes to revert."
#define SELECTIONS_MESSAGE_PATTERN  "%d saved selections."
#define NO_SEL_DOWN_MESSAGE         "No more selections downwards."
#define NO_SEL_UP_MESSAGE           "No more selections upwards."
#define INSERT_MODE_MESSAGE         "INSERT (ESC to exit)"
#define SEARCH_PATTERN_PROMPT       "Search pattern: "
#define REPLACE_PATTERN_PROMPT      "Replace pattern: "
#define NO_WORD_CURSOR_MESSAGE      "There is no word under the cursor."
#define MULTIPLIER_MESSAGE_PATTERN  "Multiplier: %d"
#define RANGE_PROMPT                "Lines range: "
#define INVALID_RANGE_MESSAGE       "Invalid range."
#define COLUMN_SEL_ERROR_MESSAGE    "Not possible: multiline running selection."
#define CHANGE_SETTING_PROMPT       "Change setting: "
#define INVALID_ASSIGNMENT_MESSAGE  "Invalid assignment."
#define COMMAND_PROMPT              "Run: "


// COMPILE-TIME PARAMETERS *****************************************************

// might be defined or not
#define MOUSE_SUPPORT
#define REMEMBER_CURSOR_COLUMN
#define UNDERLINE_CURSOR_LINE
#define HIGHLIGHT_MATCHING_BRACKET
#define VISUAL_COLUMN               79
#define ENABLE_AUTOCOMPLETE

// must de defined (but can be set to 0 to disable)
#define LINE_NUMBERS_WIDTH          4
#define SCROLL_OFFSET               3
#define SCROLL_LINE_NUMBER          3

// must de defined
#define MIN_WIDTH                   80
#define RULER_WIDTH                 8
#define BACKUP_FILE_NAME            "edit_backup_file"


// DEFAULT VALUES FOR RUNTIME-MODIFIABLE PARAMETERS ****************************

#define SYNTAX_HIGHLIGHT            1
#define HIGHLIGHT_SELECTIONS        1
#define CASE_SENSITIVE              1
#define FIELD_SEPARATOR             ','
#define TAB_WIDTH                   4


// COLORS **********************************************************************

#define TERM_256_COLORS_SUPPORT

// 256 colors mode: available colors are listed at https://jacquin.xyz/colors
#ifdef TERM_256_COLORS_SUPPORT
#define OUTPUT_MODE                 TB_OUTPUT_256
#define COLOR_DEFAULT               15
#define COLOR_RULER                 COLOR_DEFAULT //
#define COLOR_DIALOG                COLOR_DEFAULT //
#define COLOR_LINE_NUMBERS          238 //
#define COLOR_KEYWORD               99 //
#define COLOR_FLOW_CONTROL          32 //
#define COLOR_BUILT_IN              32 //
#define COLOR_NUMBER                99 //
#define COLOR_STRING                99 //
#define COLOR_COMMENT               172 //
#define COLOR_BG_DEFAULT            0
#define COLOR_BG_SELECTIONS         238 //
#define COLOR_BG_MATCHING           45 //
#define COLOR_BG_COLUMN             234 //

// 8 colors mode: available colors are TB_BLACK, TB_RED, TB_GREEN, TB_YELLOW,
// TB_BLUE, TB_MAGENTA, TB_CYAN, and TB_WHITE
#else
#define OUTPUT_MODE                 TB_OUTPUT_NORMAL
#define COLOR_DEFAULT               TB_WHITE
#define COLOR_RULER                 COLOR_DEFAULT
#define COLOR_DIALOG                COLOR_DEFAULT
#define COLOR_LINE_NUMBERS          COLOR_DEFAULT 
#define COLOR_KEYWORD               TB_BLUE
#define COLOR_FLOW_CONTROL          TB_CYAN
#define COLOR_BUILT_IN              COLOR_DEFAULT
#define COLOR_NUMBER                TB_GREEN
#define COLOR_STRING                COLOR_NUMBER
#define COLOR_COMMENT               TB_YELLOW
#define COLOR_BG_DEFAULT            TB_BLACK
#define COLOR_BG_SELECTIONS         TB_MAGENTA
#define COLOR_BG_MATCHING           COLOR_BG_DEFAULT
#define COLOR_BG_COLUMN             COLOR_BG_DEFAULT
#endif


// KEYBINDS ********************************************************************

// general
#define KB_HELP                     '?'
#define KB_QUIT                     'q'
#define KB_FORCE_QUIT               'Q'
#define KB_WRITE                    'w'
#define KB_WRITE_AS                 'W'
#define KB_RELOAD                   'R'
#define KB_INSERT_MODE              'i'
#define KB_CHANGE_SETTING           's'
#define KB_RUN_SHELL_COMMAND        'e'

// get in insert mode after removing selections
#define KB_INSERT_START_LINE        'I'
#define KB_INSERT_END_LINE          'A'
#define KB_INSERT_LINE_BELOW        'o'
#define KB_INSERT_LINE_ABOVE        'O'

// lines clipboard
#define KB_CLIP_YANK_LINE           'y'
#define KB_CLIP_YANK_BLOCK          'Y'
#define KB_CLIP_DELETE_LINE         'd'
#define KB_CLIP_DELETE_BLOCK        'D'
#define KB_CLIP_PASTE_AFTER         'p'
#define KB_CLIP_PASTE_BEFORE        'P'

// movements
#define KB_MOVE_MATCHING            'm'
#define KB_MOVE_START_LINE          '0'
#define KB_MOVE_NON_BLANK           '^'
#define KB_MOVE_END_LINE            '$'
#define KB_MOVE_SPECIFIC_LINE       'g'
#define KB_MOVE_END_FILE            'G'
#define KB_MOVE_NEXT_CHAR           'l'
#define KB_MOVE_PREV_CHAR           'h'
#define KB_MOVE_NEXT_LINE           'j'
#define KB_MOVE_PREV_LINE           'k'
#define KB_MOVE_NEXT_WORD           't'
#define KB_MOVE_PREV_WORD           'T'
#define KB_MOVE_NEXT_BLOCK          '}'
#define KB_MOVE_PREV_BLOCK          '{'
#define KB_MOVE_NEXT_SEL            'n'
#define KB_MOVE_PREV_SEL            'N'
#define KB_MOVE_JUMP_TO_NEXT        'J'

// selections
#define KB_SEL_DISPLAY_COUNT        'c'
#define KB_SEL_CURSOR_LINE          '.'
#define KB_SEL_CUSTOM_RANGE         ':'
#define KB_SEL_ALL_LINES            '%'
#define KB_SEL_LINES_BLOCK          'b'
#define KB_SEL_FIND                 'f'
#define KB_SEL_SEARCH               '/'
#define KB_SEL_CURSOR_WORD          '*'
#define KB_SEL_ANCHOR               'v'
#define KB_SEL_APPEND               'a'
#define KB_SEL_COLUMN               'z'

// actions on selections
#define KB_ACT_INCREASE_INDENT      '>'
#define KB_ACT_DECREASE_INDENT      '<'
#define KB_ACT_COMMENT              'K'
#define KB_ACT_SUPPRESS             'x'
#define KB_ACT_REPLACE              'r'
#define KB_ACT_LOWERCASE            'u'
#define KB_ACT_UPPERCASE            'U'

// autocomplete key (in both normal and insert modes)
#define KB_ACT_AUTOCOMPLETE         TB_KEY_CTRL_A


// LANGUAGES *******************************************************************

// Please define at least one language. If a language you want isn't supported
// yet, you can add it yourself in languages.h. Currently supported languages:
// C, MARKDOWN, PYTHON

#define C
#define MARKDOWN
#define PYTHON
