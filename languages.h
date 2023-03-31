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


// LANGUAGES DEFINITION ********************************************************

// TODO add languages
// https://github.com/ilai-deutel/kibi/tree/master/syntax.d
// https://github.com/vim/vim/tree/master/runtime/syntax
// https://github.com/zyedidia/micro/tree/master/runtime/syntax

#ifdef AWK
char *awk_names = "awk nawk gawk mawk ";
char *awk_kw = ""
    "function func BEGIN END BEGINFILE ENDFILE "
    "BINMODE CONVFMT FIELDWIDTHS FPAT FS IGNORECASE LINT OFMT OFS ORS PREC "
    "ROUNDMODE RS SUBSEP TEXTDOMAIN ARGC ARGV ARGIND ENVIRON ERRNO FILENAME "
    "FNR NF FUNCTAB NR PROCINFO RLENGTH RSTART RT SYMTAB ";
char *awk_fc = ""
    "if else while for do break continue exit return case default switch ";
char *awk_bi = ""
    "atan2 cos exp int log rand sin sqrt srand asort asorti gensub gsub index "
    "length match patsplit split sprintf strtonum sub substr tolower toupper "
    "close fflush system mktime strftime systime and compl lshift or rshift "
    "xor isarray typeof bindtextdomain dcgettext dcngetext delete getline next "
    "in print printf nextfile ";
char *awk_cm = "# ";
struct rule awk_rules[] = {
    {"@",   0,  COLOR_KEYWORD,      COLOR_KEYWORD},
    {"",    0,  0,                  0},
};
#endif // AWK

#ifdef C
char *c_names = "c h ";
char *c_kw = ""
    "int long short char void signed unsigned float double typedef struct "
    "union enum static register auto volatile extern const FILE DIR NULL "
    "int8_t int16_t int32_t int64_t uint8_t uint16_t uint32_t uint64_t ";
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


#ifdef CPP
char *cpp_names = "cpp hpp cc cxx hxx ";
char *cpp_kw = "";
char *cpp_fc = "";
char *cpp_bi = "";
char *cpp_cm = "// ";
// alignas alignof and and_eq asm atomic_cancel
// atomic_commit atomic_noexcept bitand bitor bool break case
// catch char char8_t char16_t char32_t class compl concept
// const consteval constexpr constinit const_cast continue co_await
// co_return co_yield decltype default delete do double dynamic_cast
// else enum explicit export extern false float for friend goto if
// inline int long mutable namespace new noexcept not not_eq nullptr
// operator or or_eq private protected public register reinterpret_cast
// return short signed sizeof static static_assert static_cast struct
// switch template this thread_local throw true try typedef typeid
// typename union unsigned using virtual void volatile wchar_t while
// xor xor_eq
// concept requires typename template class friend inline
// constexpr consteval constinit public protected private export import
// module using static_assert static_cast dynamic_cast reinterpret_cast
// const_cast typeid typename this throw catch try operator new delete
// noexcept alignas alignof and and_eq bitand bitor compl not not_eq or
// or_eq xor xor_eq co_await co_return co_yield
struct rule cpp_rules[] = {
    {"",    0,  0,                  0},
};
#endif // CPP

// #ifdef CSS
// char *css_names = " ";
// char *css_kw = ""
// char *css_fc = ""
// char *css_bi = ""
// char *css_cm = " ";
// struct rule css_rules[] = {
    // {"",    0,  0,                  0},
// };
// #endif // CSS

// #ifdef DIFF
// char *diffgemtext_names = " ";
// char *diffgemtext_kw = ""
// char *diffgemtext_fc = ""
// char *diffgemtext_bi = ""
// char *diffgemtext_cm = " ";
// struct rule diffgemtext_rules[] = {
    // {"",    0,  0,                  0},
// };
// #endif // DIFF

// #ifdef GEMTEXT
// char *gemtext_names = " ";
// char *gemtext_kw = ""
// char *gemtext_fc = ""
// char *gemtext_bi = ""
// char *gemtext_cm = " ";
// struct rule gemtext_rules[] = {
    // {"",    0,  0,                  0},
