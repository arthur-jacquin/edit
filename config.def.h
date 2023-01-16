// PARAMETERS ******************************************************************

#define MOUSE_SUPPORT               1
#define BACKUP_FILE_NAME            "edit_backup_file"
#define MIN_WIDTH                   81
#define RULER_WIDTH                 8
#define SCROLL_LINE_NUMBER          3
#define DEFAULT_BUF_SIZE            (1 << 7)


// DEFAULT VALUES FOR RUNTIME-MODIFIABLE PARAMETERS ****************************

#define AUTOINDENT                  1
#define SYNTAX_HIGHLIGHT            1
#define HIGHLIGHT_SELECTIONS        1
#define CASE_SENSITIVE              1
#define REPLACE_TABS                1
#define FIELD_SEPARATOR             ','
#define TAB_WIDTH                   4


// COLORS **********************************************************************

/*#define OUTPUT_MODE                 TB_OUTPUT_NORMAL

#define COLOR_DEFAULT_FG            TB_DEFAULT
#define COLOR_RULER                 COLOR_DEFAULT_FG
#define COLOR_DIALOG                COLOR_DEFAULT_FG
#define COLOR_KEYWORD               TB_BLUE
#define COLOR_FLOW_CONTROL          TB_GREEN
#define COLOR_BUILT_IN              TB_RED
#define COLOR_NUMBER                TB_YELLOW
#define COLOR_STRING                TB_MAGENTA
#define COLOR_COMMENT               TB_CYAN

#define COLOR_DEFAULT_BG            TB_DEFAULT
#define COLOR_SELECTIONS_BG         0
#define COLOR_MATCHING_BG           0*/

// TODO: get https://www.calmar.ws/vim/256-xterm-24bit-rgb-color-chart.html
// 99    Violet
// 32    Bleu
// 172   Orange
// 213   Rose

#define OUTPUT_MODE                 TB_OUTPUT_256

#define COLOR_DEFAULT_FG            TB_DEFAULT
#define COLOR_RULER                 COLOR_DEFAULT_FG
#define COLOR_DIALOG                COLOR_DEFAULT_FG
#define COLOR_KEYWORD               99
#define COLOR_FLOW_CONTROL          32
#define COLOR_BUILT_IN              32
#define COLOR_NUMBER                99
#define COLOR_STRING                99
#define COLOR_COMMENT               172

#define COLOR_DEFAULT_BG            TB_DEFAULT
#define COLOR_SELECTIONS_BG         234
#define COLOR_MATCHING_BG           TB_DEFAULT


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

// movements
#define KB_MOVE_MATCHING            'm'
#define KB_MOVE_START_LINE          '0'
#define KB_MOVE_NON_BLANCK          '^'
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
#define KB_ACT_REPLACE              'R'
#define KB_ACT_LOWERCASE            'u'
#define KB_ACT_UPPERCASE            'U'


// LANGUAGES SUPPORT ***********************************************************

#define START_ANY                   0
#define START_FIRST_OF_LINE         1
#define START_FIRST_NON_BLANCK      2

struct rule {
    char *mark;
    int start_at;
    int to_the_end_of_line;
    int color;
};

struct lang {
    char **names;
    char **keywords;
    char **flow_control;
    char **built_ins;
    char **comment;
    struct rule *rules;
};

// C
char *c_names = "c h ";
char *c_keywords = "volatile auto const static extern \
                    enum struct union typedef register \
                    void NULL char int float \
                    short long double signed unsigned \
                    define include ";
char *c_flow_control = "return if else for while do break \
                        continue switch case default goto ";
char *c_built_ins = "sizeof malloc strcmp strcpy ";
char *c_comment = "// ";

// Languages
struct lang languages[] = {
    {&c_names, &c_keywords, &c_flow_control, &c_built_ins, &c_comment, NULL},
};


// ERROR CODES *****************************************************************

// TODO
#define ERR_TERM_NOT_BIG_ENOUGH     1
#define ERR_BAD_ARGUMENTS           2
#define ERR_MALLOC                  3
#define ERR_TOO_LONG_LINE           4
#define ERR_INVALID_LINE_VALUE      5


// MISC ************************************************************************

#define BRACKETS                    "()[]{}"    // TODO
