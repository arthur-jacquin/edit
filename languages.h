// See LICENSE file for copyright and license details.

// macros
#define NULL_RULE                   {""}
#define CONVERT_LEADING_SPACES      (1 << 0)
#define DEFINED                     (1 << 1)
#define HIGHLIGHT_CONSTANTS         (1 << 2)
#define HIGHLIGHT_SYNTAX            (1 << 3)
#define SYNTAX_LANG(L, F) \
    {DEFINED | HIGHLIGHT_SYNTAX | F, L##_names, L##_rules, L##_cm, L##_kw, L##_fc, L##_bi}
#define NO_SYNTAX_LANG(L, F) \
    {DEFINED | F, L##_names, L##_rules}

// types
struct rule {
    char mark[5];                   // start of line to match
    int start_of_line;
    int color_mark, color_end_of_line;
};

struct lang {
    // const char * members: space-separated, space-ended ASCII words list
    unsigned int flags;
    const char *names;              // file extensions/names
    const struct rule *rules;       // must be ended with NULL_RULE
    const char *comment;            // commenting syntax (one element)
    const char *keywords, *flow_control, *built_ins;
};

// language definitions
#ifdef ASM
static const char asm_names[] = "asm ";
static const struct rule asm_rules[] = {
    {"%",   0,  COLOR_KEYWORD,      COLOR_KEYWORD},
    NULL_RULE
};
static const char asm_cm[] = "; ";
// https://www.felixcloutier.com/x86
static const char asm_kw[] = ""
    "add and andn bt btc btr bts clac clc cld cli clts clui cmp cmps cpuid dec "
    "div idiv imul in inc ins lahf lar lds lea les lfs lgdt lgs lidt lldt lmsw "
    "lods lss ltr lzcnt mov movs mul neg nop not or out outs pop popa popad "
    "popcnt popf popfd popfq push pusha pushad pushf pushfd pushfq rcl rcr "
    "rdmsr rol ror sal sar sgdt shl shr sidt sldt smsw stac stc std sti stos "
    "str stui sub test wrmsr xlat xor "
    "bits bss data db dw dd dq dt ddq do equ extern global org section segment "
    "text times ";
static const char asm_fc[] = ""
    "hlt call int ret iret iretq iretf uiret "
    "ja jae jb jbe jc je jg jge jl jle jna jnae jnb jnbe jnc jne jng jnge jnl "
    "jnle jno jnp  jns jnz jo jp jpe jpo js jz jcxz jmp loop loope loopne "
    "syscall sysenter sysexit sysret ";
static const char asm_bi[] = ""
    "al ah ax eax rax cl ch cx ecx rcx dl dh dx edx rdx bl bh bx ebx rbx "
    "spl sp esp rsp bpl bp ebp rbp sil si esi rsi dil di edi rdi "
    "r8b r8w r8d r8 r9b r9w r9d r9 r10b r10w r10d r10 r11b r11w r11d r11 "
    "r12b r12w r12d r12 r13b r13w r13d r13 r14b r14w r14d r14 r15b r15w r15d r15 "
    "ss cs ds es fs gs ip eip rip flags eflags rflags gdtr ldtr idtr tr "
    "cr0 cr1 cr2 cr3 cr4 cr5 cr6 cr7 cr8 cr9 cr10 cr11 cr12 cr13 cr14 cr15 ";
#endif // ASM

#ifdef AWK
static const char awk_names[] = "awk nawk gawk mawk ";
static const struct rule awk_rules[] = {
    {"@",   0,  COLOR_KEYWORD,      COLOR_KEYWORD},
    NULL_RULE
};
static const char awk_cm[] = "# ";
static const char awk_kw[] = ""
    "ARGC ARGIND ARGV BEGIN BEGINFILE BINMODE CONVFMT END ENDFILE ENVIRON "
    "ERRNO FIELDWIDTHS FILENAME FNR FPAT FS FUNCTAB IGNORECASE LINT NF NR OFMT "
    "OFS ORS PREC PROCINFO RLENGTH ROUNDMODE RS RSTART RT SUBSEP SYMTAB "
    "TEXTDOMAIN ";
static const char awk_fc[] = ""
    "break case continue default do else exit for if return switch while ";
static const char awk_bi[] = ""
    "and asort asorti atan2 bindtextdomain close compl cos dcgettext dcngetext "
    "delete exp fflush func function gensub getline gsub index int isarray "
    "length log lshift match mktime next nextfile or patsplit print printf "
    "rand rshift sin split sprintf sqrt srand strftime strtonum sub substr "
    "system systime tolower toupper typeof xor ";
#endif // AWK

#ifdef C
static const char c_names[] = "c h ";
static const struct rule c_rules[] = {
    {"#",   0,  COLOR_KEYWORD,      COLOR_KEYWORD},
    NULL_RULE
};
static const char c_cm[] = "// ";
static const char c_kw[] = ""
    "auto char const double enum extern float inline int long register "
    "restrict short signed sizeof static struct typedef union unsigned void "
    "volatile _Bool _Complex _Imaginary ";
static const char c_fc[] = ""
    "break case continue default do else for goto if return switch while ";
static const char c_bi[] = ""
    // assert.h
    "NDEBUG "
    "assert "
    // ctype.h
    "isalnum isalpha isblank iscntrl isdigit isgraph islower isprint ispunct "
    "isspace isupper isxdigit tolower toupper "
    // errno.h
    "EDOM EILSEQ ERANGE errno "
    // signal.h
    "sig_atomic_t SIG_DFL SIG_ERR SIG_IGN SIGABRT SIGFPE SIGILL SIGINT SIGSEGV "
    "SIGTERM "
    "signal raise "
    // stdarg.h
    "va_list "
    "va_arg va_copy va_end va_start "
    // stddef.h
    "ptrdiff_t size_t wchar_t NULL offsetof "
    // stdint.h
    "int8_t int16t int32_t int64_t uint8_t uint16_t uint32_t uint64_t "
    "intptr_t uintptr_t intmax_t uintmax_t "
    // stdio.h
    "size_t FILE fpos_t NULL _IOFBF _IOLBF _IONBF BUFSIZ EOF FOPEN_MAX "
    "FILENAME_MAX L_tmpnam SEEK_CUR SEEK_END SEEK_SET TMP_MAX stderr stdin "
    "stdout "
    "remove rename tmpfile tmpnam fclose fflush fopen freopen setbuf setvbuf "
    "fprintf fscanf printf scanf snprintf sprintf sscanf vfprintf vfscanf "
    "vprintf vscanf vsnprintf vsprintf vsscanf fgetc fgets fputc fputs getc "
    "getchar gets putc putchar puts ungetc fread fwrite fgetpos fseek fsetpos "
    "ftell rewind clearerr feof ferror perror "
    // stdlib.h
    "size_t wchar_t div_t ldiv_t lldiv_t NULL EXIT_FAILURE EXIT_SUCCESS "
    "RAND_MAX MB_CUR_MAX "
    "atof atoi atol atoll strtod strtof strtold strtol strtoll strtoul "
    "strtoull rand srand calloc free malloc realloc abort atexit exit _Exit "
    "getenv system bsearch qsort abs labs llabs div ldiv lldiv mblen mbtowc "
    "wctomb mbstowcs wcstombs "
    // string.h
    "memcpy memmove strcpy strncpy strcat strncat memcmp strcmp strcoll "
    "strncmp strxfrm memchr strchr strcspn strpbrk strrchr strspn strstr "
    "strtok memset strerror strlen "
    // tgmath.h
    "acos asin atan acosh asinh atanh cos sin tan cosh sinh tanh exp log pow "
    "sqrt fabs atan2 cbrt ceil copysign erf erfc exp2 expm1 fdim floor fma "
    "fmax fmin fmod frexp hypot ilogb ldexp lgamma llrint llround log10 log1p "
    "log2 logb lrint lround nearbyint nextafter nexttoward remainder remquo "
    "rint round scalbn scalbln tgamma trunc carg cimag conj cproj creal "
    // time.h
    "NULL CLOCKS_PER_SEC size_t clock_t time_t tm "
    "clock difftime mktime time asctime ctime gmtime localtime strftime ";
#endif // C

#ifdef C99_FULL
static const char c99_full_names[] = "c h ";
static const struct rule c99_full_rules[] = {
    {"#",   0,  COLOR_KEYWORD,      COLOR_KEYWORD},
    NULL_RULE
};
static const char c99_full_cm[] = "// ";
static const char c99_full_kw[] = ""
    "auto char const double enum extern float inline int long register "
    "restrict short signed sizeof static struct typedef union unsigned void "
    "volatile _Bool _Complex _Imaginary ";
static const char c99_full_fc[] = ""
    "break case continue default do else for goto if return switch while ";
static const char c99_full_bi[] = ""
    // assert.h
    "NDEBUG "
    "assert "
    // complex.h
    "complex imaginary I _Complex_I _Imaginary_I "
    "cacos cacosf cacosl casin casinf casinl catan catanf catanl ccos ccosf "
    "ccosl csin csinf csinl ctan ctanf ctanl cacosh cacoshf cacoshl casinh "
    "casinhf casinhl catanh catanhf catanhl ccosh ccoshf ccoshl csinh csinhf "
    "csinhl ctanh ctanhf ctanhl cexp cexpf cexpl clog clogf clogl cabs cabsf "
    "cabsl cpow cpowf cpowl csqrt csqrtf csqrtl carg cargf cargl cimag cimagf "
    "cimagl conj conjf conjl cproj cprojf cprojl creal crealf creall "
    // ctype.h
    "isalnum isalpha isblank iscntrl isdigit isgraph islower isprint ispunct "
    "isspace isupper isxdigit tolower toupper "
    // errno.h
    "EDOM EILSEQ ERANGE errno "
    // fenv.h
    "fenv_t fexcept_t FE_DIVBYZERO FE_INEXACT FE_INVALID FE_OVERFLOW "
    "FE_UNDERFLOW FE_ALL_EXCEPT FE_DOWNWARD FE_TONEAREST FE_TOWARDZERO "
    "FE_UPWARD FE_DFL_ENV "
    "feclearexcept fegetexceptflag feraiseexcept fesetexceptflag fetestexcept "
    "fegetround fesetround fegetenv feholdexcept fesetenv feupdateenv "
    // float.h
    "FLT_ROUNDS FLT_EVAL_METHOD FLT_RADIX FLT_MANT_DIG DBL_MANT_DIG "
    "LDBL_MANT_DIG DECIMAL_DIG FLT_DIG DBL_DIG LDBL_DIG FLT_MIN_EXP "
    "DBL_MIN_EXP LDBL_MIN_EXP FLT_MIN_10_EXP DBL_MIN_10_EXP LDBL_MIN_10_EXP "
    "FLT_MAX_EXP DBL_MAX_EXP LDBL_MAX_EXP FLT_MAX_10_EXP DBL_MAX_10_EXP "
    "LDBL_MAX_10_EXP FLT_MAX DBL_MAX LDBL_MAX FLT_EPSILON DBL_EPSILON "
    "LDBL_EPSILON FLT_MIN DBL_MIN LDBL_MIN "
    // inttypes.h
    "imaxdiv_t "
    "PRId8 PRId16 PRId32 PRId64 PRIdMAX PRIdPTR "
    "PRIdLEAST8 PRIdLEAST16 PRIdLEAST32 PRIdLEAST64 "
    "PRIdFAST8 PRIdFAST16 PRIdFAST32 PRIdFAST64 "
    "PRIi8 PRIi16 PRIi32 PRIi64 PRIiMAX PRIiPTR "
    "PRIiLEAST8 PRIiLEAST16 PRIiLEAST32 PRIiLEAST64 "
    "PRIiFAST8 PRIiFAST16 PRIiFAST32 PRIiFAST64 "
    "PRIo8 PRIo16 PRIo32 PRIo64 PRIoMAX PRIoPTR "
    "PRIoLEAST8 PRIoLEAST16 PRIoLEAST32 PRIoLEAST64 "
    "PRIoFAST8 PRIoFAST16 PRIoFAST32 PRIoFAST64 "
    "PRIu8 PRIu16 PRIu32 PRIu64 PRIuMAX PRIuPTR "
    "PRIuLEAST8 PRIuLEAST16 PRIuLEAST32 PRIuLEAST64 "
    "PRIuFAST8 PRIuFAST16 PRIuFAST32 PRIuFAST64 "
    "PRIx8 PRIx16 PRIx32 PRIx64 PRIxMAX PRIxPTR "
    "PRIxLEAST8 PRIxLEAST16 PRIxLEAST32 PRIxLEAST64 "
    "PRIxFAST8 PRIxFAST16 PRIxFAST32 PRIxFAST64 "
    "PRIX8 PRIX16 PRIX32 PRIX64 PRIXMAX PRIXPTR "
    "PRIXLEAST8 PRIXLEAST16 PRIXLEAST32 PRIXLEAST64 "
    "PRIXFAST8 PRIXFAST16 PRIXFAST32 PRIXFAST64 "
    "SCNd8 SCNd16 SCNd32 SCNd64 SCNdMAX SCNdPTR "
    "SCNdLEAST8 SCNdLEAST16 SCNdLEAST32 SCNdLEAST64 "
    "SCNdFAST8 SCNdFAST16 SCNdFAST32 SCNdFAST64 "
    "SCNi8 SCNi16 SCNi32 SCNi64 SCNiMAX SCNiPTR "
    "SCNiLEAST8 SCNiLEAST16 SCNiLEAST32 SCNiLEAST64 "
    "SCNiFAST8 SCNiFAST16 SCNiFAST32 SCNiFAST64 "
    "SCNo8 SCNo16 SCNo32 SCNo64 SCNoMAX SCNoPTR "
    "SCNoLEAST8 SCNoLEAST16 SCNoLEAST32 SCNoLEAST64 "
    "SCNoFAST8 SCNoFAST16 SCNoFAST32 SCNoFAST64 "
    "SCNu8 SCNu16 SCNu32 SCNu64 SCNuMAX SCNuPTR "
    "SCNuLEAST8 SCNuLEAST16 SCNuLEAST32 SCNuLEAST64 "
    "SCNuFAST8 SCNuFAST16 SCNuFAST32 SCNuFAST64 "
    "SCNx8 SCNx16 SCNx32 SCNx64 SCNxMAX SCNxPTR "
    "SCNxLEAST8 SCNxLEAST16 SCNxLEAST32 SCNxLEAST64 "
    "SCNxFAST8 SCNxFAST16 SCNxFAST32 SCNxFAST64 "
    "imaxabs imaxdiv strtoimax strtoumax wcstoimax wcstoumax "
    // iso646.h
    "and and_eq bitand bitor compl not not_eq or or_eq xor xor_eq "
    // limits.h
    "CHAR_BIT SCHAR_MIN SCHAR_MAX UCHAR_MAX CHAR_MIN CHAR_MAX MB_LEN_MAX "
    "SHRT_MIN SHRT_MAX USHRT_MAX INT_MIN INT_MAX UINT_MAX LONG_MIN LONG_MAX "
    "ULONG_MAX LLONG_MIN LLONG_MAX ULLONG_MAX "
    // locale.h
    "lconv NULL LC_ALL LC_COLLATE LC_CTYPE LC_MONETARY LC_NUMERIC LC_TIME "
    "setlocale localeconv "
    // math.h
    "float_t double_t HUGE_VAL HUGE_VALF HUGE_VALL INFINITY NAN FP_INFINITE "
    "FP_NAN FP_NORMAL FP_SUBNORMAL FP_ZERO FP_FAST_FMA FP_FAST_FMAF "
    "FP_FAST_FMAL FP_ILOGB0 FP_ILOGBNAN MATH_ERRNO MATH_ERREXCEPT "
    "math_errhandling "
    "fpclassify isfinite isinf isnan isnormal signbit acos acosf acosl asin "
    "asinf asinl atan atanf atanl atan2 atan2f atan2l cos cosf cosl sin sinf "
    "sinl tan tanf tanl acosh acoshf acoshl asinh asinhf asinhl atanh atanhf "
    "atanhl cosh coshf coshl sinh sinhf sinhl tanh tanhf tanhl exp expf expl "
    "exp2 exp2f exp2l expm1 expm1f expm1l frexp frexpf frexpl ilogb ilogbf "
    "ilogbl ldexp ldexpf ldexpl log logf logl log10 log10f log10l log1p log1pf "
    "log1pl log2 log2f log2l logb logbf logbl modf modff modfl scalbn scalbnf "
    "scalbnl scalbln scalblnf scalblnl cbrt cbrtf cbrtl fabs fabsf fabsl hypot "
    "hypotf hypotl pow powf powl sqrt sqrtf sqrtl erf erff erfl erfc erfcf "
    "erfcl lgamma lgammaf lgammal tgamma tgammaf tgammal ceil ceilf ceill "
    "floor floorf floorl nearbyint nearbyintf nearbyintl rint rintf rintl "
    "lrint lrintf lrintl llrint llrintf llrintl round roundf roundl lround "
    "lroundf lroundl llround llroundf llroundl trunc truncf truncl fmod fmodf "
    "fmodl remainder remainderf remainderl remquo remquof remquol copysign "
    "copysignf copysignl nan nanf nanl nextafter nextafterf nextafterl "
    "nexttoward nexttowardf nexttowardl fdim fdimf fdiml fmax fmaxf fmaxl fmin "
    "fminf fminl fma fmaf fmal isgreater isgreaterequal isless islessequal "
    "islessgreater isunordered "
    // setjmp.h
    "jmp_buf "
    "setjmp longjmp "
    // signal.h
    "sig_atomic_t SIG_DFL SIG_ERR SIG_IGN SIGABRT SIGFPE SIGILL SIGINT SIGSEGV "
    "SIGTERM "
    "signal raise "
    // stdarg.h
    "va_list "
    "va_arg va_copy va_end va_start "
    // stdbool.h
    "bool true false __bool_true_false_are_defined "
    // stddef.h
    "ptrdiff_t size_t wchar_t NULL offsetof "
    // stdint.h
    "int8_t int16t int32_t int64_t uint8_t uint16_t uint32_t uint64_t "
    "int_least8_t int_least16_t int_least32_t int_least64_t "
    "uint_least8_t uint_least16_t uint_least32_t uint_least64_t "
    "int_fast8_t int_fast16_t int_fast32_t int_fast64_t "
    "uint_fast8_t uint_fast16_t uint_fast32_t uint_fast64_t "
    "intptr_t uintptr_t intmax_t uintmax_t "
    "INT8_MIN INT16_MIN INT32_MIN INT64_MIN "
    "INT8_MAX INT16_MAX INT32_MAX INT64_MAX "
    "UINT8_MAX UINT16_MAX UINT32_MAX UINT64_MAX "
    "INT_LEAST8_MIN INT_LEAST16_MIN INT_LEAST32_MIN INT_LEAST64_MIN "
    "INT_LEAST8_MAX INT_LEAST16_MAX INT_LEAST32_MAX INT_LEAST64_MAX "
    "UINT_LEAST8_MAX UINT_LEAST16_MAX UINT_LEAST32_MAX UINT_LEAST64_MAX "
    "INT_FAST8_MIN INT_FAST16_MIN INT_FAST32_MIN INT_FAST64_MIN "
    "INT_FAST8_MAX INT_FAST16_MAX INT_FAST32_MAX INT_FAST64_MAX "
    "UINT_FAST8_MAX UINT_FAST16_MAX UINT_FAST32_MAX UINT_FAST64_MAX "
    "INTPTR_MIN INTPTR_MAX UINTPTR_MAX INTMAX_MIN INTMAX_MAX UINTMAX_MAX "
    "PTRDIFF_MIN PTRDIFF_MAX SIG_ATOMIC_MIN SIG_ATOMIC_MAX SIZE_MAX "
    "WCHAR_MIN WCHAR_MAX WINT_MIN WINT_MAX "
    "INT8_C INT16C INT32_C INT64_C UINT8_C UINT16C UINT32_C UINT64_C "
    "INTMAX_C UINTMAX_C "
    // stdio.h
    "size_t FILE fpos_t NULL _IOFBF _IOLBF _IONBF BUFSIZ EOF FOPEN_MAX "
    "FILENAME_MAX L_tmpnam SEEK_CUR SEEK_END SEEK_SET TMP_MAX stderr stdin "
    "stdout "
    "remove rename tmpfile tmpnam fclose fflush fopen freopen setbuf setvbuf "
    "fprintf fscanf printf scanf snprintf sprintf sscanf vfprintf vfscanf "
    "vprintf vscanf vsnprintf vsprintf vsscanf fgetc fgets fputc fputs getc "
    "getchar gets putc putchar puts ungetc fread fwrite fgetpos fseek fsetpos "
    "ftell rewind clearerr feof ferror perror "
    // stdlib.h
    "size_t wchar_t div_t ldiv_t lldiv_t NULL EXIT_FAILURE EXIT_SUCCESS "
    "RAND_MAX MB_CUR_MAX "
    "atof atoi atol atoll strtod strtof strtold strtol strtoll strtoul "
    "strtoull rand srand calloc free malloc realloc abort atexit exit _Exit "
    "getenv system bsearch qsort abs labs llabs div ldiv lldiv mblen mbtowc "
    "wctomb mbstowcs wcstombs "
    // string.h
    "size_t NULL "
    "memcpy memmove strcpy strncpy strcat strncat memcmp strcmp strcoll "
    "strncmp strxfrm memchr strchr strcspn strpbrk strrchr strspn strstr "
    "strtok memset strerror strlen "
    // tgmath.h
    "acos asin atan acosh asinh atanh cos sin tan cosh sinh tanh exp log pow "
    "sqrt fabs atan2 cbrt ceil copysign erf erfc exp2 expm1 fdim floor fma "
    "fmax fmin fmod frexp hypot ilogb ldexp lgamma llrint llround log10 log1p "
    "log2 logb lrint lround nearbyint nextafter nexttoward remainder remquo "
    "rint round scalbn scalbln tgamma trunc carg cimag conj cproj creal "
    // time.h
    "NULL CLOCKS_PER_SEC size_t clock_t time_t tm "
    "clock difftime mktime time asctime ctime gmtime localtime strftime "
    // wchar.h
    "wchar_t size_t mbstate_t wint_t tm NULL WCHAR_MAX WCHAR_MIN WEOF "
    "fwprintf fwscanf swprintf swscanf vfwprintf vfwscanf vswprintf vswscanf "
    "vwprintf vwscanf wprintf wscanf fgetwc fgetws fputwc fputws fwide getwc "
    "getwchar putwc putwchar ungetwc wcstod wcstof wcstold wcstol wcstoll "
    "wcstoul wcstoull wcscpy wcsncpy wmemcpy wmemmove wcscat wcsncat wcscmp "
    "wcscoll wcsncmp wcsxfrm wmemcmp wcschr wcscspn wcspbrk wcsrchr wcsspn "
    "wcsstr wcstok wmemchr wcslen wmemset wcsftime btowc wctob mbsinit mbrlen "
    "mbrtowc wcrtomb mbsrtowcs wcsrtombs "
    // wctype.h
    "wint_t wctrans_t wctype_t WEOF "
    "iswalnum iswalpha iswblank iswcntrl iswdigit iswgraph iswlower iswprint "
    "iswpunct iswspace iswupper iswxdigit iswctype wctype towlower towupper "
    "towctrans wctrans ";
#endif // C99_FULL

#ifdef DIFF
static const char diff_names[] = "diff patch ";
static const struct rule diff_rules[] = {
    {"@@",  1,  COLOR_FLOW_CONTROL, COLOR_FLOW_CONTROL},
    {"+",   1,  34,                 34},
    {"-",   1,  196,                196},
    NULL_RULE
};
#endif // DIFF

#ifdef GEMTEXT
static const char gemtext_names[] = "gmi ";
static const struct rule gemtext_rules[] = {
    {"=>",  1,  COLOR_FLOW_CONTROL, COLOR_FLOW_CONTROL},
    {"# ",  1,  COLOR_KEYWORD,      COLOR_KEYWORD},
    {"## ", 1,  COLOR_KEYWORD,      COLOR_KEYWORD},
    {"### ",1,  COLOR_KEYWORD,      COLOR_KEYWORD},
    {"* ",  1,  COLOR_FLOW_CONTROL, COLOR_DEFAULT},
    {">",   1,  COLOR_COMMENT,      COLOR_DEFAULT},
    NULL_RULE
};
#endif // GEMTEXT

#ifdef HTML
static const char html_names[] = "htm html ";
static const struct rule html_rules[] = {NULL_RULE};
static const char html_cm[] = "<!-- ";
static const char html_kw[] = ""
    "DOCTYPE a abbr address area article aside audio b base bdi bdo blockquote "
    "body br button canvas caption cite code col colgroup data datalist dd del "
    "details dfn dialog div dl dt em embed fieldset figcaption figure footer "
    "form h1 h2 h3 h4 h5 h6 head header hr html i iframe img input ins kbd "
    "label legend li link main map mark meta meter nav noscript object ol "
    "optgroup option output p param picture pre progress q rp rt ruby s samp "
    "script section select small source span strong style sub summary sup svg "
    "table tbody td template textarea tfoot th thead time title tr track u ul "
    "var video wbr ";
static const char html_fc[] = "";
static const char html_bi[] = "";
#endif // HTML

#ifdef JAVA
static const char java_names[] = "java ";
static const struct rule java_rules[] = {NULL_RULE};
static const char java_cm[] = "// ";
static const char java_kw[] = ""
    "abstract boolean byte char class const double enum extends final float "
    "implements import instanceof int interface long native new package "
    "private protected public short static strictfp super synchronized "
    "this throws transient void volatile ";
static const char java_fc[] = ""
    "assert break case catch continue default do else finally for goto if "
    "return switch throw try while ";
static const char java_bi[] = "";
#endif // JAVA

#ifdef LATEX
static const char latex_names[] = "tex cls ";
static const struct rule latex_rules[] = {NULL_RULE};
static const char latex_cm[] = "% ";
static const char latex_kw[] = ""
    "NeedsTeXFormat ProvidesClass LoadClass documentclass usepackage "
    "def newcommand renewcommand setcounter "
    "part chapter section subsection subsubsection paragraph subparagraph ";
static const char latex_fc[] = ""
    "input begin end footnote item "
    "includegraphics lstinputlisting addbibresource includepdf ";
static const char latex_bi[] = ""
    // "article report book memoir "
    "title author date addcontentsline "
    "ref pageref cite nocite label caption "
    "textwidth textheight linewidth lineheight "
    "document abstract minipage figure subfigure table tabular equation "
    "lstlisting itemize enumerate description quote "
    "flushleft center flushright "
    "listoffigures listoftables lstlistoflistings printbibliography "
    "tableofcontents maketitle clearpage phantomsection "
    "appendix frontmatter mainmatter backmatter "
    "rule toprule midrule bottomrule "
    "textit textsc textbf textmd emph textsc textsf texttt "
    "tiny scriptsize footnotesize small normalsize large Large LARGE huge Huge "
    "centering hspace hfill smallskip medskip bigskip vspace vfill ";
#endif // LATEX

#ifdef MAKEFILE
static const char mk_names[] = "Makefile makefile mk ";
static const struct rule mk_rules[] = {NULL_RULE};
static const char mk_cm[] = "# ";
static const char mk_kw[] = "PHONY include ";
static const char mk_fc[] = "";
static const char mk_bi[] = "";
#endif // MAKEFILE

#ifdef MANPAGE
static const char mp_names[] = "1 2 3 4 5 6 7 8 9 ";
static const struct rule mp_rules[] = {
    {"'",   1,  COLOR_COMMENT,      COLOR_COMMENT},
    {".EE", 1,  COLOR_KEYWORD,      COLOR_DEFAULT},
    {".EX", 1,  COLOR_KEYWORD,      COLOR_DEFAULT},
    {".RE", 1,  COLOR_KEYWORD,      COLOR_DEFAULT},
    {".RS", 1,  COLOR_KEYWORD,      COLOR_DEFAULT},
    {".SH", 1,  COLOR_KEYWORD,      COLOR_BUILT_IN},
    {".SS", 1,  COLOR_KEYWORD,      COLOR_KEYWORD},
    {".TH", 1,  COLOR_KEYWORD,      COLOR_DEFAULT},
    {".SY", 1,  COLOR_FLOW_CONTROL, COLOR_DEFAULT},
    {".YS", 1,  COLOR_FLOW_CONTROL, COLOR_DEFAULT},
    {".IP", 1,  COLOR_FLOW_CONTROL, COLOR_DEFAULT},
    {".LP", 1,  COLOR_FLOW_CONTROL, COLOR_DEFAULT},
    {".P",  1,  COLOR_FLOW_CONTROL, COLOR_DEFAULT},
    {".TP", 1,  COLOR_FLOW_CONTROL, COLOR_DEFAULT},
    {".TQ", 1,  COLOR_FLOW_CONTROL, COLOR_DEFAULT},
    {".BI", 1,  COLOR_FLOW_CONTROL, COLOR_DEFAULT},
    {".BR", 1,  COLOR_FLOW_CONTROL, COLOR_DEFAULT},
    {".B",  1,  COLOR_FLOW_CONTROL, COLOR_DEFAULT},
    {".IB", 1,  COLOR_FLOW_CONTROL, COLOR_DEFAULT},
    {".IR", 1,  COLOR_FLOW_CONTROL, COLOR_DEFAULT},
    {".I",  1,  COLOR_FLOW_CONTROL, COLOR_DEFAULT},
    {".RB", 1,  COLOR_FLOW_CONTROL, COLOR_DEFAULT},
    {".RI", 1,  COLOR_FLOW_CONTROL, COLOR_DEFAULT},
    {".SB", 1,  COLOR_FLOW_CONTROL, COLOR_DEFAULT},
    {".SM", 1,  COLOR_FLOW_CONTROL, COLOR_DEFAULT},
    {".ME", 1,  COLOR_FLOW_CONTROL, COLOR_DEFAULT},
    {".MR", 1,  COLOR_FLOW_CONTROL, COLOR_DEFAULT},
    {".MT", 1,  COLOR_FLOW_CONTROL, COLOR_DEFAULT},
    {".UE", 1,  COLOR_FLOW_CONTROL, COLOR_DEFAULT},
    {".UR", 1,  COLOR_FLOW_CONTROL, COLOR_DEFAULT},
    NULL_RULE
};
#endif // MANPAGE

#ifdef MARKDOWN
static const char md_names[] = "md ";
static const struct rule md_rules[] = {
    {"###",  1,  COLOR_KEYWORD,      COLOR_KEYWORD},
    {"##",   1,  COLOR_KEYWORD,      COLOR_KEYWORD},
    {"#",    1,  COLOR_KEYWORD,      COLOR_KEYWORD},
    {"***",  1,  COLOR_KEYWORD,      COLOR_KEYWORD},
    {"---",  1,  COLOR_KEYWORD,      COLOR_KEYWORD},
    {"___",  1,  COLOR_KEYWORD,      COLOR_KEYWORD},
    {">",    1,  COLOR_COMMENT,      COLOR_DEFAULT},
    {"-",    0,  COLOR_FLOW_CONTROL, COLOR_DEFAULT},
    {"*",    0,  COLOR_FLOW_CONTROL, COLOR_DEFAULT},
    {"+",    0,  COLOR_FLOW_CONTROL, COLOR_DEFAULT},
    {"    ", 1,  COLOR_DEFAULT,      COLOR_DEFAULT},
    NULL_RULE
};
#endif // MARKDOWN

#ifdef PYTHON
static const char py_names[] = "py ";
struct rule py_rules[] = {NULL_RULE};
static const char py_cm[] = "# ";
static const char py_kw[] = ""
    "False None True and in is not or as assert del global ";
static const char py_fc[] = ""
    "while for if else elif try except finally with break continue pass return "
    "yield lambda class def import from raise async await ";
static const char py_bi[] = ""
    "abs all any ascii bin bool breakpoint bytearray bytes callable chr "
    "classmethod compile complex delattr dict dir divmod enumerate eval exec "
    "filter float format frozenset getattr globals hasattr hash help hex id "
    "input int isinstance issubclass iter len list locals map max memoryview "
    "min next object oct open ord pow print property range repr reversed round "
    "set setattr slice sorted staticmethod str sum super tuple type vars zip "
    "__import__ ";
#endif // PYTHON

#ifdef OCAML
static const char ml_names[] = "ml mli mll mly ";
static const struct rule ml_rules[] = {
    {"#",   1,  COLOR_KEYWORD,      COLOR_KEYWORD},
    NULL_RULE
};
static const char ml_cm[] = "(* ";
static const char ml_kw[] = ""
    "as assert asr class constraint external false fun function functor "
    "inherit include inherit initializer land lazy lor lsl lsr lxor method mod "
    "module mutable new nonrec object of open or private rec sig struct true "
    "type val virtual array bool char exn float format format4 int int32 int64 "
    "lazy_t list nativeint option bytes string unit ";
static const char ml_fc[] = ""
    "and begin do done downto else end exception for if in let match then to "
    "try when while with ";
static const char ml_bi[] = "";
#endif // OCAML

#ifdef SH
static const char sh_names[] = "sh ";
static const struct rule sh_rules[] = {NULL_RULE};
static const char sh_cm[] = "# ";
static const char sh_kw[] = "";
static const char sh_fc[] = ""
    "case do done elif else esac fi for if in then until while ";
static const char sh_bi[] = ""
    "break cd continue echo eval exec exit export getopts hash newgrp pwd read "
    "readonly return set shift times test trap type ulimit umask unset wait ";
#endif // SH

#ifdef SQLITE
static const char sql_names[] = "sql sqlite ";
static const struct rule sql_rules[] = {NULL_RULE};
static const char sql_cm[] = "-- ";
static const char sql_kw[] = ""
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
static const char sql_fc[] = "";
static const char sql_bi[] = ""
    "AVG COUNT GROUP_CONCAT MAX MIN SUM TOTAL "
    "date time datetime julianday unixepoch strftime timediff "
    "analysis_limit application_id auto_vacuum automatic_index busy_timeout "
    "cache_size cache_spill case_sensitive_like cell_size_check "
    "checkpoint_fullfsync collation_list compile_options data_version "
    "database_list defer_foreign_keys encoding foreign_key_check "
    "foreign_key_list foreign_keys freelist_count fullfsync function_list "
    "hard_heap_limit ignore_check_constraints incremental_vacuum index_info "
    "index_list index_xinfo integrity_check journal_mode journal_size_limit "
    "legacy_alter_table legacy_file_format locking_mode max_page_count "
    "mmap_size module_list optimize page_count page_size parser_trace "
    "pragma_list query_only quick_check read_uncommitted recursive_triggers "
    "reverse_unordered_selects schema_version secure_delete shrink_memory "
    "soft_heap_limit stats synchronous table_info table_list table_xinfo "
    "temp_store threads trusted_schema user_version vdbe_addoptrace vdbe_debug "
    "vdbe_listing vdbe_trace wal_autocheckpoint wal_checkpoint "
    "writable_schema ";
#endif // SQLITE

#ifdef VHDL
static const char vhdl_names[] = "vhd vhdl ";
static const struct rule vhdl_rules[] = {NULL_RULE};
static const char vhdl_cm[] = "-- ";
static const char vhdl_kw[] = ""
    // keywords
    "access after alias all architecture array attribute assert assume block "
    "body buffer bus component configuration constant context cover disconnect "
    "downto entity file fairness force generate generic group guarded impure "
    "in inertial inout label library linkage literal map new null of on open "
    "out package port postponed procedure process pure parameter property "
    "protected private range record register reject report release restrict "
    "select severity signal shared subtype sequence strong to transport type "
    "unaffected units until use variable view vpkg vmode vprop vunit wait with "
    "true false "
    // types
    "bit boolean natural positive integer real time bit_vector boolean_vector "
    "integer_vector real_vector time_vector character string std_ulogic "
    "std_logic std_ulogic_vector std_logic_vector unresolved_signed "
    "unresolved_unsigned u_signed u_unsigned signed unsigned line text side "
    "width ";
static const char vhdl_fc[] = ""
    "begin case else elsif end exit for function if is loop next others return "
    "then when while ";
static const char vhdl_bi[] = ""
    // atttributes
    "high left length low range reverse_range right ascending simple_name "
    "instance_name path_name foreign active delayed event last_active "
    "last_event last_value quiet stable transaction driving driving_value base "
    "subtype element leftof pos pred rightof succ val image value converse "
    // operators
    "and nand or nor xor xnor rol ror sla sll sra srl mod rem abs not ";
#endif // VHDL

static const struct lang languages[] = {
#ifdef ASM
    SYNTAX_LANG(asm, HIGHLIGHT_CONSTANTS),
#endif
#ifdef AWK
    SYNTAX_LANG(awk, HIGHLIGHT_CONSTANTS),
#endif
#ifdef C
    SYNTAX_LANG(c, HIGHLIGHT_CONSTANTS),
#endif
#ifdef C99_FULL
    SYNTAX_LANG(c99_full, HIGHLIGHT_CONSTANTS),
#endif
#ifdef DIFF
    NO_SYNTAX_LANG(diff, 0),
#endif
#ifdef GEMTEXT
    NO_SYNTAX_LANG(gemtext, 0),
#endif
#ifdef HTML
    SYNTAX_LANG(html, HIGHLIGHT_CONSTANTS),
#endif
#ifdef JAVA
    SYNTAX_LANG(java, HIGHLIGHT_CONSTANTS),
#endif
#ifdef LATEX
    SYNTAX_LANG(latex, 0),
#endif
#ifdef MAKEFILE
    SYNTAX_LANG(mk, CONVERT_LEADING_SPACES),
#endif
#ifdef MANPAGE
    NO_SYNTAX_LANG(mp, 0),
#endif
#ifdef MARKDOWN
    NO_SYNTAX_LANG(md, 0),
#endif
#ifdef PYTHON
    SYNTAX_LANG(py, HIGHLIGHT_CONSTANTS),
#endif
#ifdef OCAML
    SYNTAX_LANG(ml, HIGHLIGHT_CONSTANTS),
#endif
#ifdef SH
    SYNTAX_LANG(sh, HIGHLIGHT_CONSTANTS),
#endif
#ifdef SQLITE
    SYNTAX_LANG(sql, HIGHLIGHT_CONSTANTS),
#endif
#ifdef VHDL
    SYNTAX_LANG(vhdl, HIGHLIGHT_CONSTANTS),
#endif
    {0}
};