// };
// #endif // GEMTEXT

// #ifdef GO
// char *go_names = " ";
// char *go_kw = ""
// char *go_fc = ""
// char *go_bi = ""
// char *go_cm = " ";
// struct rule go_rules[] = {
    // {"",    0,  0,                  0},
// };
// #endif // GO

// #ifdef HASKELL
// char *haskell_names = " ";
// char *haskell_kw = ""
// char *haskell_fc = ""
// char *haskell_bi = ""
// char *haskell_cm = " ";
// struct rule haskell_rules[] = {
    // {"",    0,  0,                  0},
// };
// #endif // HASKELL

// #ifdef HTML
// char *html_names = " ";
// char *html_kw = ""
// char *html_fc = ""
// char *html_bi = ""
// char *html_cm = " ";
// struct rule html_rules[] = {
    // {"",    0,  0,                  0},
// };
// #endif // HTML

// #ifdef JAVA
// char *java_names = " ";
// char *java_kw = ""
// char *java_fc = ""
// char *java_bi = ""
// char *java_cm = " ";
// struct rule java_rules[] = {
    // {"",    0,  0,                  0},
// };
// #endif // JAVA

// #ifdef JAVASCRIPT
// char *js_names = " ";
// char *js_kw = ""
// char *js_fc = ""
// char *js_bi = ""
// char *js_cm = " ";
// struct rule js_rules[] = {
    // {"",    0,  0,                  0},
// };
// #endif // JAVASCRIPT

// #ifdef JSON
// char *json_names = " ";
// char *json_kw = ""
// char *json_fc = ""
// char *json_bi = ""
// char *json_cm = " ";
// struct rule json_rules[] = {
    // {"",    0,  0,                  0},
// };
// #endif // JSON

// #ifdef LUA
// char *lua_names = " ";
// char *lua_kw = ""
// char *lua_fc = ""
// char *lua_bi = ""
// char *lua_cm = " ";
// struct rule lua_rules[] = {
    // {"",    0,  0,                  0},
// };
// #endif // LUA

// #ifdef MAKE
// char *make_names = " ";
// char *make_kw = ""
// char *make_fc = ""
// char *make_bi = ""
// char *make_cm = " ";
// struct rule make_rules[] = {
    // {"",    0,  0,                  0},
// };
// #endif // MAKE

// #ifdef MAN
// char *man_names = " ";
// char *man_kw = ""
// char *man_fc = ""
// char *man_bi = ""
// char *man_cm = " ";
// struct rule man_rules[] = {
    // {"",    0,  0,                  0},
// };
// #endif // MAN

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

// #ifdef OCAML
// char *ml_names = " ";
// char *ml_kw = ""
// char *ml_fc = ""
// char *ml_bi = ""
// char *ml_cm = " ";
// struct rule ml_rules[] = {
    // {"",    0,  0,                  0},
// };
// #endif // OCAML

// #ifdef PERL
// char *perl_names = " ";
// char *perl_kw = ""
// char *perl_fc = ""
// char *perl_bi = ""
// char *perl_cm = " ";
// struct rule perl_rules[] = {
    // {"",    0,  0,                  0},
// };
// #endif // PERL

// #ifdef PHP
// char *php_names = " ";
// char *php_kw = ""
// char *php_fc = ""
// char *php_bi = ""
// char *php_cm = " ";
// struct rule php_rules[] = {
    // {"",    0,  0,                  0},
// };
// #endif // PHP

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
char *py_cm = "# ";
struct rule py_rules[] = EMPTY_RULES;
#endif // PYTHON

// #ifdef R
// char *r_names = " ";
// char *r_kw = ""
// char *r_fc = ""
// char *r_bi = ""
// char *r_cm = " ";
// struct rule r_rules[] = {
    // {"",    0,  0,                  0},
// };
// #endif // R

