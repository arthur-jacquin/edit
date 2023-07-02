// see LICENSE file for copyright and license details

// MACROS AND STRUCTS **********************************************************

#define LANG(L)     {&L##_rules,1,&L##_names,&L##_kw,&L##_fc,&L##_bi,&L##_cm}
#define LANG_0(L)   {&L##_rules,0,&L##_names,NULL,NULL,NULL,NULL}
#define EMPTY_RULES {{"", 0, 0, 0}}

struct rule {
    char mark[5];
    int start_of_line;      // whether a rule requires the start of the line
    int color_mark;
    int color_end_of_line;
};

struct lang {               // used for syntax highlighting and autocommenting
    // pointer to array of struct rule
    // must be ended with non-significant rule with empty ("") mark field
    struct rule (*rules)[];
    int highlight_elements; // whether non-rules elements should be highlighted
    // pointers to string containing space-separated, space-ended list of ASCII
    // words
    char **names;           // extensions recognised with this language
    char **keywords;
    char **flow_control;
    char **built_ins;
    char **comment;         // commenting syntax (one element, space ended)
};


// LANGUAGES PACKS *************************************************************

// define these languages packs in config.h to get many languages in one line

#ifdef ALL_LANGUAGES
#define C
#define DIFF
#define GEMTEXT
#define MARKDOWN
#define OCAML
#define PYTHON
#define SH
#define SQL
#endif // ALL_LANGUAGES


// LANGUAGES DEFINITION ********************************************************

// useful ressources:
// https://github.com/ilai-deutel/kibi/tree/master/syntax.d
// https://github.com/zyedidia/micro/tree/master/runtime/syntax
// https://github.com/vim/vim/tree/master/runtime/syntax

#ifdef C
char *c_names = "c h ";
char *c_kw = ""
    "int long short char void signed unsigned float double typedef struct "
    "union enum static register auto volatile extern const FILE DIR NULL "
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
struct rule c_rules[] = {
    {"#",   0,  COLOR_KEYWORD,      COLOR_KEYWORD},
    {"",    0,  0,                  0},
};
#endif // C

#ifdef DIFF
char *diff_names = "diff patch ";
struct rule diff_rules[] = {
    {"@@",  1,  COLOR_FLOW_CONTROL, COLOR_FLOW_CONTROL},
    {"+",   1,  34,                 34},
    {"-",   1,  196,                196},
    {"",    0,  0,                  0},
};
#endif // DIFF

#ifdef GEMTEXT
char *gemtext_names = "gmi gemini ";
struct rule gemtext_rules[] = {
    {"=>",  1,  COLOR_FLOW_CONTROL, COLOR_FLOW_CONTROL},
    {"# ",  1,  COLOR_KEYWORD,      COLOR_KEYWORD},
    {"## ", 1,  COLOR_KEYWORD,      COLOR_KEYWORD},
    {"### ",1,  COLOR_KEYWORD,      COLOR_KEYWORD},
    {"* ",  1,  COLOR_FLOW_CONTROL, COLOR_DEFAULT},
    {">",   1,  COLOR_COMMENT,      COLOR_DEFAULT},
    {"",    0,  0,                  0},
};
#endif // GEMTEXT

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

#ifdef OCAML
char *ml_names = "ml mli mll mly ";
char *ml_kw = ""
    "and as assert asr class constraint downto external false fun function "
    "functor in inherit include inherit initializer land lazy lor lsl lsr lxor "
    "method mod module mutable new nonrec object of open or private rec sig "
    "struct to true type val virtual with "
    "array bool char exn float format format4 int int32 int64 lazy_t list "
    "nativeint option bytes string unit ";
char *ml_fc = ""
    "begin end do done for if then else exception let match try when while ";
char *ml_bi = "";
char *ml_cm = "(* ";
struct rule ml_rules[] = {
    {"#",   1,  COLOR_KEYWORD,      COLOR_KEYWORD},
    {"",    0,  0,                  0},
};
#endif // OCAML

#ifdef PYTHON
char *py_names = "py ";
char *py_kw = ""
    "False None True and in is not or as assert del global ";
char *py_fc = ""
    "while for if else elif try except finally with break continue pass return "
    "yield lambda class def import from raise async await ";
