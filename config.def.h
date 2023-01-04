// PARAMETERS ******************************************************************

#define MOUSE_SUPPORT               1
#define BACKUP_FILE_NAME            "edit_backup_file"
#define MIN_WIDTH                   81
#define RULER_WIDTH                 8
#define SCROLL_LINE_NUMBER          3


// DEFAULT VALUES FOR RUNTIME-MODIFIABLE PARAMETERS ****************************

#define AUTOINDENT                  1
#define SYNTAX_HIGHLIGHT            1
#define HIGHLIGHT_SELECTIONS        1
#define CASE_SENSITIVE              1
#define REPLACE_TABS                1
#define FIELD_SEPARATOR             ','
#define TAB_WIDTH                   4
#define LANGUAGE                    "none"


// COLORS **********************************************************************

// TODO


// KEYBINDS ********************************************************************

// general
#define KB_HELP                     '?'
#define KB_QUIT                     'q'
#define KB_WRITE                    'w'
#define KB_WRITE_AS                 'W'
#define KB_RELOAD                   'r'
#define KB_INSERT_MODE              'i'
#define KB_CHANGE_SETTING           's'

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
// CTRL + arrows   move cursor line <n> lines up/down

// movements
#define KB_MOVE_MATCHING            'm'
#define KB_MOVE_START_LINE          '0'
#define KB_MOVE_NON_BLANCK          '^'
#define KB_MOVE_END_LINE            '$'
#define KB_MOVE_SPECIFIC_LINE       'g'
#define KB_MOVE_END_FILE            'G'
#define KB_MOVE_NEXT_CHAR           'l' // RIGHT
#define KB_MOVE_PREV_CHAR           'h' // LEFT
#define KB_MOVE_NEXT_LINE           'j' // DOWN
#define KB_MOVE_PREV_LINE           'k' // UP
#define KB_MOVE_NEXT_WORD           't'
#define KB_MOVE_PREV_WORD           'T'
#define KB_MOVE_NEXT_BLOCK          '{'
#define KB_MOVE_PREV_BLOCK          '}'
#define KB_MOVE_NEXT_MATCH          'n'
#define KB_MOVE_PREV_MATCH          'N'

// selections
#define KB_SEL_DISPLAY_COUNT        'c'
#define KB_SEL_CURSOR_LINE          '.'
#define KB_SEL_CUSTOM_RANGE         ':'
#define KB_SEL_ALL_LINES            '%'
#define KB_SEL_LINES_BLOCK          'b'
#define KB_SEL_FIND                 'f'
#define KB_SEL_SEARCH               '/'
#define KB_SEL_ANCHOR               'v'
#define KB_SEL_APPEND               'a'
#define KB_SEL_COLUMN               'z'

// actions on selections
#define KB_ACT_INCREASE_INDENT      '>'
#define KB_ACT_DECREASE_INDENT      '<'
#define KB_ACT_COMMENT              'K'
#define KB_ACT_SUPPRESS             'x'
#define KB_ACT_REPLACE              'R'
#define KB_ACT_LOWERCASE            'u'
#define KB_ACT_UPPERCASE            'U'


// LANGUAGES SUPPORT ***********************************************************

// TODO (keywords, autoindent rules...)


// ERROR CODES *****************************************************************

// TODO
#define ERR_TERM_NOT_BIG_ENOUGH     1
#define ERR_MISSING_FILE_NAME       2
#define ERR_MALLOC                  3
#define ERR_TOO_LONG_LINE           4
#define ERR_INVALID_LINE_VALUE      5


// MISC ************************************************************************

#define BRACKETS                    "()[]{}"    // TODO
#define BLANKS                      " "
#define DELIMITERS                  ", "
