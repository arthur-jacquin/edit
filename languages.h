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

// useful ressources:
// https://github.com/ilai-deutel/kibi/tree/master/syntax.d
// https://github.com/zyedidia/micro/tree/master/runtime/syntax
// https://github.com/vim/vim/tree/master/runtime/syntax

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

#ifdef CSS
char *css_names = "css ";
char *css_kw = ""
    "accent-color align-content align-items align-self all animation "
    "animation-delay animation-direction animation-duration "
    "animation-fill-mode animation-iteration-count animation-name "
    "animation-play-state animation-timing-function aspect-ratio "
    "backdrop-filter backface-visibility background background-attachment "
    "background-blend-mode background-clip background-color background-image "
    "background-origin background-position background-position-x "
    "background-position-y background-repeat background-size block-size border "
    "border-block border-block-color border-block-end-color "
    "border-block-end-style border-block-end-width border-block-start-color "
    "border-block-start-style border-block-start-width border-block-style "
    "border-block-width border-bottom border-bottom-color "
    "border-bottom-left-radius border-bottom-right-radius border-bottom-style "
    "border-bottom-width border-collapse border-color border-image "
    "border-image-outset border-image-repeat border-image-slice "
    "border-image-source border-image-width border-inline border-inline-color "
    "border-inline-end-color border-inline-end-style border-inline-end-width "
    "border-inline-start-color border-inline-start-style "
    "border-inline-start-width border-inline-style border-inline-width "
    "border-left border-left-color border-left-style border-left-width "
    "border-radius border-right border-right-color border-right-style "
    "border-right-width border-spacing border-style border-top "
    "border-top-color border-top-left-radius border-top-right-radius "
    "border-top-style border-top-width border-width bottom "
    "box-decoration-break box-reflect box-shadow box-sizing break-after "
    "break-before break-inside caption-side caret-color charset clear clip "
    "color column-count column-fill column-gap column-rule column-rule-color "
    "column-rule-style column-rule-width column-span column-width columns "
    "content counter-increment counter-reset cursor direction display "
    "empty-cells filter flex flex-basis flex-direction flex-flow flex-grow "
    "flex-shrink flex-wrap float font font-face font-family "
    "font-feature-settings font-feature-values font-kerning "
    "font-language-override font-size font-size-adjust font-stretch font-style "
    "font-synthesis font-variant font-variant-alternates font-variant-caps "
    "font-variant-east-asian font-variant-ligatures font-variant-numeric "
    "font-variant-position font-weight gap grid grid-area grid-auto-columns "
    "grid-auto-flow grid-auto-rows grid-column grid-column-end grid-column-gap "
    "grid-column-start grid-gap grid-row grid-row-end grid-row-gap "
    "grid-row-start grid-template grid-template-areas grid-template-columns "
    "grid-template-rows hanging-punctuation height hyphens image-rendering "
    "import inline-size inset inset-block inset-block-end inset-block-start "
    "inset-inline inset-inline-end inset-inline-start isolation "
    "justify-content justify-items justify-self keyframes left letter-spacing "
    "line-break line-height list-style list-style-image list-style-position "
    "list-style-type margin margin-block margin-block-end margin-block-start "
    "margin-bottom margin-inline margin-inline-end margin-inline-start "
    "margin-left margin-right margin-top mask mask-clip mask-composite "
    "mask-image mask-mode mask-origin mask-position mask-repeat mask-size "
    "mask-type max-height max-width media max-block-size max-inline-size "
    "min-block-size min-inline-size min-height min-width mix-blend-mode "
    "object-fit object-position offset offset-anchor offset-distance "
    "offset-path offset-rotate opacity order orphans outline outline-color "
    "outline-offset outline-style outline-width overflow overflow-anchor "
    "overflow-wrap overflow-x overflow-y overscroll-behavior "
    "overscroll-behavior-block overscroll-behavior-inline "
    "overscroll-behavior-x overscroll-behavior-y padding padding-block "
    "padding-block-end padding-block-start padding-bottom padding-inline "
    "padding-inline-end padding-inline-start padding-left padding-right "
    "padding-top page-break-after page-break-before page-break-inside "
    "paint-order perspective perspective-origin place-content place-items "
    "place-self pointer-events position quotes resize right rotate row-gap "
    "scale scroll-behavior scroll-margin scroll-margin-block "
    "scroll-margin-block-end scroll-margin-block-start scroll-margin-bottom "
    "scroll-margin-inline scroll-margin-inline-end scroll-margin-inline-start "
    "scroll-margin-left scroll-margin-right scroll-margin-top scroll-padding "
    "scroll-padding-block scroll-padding-block-end scroll-padding-block-start "
    "scroll-padding-bottom scroll-padding-inline scroll-padding-inline-end "
    "scroll-padding-inline-start scroll-padding-left scroll-padding-right "
    "scroll-padding-top scroll-snap-align scroll-snap-stop scroll-snap-type "
    "tab-size table-layout text-align text-align-last text-combine-upright "
    "text-decoration text-decoration-color text-decoration-line "
    "text-decoration-style text-decoration-thickness text-emphasis text-indent "
    "text-justify text-orientation text-overflow text-shadow text-transform "
    "text-underline-position top transform transform-origin transform-style "
    "transition transition-delay transition-duration transition-property "
    "transition-timing-function translate unicode-bidi user-select "
    "vertical-align visibility white-space widows width word-break "
    "word-spacing word-wrap writing-mode z-index ";