char *py_bi = ""
    "abs aiter all anext any ascii bin bool breakpoint bytearray bytes "
    "callable chr classmethod compile complex delattr dict dir divmod "
    "enumerate eval exec filter float format frozenset getattr globals hasattr "
    "hash help hex id input int isinstance issubclass iter len list locals map "
    "max memoryview min next object oct open ord pow print property range repr "
    "reversed round set setattr slice sorted staticmethod str sum super tuple "
    "type vars zip __import__ ";
char *py_cm = "# ";
struct rule py_rules[] = EMPTY_RULES;
#endif // PYTHON

#ifdef SH
char *sh_names = "sh ";
char *sh_kw = "";
char *sh_fc = ""
    "case do done elif else esac fi for if in then until while "
    "break continue exit return ";
char *sh_bi = ""
    "eval exec readonly set shift times trap unset ";
char *sh_cm = "# ";
struct rule sh_rules[] = EMPTY_RULES;
#endif // SH

#ifdef SQL
char *sql_names = "sql sqlite ";
char *sql_kw = ""
    "ABORT ACTION ADD AFTER ALL ALTER ALWAYS ANALYZE AND AS ASC ATTACH "
    "AUTOINCREMENT BEFORE BEGIN BETWEEN BY CASCADE CASE CAST CHECK COLLATE "
    "COLUMN COMMIT CONFLICT CONSTRAINT CREATE CROSS CURRENT CURRENT_DATE "
    "CURRENT_TIME CURRENT_TIMESTAMP DATABASE DEFAULT DEFERRABLE DEFERRED "
    "DELETE DESC DETACH DISTINCT DO DROP EACH ELSE END ESCAPE EXCEPT EXCLUDE "
    "EXCLUSIVE EXISTS EXPLAIN FAIL FILTER FIRST FOLLOWING FOR FOREIGN FROM "
    "FULL GENERATED GLOB GROUP GROUPS HAVING IF IGNORE IMMEDIATE IN INDEX "
    "INDEXED INITIALLY INNER INSERT INSTEAD INTERSECT INTO IS ISNULL JOIN KEY "
    "LAST LEFT LIKE LIMIT MATCH MATERIALIZED NATURAL NO NOT NOTHING NOTNULL "
    "NULL NULLS OF OFFSET ON OR ORDER OTHERS OUTER OVER PARTITION PLAN PRAGMA "
    "PRECEDING PRIMARY QUERY RAISE RANGE RECURSIVE REFERENCES REGEXP REINDEX "
    "RELEASE RENAME REPLACE RESTRICT RETURNING RIGHT ROLLBACK ROW ROWS "
    "SAVEPOINT SELECT SET TABLE TEMP TEMPORARY THEN TIES TO TRANSACTION "
    "TRIGGER UNBOUNDED UNION UNIQUE UPDATE USING VACUUM VALUES VIEW VIRTUAL "
    "WHEN WHERE WINDOW WITH WITHOUT ";
char *sql_fc = "";
char *sql_bi = ""
    "avg count count group_concat group_concat max min sum total "
    "abs changes char coalesce format glob hex ifnull iif instr "
    "last_insert_rowid length like like likelihood likely load_extension "
    "load_extension lower ltrim ltrim max min nullif printf quote random "
    "randomblob replace round round rtrim rtrim sign soundex "
    "sqlite_compileoption_get sqlite_compileoption_used sqlite_offset "
    "sqlite_source_id sqlite_version substr substr substring substring "
    "total_changes trim trim typeof unhex unhex unicode unlikely upper "
    "zeroblob "
    "acos acosh asin asinh atan atan2 atanh ceil ceiling cos cosh degrees exp "
    "floor ln log log log10 log2 mod pi pow power radians sin sinh sqrt tan "
    "tanh trunc "
    "date time datetime julianday unixepoch strftime ";
char *sql_cm = "-- ";
struct rule sql_rules[] = EMPTY_RULES;
#endif // SQL


// LANGUAGES ARRAY *************************************************************

struct lang languages[] = {
#ifdef C
    LANG(c),
#endif
#ifdef DIFF
    LANG_0(diff),
#endif
#ifdef GEMTEXT
    LANG_0(gemtext),
#endif
#ifdef MARKDOWN
    LANG_0(md),
#endif
#ifdef OCAML
    LANG(ml),
#endif
#ifdef PYTHON
    LANG(py),
#endif
#ifdef SH
    LANG(sh),
#endif
#ifdef SQL
    LANG(sql),
#endif
};
