// MESSAGES AND PROMPTS ********************************************************

#define WELCOME_MESSAGE             "Welcome to edit!"
#define FILE_CONNECTION_MESSAGE     "The file does not exist or can not be accessed."
#define FILE_SAVED_MESSAGE          "File saved."
#define FILE_RELOADED_MESSAGE       "File reloaded."
#define INSERT_MODE_MESSAGE         "INSERT (ESC to exit)"
#define UNSAVED_CHANGES_MESSAGE     "There are unsaved changes."
#define NOTHING_TO_WRITE_MESSAGE    "No changes to write."
#define NOTHING_TO_REVERT_MESSAGE   "No changes to revert."
#define NO_SEL_DOWN_MESSAGE         "No more selections downwards."
#define NO_SEL_UP_MESSAGE           "No more selections upwards."
#define COLUMN_SEL_ERROR_MESSAGE    "Not possible when multiline running selection."
#define MULTIPLIER_MESSAGE_PATTERN  "Multiplier: %d"
#define SELECTIONS_MESSAGE_PATTERN  "%d saved selections."
#define SAVE_AS_PROMPT              "Save as: "
#define CHANGE_SETTING_PROMPT       "Change setting: "
#define INVALID_ASSIGNMENT_MESSAGE  "Invalid assignment."
#define RANGE_PROMPT                "Lines range: "
#define INVALID_RANGE_MESSAGE       "Invalid range."
#define SEARCH_PATTERN_PROMPT       "Search pattern: "
#define REPLACE_PATTERN_PROMPT      "Replace pattern: "


// COMPILE-TIME PARAMETERS *****************************************************

#define TERMINAL_SUPPORTS_256_COLORS
#define MOUSE_SUPPORT
#define UNDERLINE_CURSOR_LINE
#define HIGHLIGHT_MATCHING_BRACKET
#define VISUAL_COLUMN               80

#define SCROLL_LINE_NUMBER          3
#define SCROLL_OFFSET               0
#define BACKUP_FILE_NAME            "edit_backup_file"
#define MIN_WIDTH                   81
#define RULER_WIDTH                 8
#define DEFAULT_BUF_SIZE            (1 << 7)


// DEFAULT VALUES FOR RUNTIME-MODIFIABLE PARAMETERS ****************************

#define SYNTAX_HIGHLIGHT            1
#define HIGHLIGHT_SELECTIONS        1
#define CASE_SENSITIVE              1
#define FIELD_SEPARATOR             ','
#define TAB_WIDTH                   4


// COLORS **********************************************************************

// 8-colors terminal
#ifndef TERMINAL_SUPPORTS_256_COLORS
#define OUTPUT_MODE                 TB_OUTPUT_NORMAL
#define COLOR_DEFAULT               TB_DEFAULT
#define COLOR_RULER                 COLOR_DEFAULT
#define COLOR_DIALOG                COLOR_DEFAULT
#define COLOR_KEYWORD               TB_BLUE
#define COLOR_FLOW_CONTROL          TB_GREEN
#define COLOR_BUILT_IN              TB_RED
#define COLOR_NUMBER                TB_YELLOW
#define COLOR_STRING                TB_MAGENTA
#define COLOR_COMMENT               TB_CYAN
#define COLOR_BG_DEFAULT            TB_DEFAULT
#define COLOR_BG_SELECTIONS         0
#define COLOR_BG_MATCHING           0
#define COLOR_BG_COLUMN             0
#endif

// 256-colors terminal
// TODO: refer to https://jacquin.xyz/colors
// 99    Violet
// 32    Bleu
// 172   Orange
// 213   Rose
#ifdef TERMINAL_SUPPORTS_256_COLORS
#define OUTPUT_MODE                 TB_OUTPUT_256
#define COLOR_DEFAULT               TB_DEFAULT
#define COLOR_RULER                 COLOR_DEFAULT
#define COLOR_DIALOG                COLOR_DEFAULT
#define COLOR_KEYWORD               99
#define COLOR_FLOW_CONTROL          32
#define COLOR_BUILT_IN              32
#define COLOR_NUMBER                99
#define COLOR_STRING                99
#define COLOR_COMMENT               172
#define COLOR_BG_DEFAULT            TB_DEFAULT
#define COLOR_BG_SELECTIONS         234
#define COLOR_BG_MATCHING           45
#define COLOR_BG_COLUMN             233
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
#define KB_ACT_REPLACE              'r'
#define KB_ACT_LOWERCASE            'u'
#define KB_ACT_UPPERCASE            'U'


