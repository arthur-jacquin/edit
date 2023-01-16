#include <stdio.h>
#include <string.h>

int is_blank(char c);
int is_word_char(char c);
int is_number(char c);
int is_in(const char *list, const char *chars, int x, int length);

void test(char *sp, char *chars, int expected);
int mark_pattern(char *sp, char *chars, int x, int n, int line_length);
int replace(char *chars, int x, int n, int line_length);

struct substring {
    int st;                             // starting position in original string
    int n;                              // number of characters
};
struct substring fields[10];
struct substring subpatterns[10];
    
char sep = ',';

char sp[] = "Arthu+rtla";
char rp[] = "$2,$1: $0";
char replaced[100]; // TODO: manage size interactively

int
main(int argc, char **argv)
{

    test("Arthur", "Arthur", 6);
    test("^Arthur", "Arthur", 6);
    test("^Arthur$", "Arthur", 6);
    test("Arthur$", "Arthur", 6);
    test("Arth^ur", "Arthur", 0);
    test("Arth$ur", "Arthur", 0);
    test("u? ", " ", 1);
    test("u?", "u", 1);
    test("u?", "uu", 1);
    test("u? ", "uu", 0);
    test("u+ ", " ", 0);
    test("u* ", " ", 1);
    test("u+", "u", 1);
    test("u+", "uu", 2);
    test("u*", "u", 1);
    test("u*", "uuuu", 4);
    test("r{1,5}", "rrr", 3);
    test("r{1,2}", "rrr", 2);
    test("r{,5}", "rrr", 3);
    test("r{,2}", "rrr", 2);
    test("r{2,}", "rrr", 3);
    test("r{3}", "rrr", 3);
    test("r{4,5}", "rrr", 0);
    test("r{4,}", "rrr", 0);
    test("r{4}", "rrr", 0);
    test("A{,2}r*y?t+r*o{1}", "AAttrrro", 8);

    test("\\(Ar\\)thur\\(\\)", "Arthur", 6);
    printf("%d, %d\n", subpatterns[1].st, subpatterns[1].n);
    printf("%d, %d\n", subpatterns[2].st, subpatterns[2].n);

//     test("\\(ar\\)?thur", "arthur", 6);
//     test("\\(ar\\)+thur", "arthur", 6);
//     test("\\(ar\\)?thur", "thur", 4);
//     test("\\(ar\\)+thur", "thur", 4);

//     printf("\n%s\n", chars);
//     replace(chars, 0, strlen(chars), strlen(chars));
//     printf("%s\n", replaced);
}

void test(char *sp, char *chars, int expected) {
    int res = mark_pattern(sp, chars, 0, strlen(chars), strlen(chars));
    
    if (res != expected)
        //printf("-------------------------------------------------\n");
        printf("%2d %2d (searched pattern \"%s\" in \"%s\")\n", expected, res, sp, chars);
}


int
mark_pattern(char *sp, char *chars, int x, int n, int line_length)
{
    // return length of read pattern if found at x, of length < n, else 0

    int i; // index in chars
    int k; // index in sp
    int l; // generic index
    int s, st; // number of subpatterns, start of running subpattern
    char c;

    subpatterns[0].st = x;
    subpatterns[0].n = n;
    for (l = 1; l < 10; l++)
        subpatterns[l].n = 0;
    s = 1;

    int NONE = 0, ELEM = 1, BLOCK = 2;
    
    int min, max;
    int in_block, in_class, last_was, found_in_class, is_neg_class;
    int start_block, start_block_i, nb_block, is_block_ok; // subpatterns
    int start_elem, start_elem_i, nb_elem, is_elem_ok; // char, ., \w, \W, \d, \D, \^, \$, \\, \., (class)

    in_block = in_class = 0;
    nb_block = nb_elem = 0;
    start_block = start_elem = 0;
    start_block_i = start_elem_i = 0;
    last_was = ELEM;
    is_block_ok = is_elem_ok = 1;

    i = x;
    k = 0;
 
    for (k = 0; k < strlen(sp);) {
        // TODO: act if in_class, [, ]
        //printf("pattern: %d, %c; string: %d, %c; is_elem_ok: %d.\n", k, sp[k], i, chars[x+i], is_elem_ok);
        
        if (!in_block && !is_block_ok) {
            return 0;
        } else if (sp[k] == '*' || sp[k] == '+' || sp[k] == '?' || sp[k] == '{') {
            if (sp[k] == '{') {
                // does not check correct syntax
                min = max = 0;
                k++;
                while ((c = sp[k++]) != '}' && c != ',')
                    min = 10*min + c - '0';
                if (c == ',') {
                    while ((c = sp[k++]) != '}' && c != ',')
                        max = 10*max + c - '0';
                } else {
                    max = min;
                }
            } else {
                min = (sp[k] == '+') ? 1 : 0;
                max = (sp[k] == '?') ? 1 : 0;
                k++;
            }
            if (last_was == NONE) {
                return 0; // error
            } else if (last_was == ELEM) {
                if (!is_elem_ok) {
                    if (nb_elem < min)
                        is_block_ok = 0;
                    nb_elem = 0;
                    is_elem_ok = 1;
                    i = start_elem_i;
                } else if (i == x + n) {
                    if (nb_elem + 1 < min)
                        is_block_ok = 0;
                } else if (max && nb_elem + 1 == max) {
                    nb_elem = 0;
                    last_was = NONE;
                } else {
                    nb_elem++;
                    k = start_elem;
                }
            } else {
                // TODO
                if (!is_block_ok) {
                    if (nb_block < min)
                        return 0;
                    nb_block = 0;
                    is_block_ok = 1;
                    i = start_block_i;
                } else if (i == x + n) {
                } else if (max && nb_elem + 1 == max) {
                    nb_block = 0;
                    last_was = NONE;
                } else {
                    nb_block++;
                    k = start_block;
                }
            }
        } else if (sp[k] == '|') {
            // TODO store in subpattern
            if (last_was == NONE) {
                return 0; // error
            } else if (last_was == ELEM) {
                if (is_elem_ok) {
                    last_was = NONE;
                    // TODO, move k to next, 
                } else {
                    i = start_elem_i;
                    k++;
                }
            } else {
                if (is_block_ok) {
                    last_was = NONE;
                    // TODO
                } else {
                    i = start_block_i;
                    k++;
                }
            }
        } else if (k + 1 < strlen(sp) && sp[k] == '\\' && sp[k+1] == '(') {
            if (in_block)
                return 0; // error
            in_block = 1;
            is_block_ok = is_elem_ok = 1;
            last_was = NONE;
            start_block = k + 2;
            start_block_i = i;
            // TODO
            k += 2; 
        } else if (k + 1 < strlen(sp) && sp[k] == '\\' && sp[k+1] == ')') {
            if (!in_block)
                return 0; // error
            in_block = 0;
            last_was = BLOCK;
            subpatterns[s].st = start_block_i;
            subpatterns[s].n = i - start_block_i;
            s++;
            // TODO: store in subpattern
            k += 2; 
        } else if (sp[k] == '^') {
            if (i > x)
                is_block_ok = 0;
            k++;
        } else if (sp[k] == '$') {
            if (k < strlen(sp) - 1 || i < x + n)
                is_block_ok = 0;
            k++;
        } else if (i == x + n) {
            return 0; // error
        } else { // EATING ELEM
            if (!is_elem_ok)
                is_block_ok = 0;
            last_was = ELEM;
            start_elem = k;
            start_elem_i = i;
            if (k + 1 < strlen(sp) && sp[k] == '\\') {
                is_elem_ok = (sp[k+1] == 'w' && is_word_char(chars[x+i])) ||
                             (sp[k+1] == 'W' && !is_word_char(chars[x+i])) ||
                             (sp[k+1] == 'd' && is_number(chars[x+i])) ||
                             (sp[k+1] == 'D' && !is_number(chars[x+i])) ||
                             (sp[k+1] == chars[x+i]); // escaped char
                k += 2;
            } else {
                is_elem_ok = (sp[k] == '.' || sp[k] == chars[x+i]);
                k++;
            }
            i++;
        }
    }

    // TODO: check if ok

//     printf("in_block %d, in_class %d, is_block_ok %d, is_elem_ok %d\n",
//         in_block, in_class, is_block_ok, is_elem_ok);

    if (!is_block_ok || !is_elem_ok)
        return 0;

    return i - x;
}