// #ifdef RUBY
// char *rb_names = " ";
// char *rb_kw = ""
// char *rb_fc = ""
// char *rb_bi = ""
// char *rb_cm = " ";
// struct rule rb_rules[] = {
    // {"",    0,  0,                  0},
// };
// #endif // RUBY

// #ifdef RUST
// char *rs_names = " ";
// char *rs_kw = ""
// char *rs_fc = ""
// char *rs_bi = ""
// char *rs_cm = " ";
// struct rule rs_rules[] = {
    // {"",    0,  0,                  0},
// };
// #endif // RUST

// #ifdef SED
// char *sed_names = " ";
// char *sed_kw = ""
// char *sed_fc = ""
// char *sed_bi = ""
// char *sed_cm = " ";
// struct rule sed_rules[] = {
    // {"",    0,  0,                  0},
// };
// #endif // SED

#ifdef SH
char *sh_names = "sh ";
char *sh_kw = "";
char *sh_fc = ""
    "case do done elif else esac fi for if in then until while ";
char *sh_bi = ""
    "break continue eval exec exit export readonly return set shift times trap "
    "unset ";
char *sh_cm = "# ";
struct rule sh_rules[] = EMPTY_RULES;
#endif // SH

// #ifdef SQL
// char *sql_names = " ";
// char *sql_kw = ""
// char *sql_fc = ""
// char *sql_bi = ""
// char *sql_cm = " ";
// struct rule sql_rules[] = {
    // {"",    0,  0,                  0},
// };
// #endif // SQL

// #ifdef SWIFT
// char *swift_names = " ";
// char *swift_kw = ""
// char *swift_fc = ""
// char *swift_bi = ""
// char *swift_cm = " ";
// struct rule swift_rules[] = {
    // {"",    0,  0,                  0},
// };
// #endif // SWIFT

// #ifdef TEX
// char *tex_names = " ";
// char *tex_kw = ""
// char *tex_fc = ""
// char *tex_bi = ""
// char *tex_cm = " ";
// struct rule tex_rules[] = {
    // {"",    0,  0,                  0},
// };
// #endif // TEX

// #ifdef YAML
// char *yaml_names = " ";
// char *yaml_kw = ""
// char *yaml_fc = ""
// char *yaml_bi = ""
// char *yaml_cm = " ";
// struct rule yaml_rules[] = {
    // {"",    0,  0,                  0},
// };
// #endif // YAML


// LANGUAGES ARRAY *************************************************************

struct lang languages[] = {
#ifdef AWK
    LANG(awk),
#endif
#ifdef C
    LANG(c),
#endif
#ifdef CPP
    LANG(cpp),
#endif
#ifdef CSS
    LANG(css),
#endif
#ifdef DIFF
    LANG(diff),
#endif
#ifdef GEMTEXT
    LANG(gemtext),
#endif
#ifdef GO
    LANG(go),
#endif
#ifdef HASKELL
    LANG(haskell),
#endif
#ifdef HTML
    LANG(html),
#endif
#ifdef JAVA
    LANG(java),
#endif
#ifdef JAVASCRIPT
    LANG(js),
#endif
#ifdef JSON
    LANG(json),
#endif
#ifdef LUA
    LANG(lua),
#endif
#ifdef MAKE
    LANG(make),
#endif
#ifdef MAN
    LANG(man),
#endif
#ifdef MARKDOWN
    LANG_0(md),
#endif
#ifdef OCAML
    LANG(ml),
#endif
#ifdef PERL
    LANG(perl),
#endif
#ifdef PHP
    LANG(php),
#endif
#ifdef PYTHON
    LANG(py),
#endif
#ifdef R
    LANG(r),
#endif
#ifdef RUBY
    LANG(rb),
#endif
#ifdef RUST
    LANG(rs),
#endif
#ifdef SED
    LANG(sed),
#endif
#ifdef SH
    LANG(sh),
#endif
#ifdef SQL
    LANG(sql),
#endif
#ifdef SWIFT
    LANG(swift),
#endif
#ifdef TEX
    LANG(tex),
#endif
#ifdef YAML
    LANG(yaml),
#endif
};