char *css_fc = "";
char *css_bi = "";
char *css_cm = "/* ";
struct rule css_rules[] = EMPTY_RULES;
#endif // CSS

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

#ifdef HTML
char *html_names = "html ";
char *html_kw = ""
    "DOCTYPE a abbr address area article aside audio b base bdi bdo blockquote "
    "body br button canvas caption cite code col colgroup data datalist dd del "
    "details dfn dialog div dl dt em embed fieldset figcaption figure footer "
    "form h1 h2 h3 h4 h5 h6 head header hr html i iframe img input ins kbd "
    "label legend li link main map mark meta meter nav noscript object ol "
    "optgroup option output p param picture pre progress q rp rt ruby s samp "
    "script section select small source span strong style sub summary sup svg "
    "table tbody td template textarea tfoot th thead time title tr track u ul "
    "var video wbr ";
char *html_fc = "";
char *html_bi = "";
char *html_cm = "<!-- ";
struct rule html_rules[] = EMPTY_RULES;
#endif // HTML

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

#ifdef RUBY
char *rb_names = "rb ";
char *rb_kw = ""
    "alias and BEGIN class def defined END ensure false in module next nul not "
    "or rescue self super true undef "
char *rb_fc = ""
    "begin break case do else elsif end for if redo retry return then unless "
    "until when while yield ";
char *rb_bi = "";
char *rb_cm = "// ";
struct rule rb_rules[] = EMPTY_RULES;
#endif // RUBY

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

#ifdef SWIFT
// https://github.com/zyedidia/micro/blob/master/runtime/syntax/swift.yaml
// https://github.com/vim/vim/blob/master/runtime/syntax/swift.vim
char *swift_names = "swift ";
// char *swift_kw = ""
// true false nil
// char *swift_fc = ""
// char *swift_bi = ""
char *swift_cm = "// ";
// struct rule swift_rules[] = {
    // {"@",   1,  COLOR_KEYWORD,      COLOR_KEYWORD},
    // {"",    0,  0,                  0},
// };
#endif // SWIFT

#ifdef TEX
char *tex_names = "tex bib cls ";
char *tex_kw = ""
    "documentclass documentstyle usepackage input include includegraphics "
    "includelist author title ";
char *tex_fc = ""
    "begin end document part chapter section subsection subsubsection "
    "paragraph subparagraph abstract bibliography label pageref eqref ref def ";
char *tex_bi = ""
    "rm em bf it sl sf sc tt textbf textit emph textmd textrm mathbf mathcal "
    "mathit mathnormal mathrm mathsf mathtt rmfamily sffamily ttfamily itshape "
    "scshape slshape upshape bfseries mdseries tiny scriptsize footnotesize "
    "small normalsize large Large LARGE huge Huge ";
char *tex_cm = "% ";
struct rule tex_rules[] = EMPTY_RULES;
#endif // TEX

#ifdef YAML
char *yaml_names = "yaml yml ";
char *yaml_kw = "true false null yes no on off ";
char *yaml_fc = "";
char *yaml_bi = "str seq map int float bool timestanp ";
char *yaml_cm = "# ";
struct rule yaml_rules[] = EMPTY_RULES;
#endif // YAML


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
    LANG_0(diff),
#endif
#ifdef GEMTEXT
    LANG_0(gemtext),
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
#ifdef LUA
    LANG(lua),
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
