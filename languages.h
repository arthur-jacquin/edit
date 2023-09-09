// See LICENSE file for copyright and license details.

// macros
#define NULL_RULE                   {""}
#define CONVERT_LEADING_SPACES      (1 << 0)
#define DEFINED                     (1 << 1)
#define ONLY_RULES                  (1 << 2)
#define LANG(L, F) \
    {F | DEFINED, &L##_names, &L##_rules, &L##_kw, &L##_fc, &L##_bi, &L##_cm}
#define ONLY_RULES_LANG(L, F) \
    {F | DEFINED | ONLY_RULES, &L##_names, &L##_rules}

// types
struct rule {
    char mark[5];
    int start_of_line;
    int color_mark, color_end_of_line;
};

struct lang {
    // char ** members: pointer to space-separated, space-ended ASCII words list
    unsigned int flags;
    char **names;                   // file extensions/names
    struct rule (*rules)[];         // must be ended with NULL_RULE
    char **keywords, **flow_control, **built_ins;
    char **comment;                 // commenting syntax (one element)
};

// language definitions
#ifdef C
char *c_names = "c h ";
char *c_kw = ""
    "int long short char void signed unsigned float double typedef struct "
    "union enum static register auto volatile extern const FILE DIR NULL "
    "size_t "
    "int8_t int16_t int32_t int64_t uint8_t uint16_t uint32_t uint64_t sizeof ";
char *c_fc = ""
    "while for do if else switch case default goto break return continue ";
char *c_bi = ""
    // stdio.h
    "stdin stdout stderr fopen freopen fflush fclose remove rename tmpfile "
    "tmpnam setvbuf setbuf fprintf printf sprintf vprintf vfprintf vsprintf "
    "fscanf scanf sscanf fgetc fgets fputc fputs getc getchar gets putc "
    "putchar puts ungetc fread fwrite fseek ftell rewind fgetpos fsetpos "
    "clearerr feof ferror perror "
    // ctype.h
    "isalnum isalpha iscntrl isdigit isgraph islower isprint ispunct isspace "
    "isupper isxdigit tolower toupper "
    // string.h
    "strcpy strncpy strcat strncat strcmp strncmp strchr strrchr strspn "
    "strcspn strpbrk strstr strlen strerror strtok memcpy memmove memcmp "
    "memchr memset "
    // math.h
    "sin cos tan asin acos atan atan2 sinh cosh tanh exp log log10 pow sqrt "
    "ceil floor fabs ldexp frexp modf fmod "
    // stdlib.h
    "atoi atol strtod strtol strtoul rand srand calloc malloc realloc free "
    "abort exit atexit system getenv bsearch qsort abs labs div ldiv "
    // assert.h
    "assert "
    // stdarg.h
    "va_start va_end "
    // setjmp.h
    "setjmp longjmp "
    // signal.h
    "signal raise "
    // time.h
    "clock time difftime mktime asctime ctime gmtime localtime strftime ";
char *c_cm = "// ";
static struct rule c_rules[] = {
    {"#",   0,  COLOR_KEYWORD,      COLOR_KEYWORD},
    NULL_RULE
};
#endif // C

#ifdef DIFF
char *diff_names = "diff patch ";
static struct rule diff_rules[] = {
    {"@@",  1,  COLOR_FLOW_CONTROL, COLOR_FLOW_CONTROL},
    {"+",   1,  34,                 34},
    {"-",   1,  196,                196},
    NULL_RULE
};
#endif // DIFF

#ifdef GEMTEXT
char *gemtext_names = "gmi gemini ";
static struct rule gemtext_rules[] = {
    {"=>",  1,  COLOR_FLOW_CONTROL, COLOR_FLOW_CONTROL},
    {"# ",  1,  COLOR_KEYWORD,      COLOR_KEYWORD},
    {"## ", 1,  COLOR_KEYWORD,      COLOR_KEYWORD},
    {"### ",1,  COLOR_KEYWORD,      COLOR_KEYWORD},
    {"* ",  1,  COLOR_FLOW_CONTROL, COLOR_DEFAULT},
    {">",   1,  COLOR_COMMENT,      COLOR_DEFAULT},
    NULL_RULE
};
#endif // GEMTEXT

#ifdef MAKEFILE
char *mk_names = "Makefile makefile mk ";
char *mk_kw = "all clean dist install uninstall ";
char *mk_fc = "PHONY ";
char *mk_bi = "";
char *mk_cm = "# ";
static struct rule mk_rules[] = {NULL_RULE};
#endif // MAKEFILE

// TODO: MANPAGE

#ifdef MARKDOWN
char *md_names = "md README ";
static struct rule md_rules[] = {
    {"###",  1,  COLOR_KEYWORD,      COLOR_KEYWORD},
    {"##",   1,  COLOR_KEYWORD,      COLOR_KEYWORD},
    {"#",    1,  COLOR_KEYWORD,      COLOR_KEYWORD},
    {"---",  1,  COLOR_KEYWORD,      COLOR_KEYWORD},
    {">",    1,  COLOR_COMMENT,      COLOR_DEFAULT},
    {"    ", 1,  COLOR_DEFAULT,      COLOR_DEFAULT},
    {"*",    0,  COLOR_FLOW_CONTROL, COLOR_DEFAULT},
    {"-",    0,  COLOR_FLOW_CONTROL, COLOR_DEFAULT},
    NULL_RULE
};
#endif // MARKDOWN

static const struct lang languages[] = {
#ifdef C
    LANG(c, 0),
#endif
#ifdef DIFF
    ONLY_RULES_LANG(diff, 0),
#endif
#ifdef GEMTEXT
    ONLY_RULES_LANG(gemtext, 0),
#endif
#ifdef MAKEFILE
    LANG(mk, CONVERT_LEADING_SPACES),
#endif
#ifdef MARKDOWN
    ONLY_RULES_LANG(md, 0),
#endif
    {0}
};
