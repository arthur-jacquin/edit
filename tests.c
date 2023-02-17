#include "globals.h"
#include "utils.c"
#include "search_and_replace.c"

void
test(char *sp, char *chars, int x, int n, int expected)
{
    // test the search engine, print something if an error is detected

    int res;

    strcpy(search_pattern.current, sp);
    res = mark_pattern(chars, x, n);
    if (res != expected)
        printf("sp = \"%s\", chars = \"%s\", x = %d: expected %d, got %d.\n",
            sp, chars, x, expected, res);
}

int
main(int argc, char **argv)
{
    // simple characters
    test("a", "a", 0, 1, 1);
    test("a", "b", 0, 1, 0);
    test("Arthur Jacquin", "Arthur Jacquin", 0, 14, 14);
    test("é", "é", 0, 1, 1);
    test("a", " a", 1, 1, 1);
    test("à", " à", 1, 1, 1);

    // partial match
    test("a", "aa", 0, 2, 1);

//    test("Arte|t", "Arthur", 0);
//    test("Art\\(h\\)|eur", "Arthur", 6);
//    test("Art\\(h\\)|\\(lol\\)ur", "Arthur", 6);
//    test("Arte|\\(h\\)ur", "Arthur", 6);
//    test("Arth|e|\\(lol\\)ur", "Arthur", 6);
//    test("Arte|\\(lol\\)|u|hur", "Arthur", 6);
//    test("Art[yui]ur", "Arthur", 0);
//    test("Art[yuhi]ur", "Arthur", 6);
//    test("Art[^yui]ur", "Arthur", 6);
//    test("Art[^yuhi]ur", "Arthur", 0);
//    test("Art[a-z]", "Arthur", 4);
//    test("Art[0-9]", "Arthur", 0);
//    test("Art[A-Za-z-]", "Art-ur", 4);
//    test("Art[]", "Arthur", 0);
//    test("Art[h", "Arthur", 0);
//    test("Arthur", "Arthur", 6);
//    test("^Arthur", "Arthur", 6);
//    test("^Arthur$", "Arthur", 6);
//    test("Arthur$", "Arthur", 6);
//    test("Arth^ur", "Arthur", 0);
//    test("Arth$ur", "Arthur", 0);
//    test("u? ", " ", 1);
//    test("u?", "u", 1);
//    test("u?", "uu", 1);
//    test("u? ", "uu", 0);
//    test("u+ ", " ", 0);
//    test("u+", " ", 0);
//    test("u* ", " ", 1);
//    test("u*", " ", 0);
//    test("u+", "u", 1);
//    test("u+", "uu", 2);
//    test("u*", "u", 1);
//    test("u*", "uuuu", 4);
//    test("r{1,5}", "rrr", 3);
//    test("r{1,2}", "rrr", 2);
//    test("r{,5}", "rrr", 3);
//    test("r{,2}", "rrr", 2);
//    test("r{2,}", "rrr", 3);
//    test("r{3}", "rrr", 3);
//    test("r{4,5}", "rrr", 0);
//    test("r{4,}", "rrr", 0);
//    test("r{4}", "rrr", 0);
//    test("A{,2}r*y?t+r*o{1}", "AAttrrro", 8);
//    test("\\(ar\\)?thur", "arthur", 6);
//    test("\\(ar\\)+thur", "arthur", 6);
//    test("\\(ar\\)*thur", "arthur", 6);
//    test("\\(ar\\)?thur", "thur", 4);
//    test("\\(ar\\)+thur", "thur", 0);
//    test("\\(ar\\)*thur", "thur", 4);
//    test("\\(ar\\)?thur", "ararthur", 0);
//    test("\\(ar\\)+thur", "ararthur", 8);
//    test("\\(ar\\)*thur", "ararthur", 8);
//    test("\\(ar\\){2}thur", "ararthur", 8);
//    test("\\(ar\\){1}thur", "ararthur", 0);
//    test("\\(ar\\){3}thur", "ararthur", 0);
//    test("\\(ar\\){2,}thur", "ararthur", 8);
//    test("\\(ar\\){3,}thur", "ararthur", 0);
//    test("\\(ar\\){,2}thur", "ararthur", 8);
//    test("\\(ar\\){,1}thur", "ararthur", 0);
//    test("\\(A?r*\\)thu\\(r{2}\\)", "Arthurr", 7);
//    //printf("%d, %d\n", subpatterns[1].st, subpatterns[1].n); // 0, 2
//    //printf("%d, %d\n", subpatterns[2].st, subpatterns[2].n); // 5, 2
//    test("\\(ar\\){,2}thur", "ararthur", 8);
//    //printf("%d, %d\n", subpatterns[1].st, subpatterns[1].n); // 0, 4
//    test("\\(lol\\)?\\(ar\\)*thur", "arararthur", 10);
//    //printf("%d, %d\n", subpatterns[2].st, subpatterns[2].n); // 0, 6
//
//    test("a*\\(.*\\)", "aaaaabrout", 10);
//    //printf("%d, %d\n", subpatterns[1].st, subpatterns[1].n); // 5, 5
//
//    test("\\(a\\){4}", "aa", 0);
//    test("\\(a\\){2}", "aa", 2);
//    test("\\(a\\){1}", "aa", 1);
//
//    strcpy(search_pattern.current, "Il y a \\(\\d*\\) \\(camions\\)|\\(voitures\\)");
//    strcpy(replace_pattern.current, "En \\2\\3, il y en a \\1.");
//    char string[] = "Il y a 42 camions";
//    _replace(string, 0, strlen(string), strlen(string));
//    printf("\"%s\" BECOMES \"%s\"\n", string, replaced);

}