int
replace(char *chars, int x, int n, int line_length)
{
    int i; // index in chars
    int j; // index in replaced
    int k; // index in rp
    int l; // no name index
    int f, st; // number of fields, start of running field

    // search for fields
    fields[0].st = x;
    fields[0].n = n;
    for (l = 1; l < 10; l++)
        fields[l].n = 0;
    f = 1;
    st = x;
    for (i = x; i < x + n && f < 9; i++) {
        if (chars[i] == sep) {
            fields[f].n = i - st; 
            fields[f].st = st;
            st = i + 1;
            f++;
        }
    }
    if (f < 9) {
        fields[f].n = i - st; 
        fields[f].st = st;
        f++;
    }

    // search for subpatterns
    mark_pattern(sp, chars, x, n, line_length);

    // copy before selection
    i = j = 0;
    while (i < x)
        replaced[j++] = chars[i++];
    
    // replace selection
    for (k = 0; k < strlen(rp) - 1; k++) {
        if (rp[k] == '\\' && (rp[k+1] == '\\' || rp[k+1] == '$')) {
            replaced[j++] = rp[k+1];
            k++;
        } else if (rp[k] == '$' && is_number(rp[k+1])) {
            for (l = 0; l < fields[rp[k+1] - '0'].n; l++)
                replaced[j++] = chars[fields[rp[k+1] - '0'].st + l];
            k++;
        } else if (rp[k] == '\\' && is_number(rp[k+1])) {
            for (l = 0; l < subpatterns[rp[k+1] - '0'].n; l++)
                replaced[j++] = chars[subpatterns[rp[k+1] - '0'].st + l];
            k++;
        } else {
            replaced[j++] = rp[k];
        }
    }
    if (k < strlen(rp))
        replaced[j++] = rp[k];

    // copy after selection
    i = x + n;
    while (i < line_length)
        replaced[j++] = chars[i++];

    return j;
}


// IGNORED
//     \s 	Find a whitespace character
//     \S 	Find a non-whitespace character
//     \b 	Find a match at the beginning/end of a word, beginning like this: \bHI, end like this: HI\b
//     \B 	Find a match, but not at the beginning/end of a word
//     \0 	Find a NULL character
//     \n 	Find a new line character
//     \f 	Find a form feed character
//     \r 	Find a carriage return character
//     \t 	Find a tab character
//     \v 	Find a vertical tab character
//     \xxx 	Find the character specified by an octal number xxx
//     \xdd 	Find the character specified by a hexadecimal number dd
//     \udddd 	Find the Unicode character specified by a hexadecimal number dddd

int
is_blank(char c)
{
    return (c == ' ');
}

int
is_word_char(char c)
{
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || (c == '_');
}

int
is_number(char c)
{
    return ('0' <= c && c <= '9');
}

//             } else if (sp[k] == '^' || sp[k] == '$' || sp[k] == '.' || sp[k] == '\\'
//                     || sp[k] == '[' || sp[k] == '*' || sp[k] == '+' || sp[k] == '?'
//                     || sp[k] == '|' || sp[k] == '{') {
