// See LICENSE file for copyright and license details.

// macros
#define NULL_RULE                   {""}
#define CONVERT_LEADING_SPACES      (1 << 0)
#define DEFINED                     (1 << 1)
#define ONLY_RULES                  (1 << 2)
#define LANG(L, F) \
    {F | DEFINED, L##_names, L##_rules, L##_cm, L##_kw, L##_fc, L##_bi}
#define ONLY_RULES_LANG(L, F) \
    {F | DEFINED | ONLY_RULES, L##_names, L##_rules}

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

static const struct lang languages[] = {
#ifdef C
    LANG(c, 0),
#endif
#ifdef C99_FULL
    LANG(c99_full, 0),
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
#ifdef MANPAGE
    ONLY_RULES_LANG(mp, 0),
#endif
#ifdef MARKDOWN
    ONLY_RULES_LANG(md, 0),
#endif
    {0}
};