// LANGUAGES SUPPORT ***********************************************************

// please read syntax_highlighting.md to understand languages support

struct rule {
    char mark[5];
    int start_of_line;      // wether a rule recquires the start of the line
    int color_mark;
    int color_end_of_line;
};

struct lang {               // used for syntax highlighting and autocommenting
    // pointers to string containing space-separated, space-ended list of words
    char **names;           // extensions recognised with this language
    char **keywords;
    char **flow_control;
    char **built_ins;
    char **comment;         // commenting syntax (one element, space ended)
    // pointer to array of struct rule
    // must be ended with non-significant rule with empty ("") mark field
    struct rule (*rules)[];
};

// Markdown
char *md_names = "md ";
char *md_keywords = "";
char *md_flow_control = "";
char *md_built_ins = "";
char *md_comment = "";
struct rule md_rules[] = {
    {"###",  1,  COLOR_KEYWORD,      COLOR_KEYWORD},
    {"##",   1,  COLOR_KEYWORD,      COLOR_KEYWORD},
    {"#",    1,  COLOR_KEYWORD,      COLOR_KEYWORD},
    {"---",  1,  COLOR_KEYWORD,      COLOR_KEYWORD},
    {">",    1,  COLOR_COMMENT,      COLOR_DEFAULT},
    {"    ", 1,  COLOR_DEFAULT,      COLOR_DEFAULT},
    {"*",    0,  COLOR_FLOW_CONTROL, COLOR_DEFAULT},
    {"-",    0,  COLOR_FLOW_CONTROL, COLOR_DEFAULT},
    {"",     0,  0,                  0},
};

// C
char *c_names = "c h ";
char *c_keywords = "\
    int long short char void \
    signed unsigned float double \
    typedef struct union enum \
    static register auto volatile extern const \
    FILE DIR NULL \
    int8_t int16_t int32_t int64_t \
    uint8_t uint16_t uint32_t uint64_t \
    define include ";
char *c_flow_control = "\
    while for do if else switch case default \
    goto break return continue ";
char *c_built_ins = "\
    sizeof malloc strcmp strcpy "; // TODO: cf stdlib.h, string.h...
char *c_comment = "// ";
struct rule c_rules[] = {
    {"#",   1,  COLOR_KEYWORD,      COLOR_KEYWORD},
    {"",    0,  0,                  0},
};

// Python
char *py_names = "py ";
char *py_keywords = "\
    False None True and in is not or \
    as assert del global ";
char *py_flow_control = "\
	while for if else elif try except finally with \
    break continue pass return yield lambda \
    class def import from raise async await ";
char *py_built_ins = "\
    abs all any ascii bin bool breakpoint bytearray \
    bytes callable chr classmethod compile complex \
    delattr dict dir divmod enumerate eval exec \
    filter float format frozenset getattr globals \
    hasattr hash help hex id input int isinstance \
    issubclass iter len list locals map max \
    memoryview min next object oct open ord pow \
    print property range repr reversed round set \
    setattr slice sorted staticmethod str sum super \
    tuple type vars zip __import__ ";
char *py_comment = "# ";
struct rule py_rules[] = {
    {"",    0,  0,                  0},
};

// Languages
struct lang languages[] = {
    {&md_names, &md_keywords, &md_flow_control, &md_built_ins, &md_comment, &md_rules},
    {&c_names, &c_keywords, &c_flow_control, &c_built_ins, &c_comment, &c_rules},
    {&py_names, &py_keywords, &py_flow_control, &py_built_ins, &py_comment, &py_rules},
};
