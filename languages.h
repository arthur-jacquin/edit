// STRUCTS *********************************************************************

// please read syntax_highlighting.md to understand languages support

struct rule {
    char mark[5];
    int start_of_line;      // wether a rule requires the start of the line
    int color_mark;
    int color_end_of_line;
};

struct lang {               // used for syntax highlighting and autocommenting
    // pointer to array of struct rule
    // must be ended with non-significant rule with empty ("") mark field
    struct rule (*rules)[];
    int highlight_elements; // wether non-rules elements should be highlighted
    // pointers to string containing space-separated, space-ended list of words
    char **names;           // extensions recognised with this language
    char **keywords;
    char **flow_control;
    char **built_ins;
    char **comment;         // commenting syntax (one element, space ended)
};


// LANGUAGES DEFINITION ********************************************************

#ifdef C
char *c_names = "c h ";
char *c_kw = ""
    "int long short char void signed unsigned float double typedef struct "
    "union enum static register auto volatile extern const FILE DIR NULL "
    "int8_t int16_t int32_t int64_t uint8_t uint16_t uint32_t uint64_t "
    "define include ";
char *c_fc = ""
    "while for do if else switch case default goto break return continue ";
char *c_bi = ""
    "sizeof malloc strcmp strcpy "; // TODO: cf stdlib.h, string.h...
char *c_comment = "// ";
struct rule c_rules[] = {
    {"#",   1,  COLOR_KEYWORD,      COLOR_KEYWORD},
    {"",    0,  0,                  0},
};
#endif // C

#ifdef MARKDOWN
char *md_names = "md ";
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
#endif // MARKDOWN

#ifdef PYTHON
char *py_names = "py ";
char *py_kw = ""
    "False None True and in is not or as assert del global ";
char *py_fc = ""
	"while for if else elif try except finally with break continue pass return "
    "yield lambda class def import from raise async await ";
char *py_bi = ""
    "abs all any ascii bin bool breakpoint bytearray bytes callable chr "
    "classmethod compile complex delattr dict dir divmod enumerate eval exec "
    "filter float format frozenset getattr globals hasattr hash help hex id "
    "input int isinstance issubclass iter len list locals map max memoryview "
    "min next object oct open ord pow print property range repr reversed round "
    "set setattr slice sorted staticmethod str sum super tuple type vars zip "
    "__import__ ";
char *py_comment = "# ";
struct rule py_rules[] = {
    {"",    0,  0,                  0},
};
#endif // PYTHON


// LANGUAGES ARRAY *************************************************************

struct lang languages[] = {
#ifdef C
    {&c_rules,  1,  &c_names,   &c_kw,  &c_fc,  &c_bi,  &c_comment},
#endif // C
#ifdef MARKDOWN
    {&md_rules, 0,  &md_names,  NULL,   NULL,   NULL,   NULL},
#endif // MARKDOWN
#ifdef PYTHON
    {&py_rules, 1,  &py_names,  &py_kw, &py_fc, &py_bi, &py_comment},
#endif // PYTHON
};
