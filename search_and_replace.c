int mark_pattern(char *chars, int x, int n, int line_length);
int _replace(char *chars, int x, int n, int line_length);

char replaced[100]; // TODO: manage size interactively

int
mark_pattern(char *chars, int x, int n, int line_length)
{
    // return length of read pattern if found at x, of length < n, else 0

    char *sp; // search pattern
    int lsp; // length of search pattern

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

    // TODO: move to globals.h
    int NONE = 0, ELEM = 1, BLOCK = 2;
    
    int min, max;
    int in_block, in_class, last_was, found_in_class, is_neg_class;
    int start_block, start_block_i, nb_block, is_block_ok; // subpatterns
    int start_elem, start_elem_i, nb_elem, is_elem_ok; // char, ., \w, \W, \d, \D, \^, \$, \\, \., class

    sp = search_pattern.current;
    lsp = strlen(sp);

    in_block = in_class = 0;
    nb_block = nb_elem = 0;
    start_block = start_elem = 0;
    start_block_i = start_elem_i = 0;
    last_was = NONE;
    is_block_ok = is_elem_ok = 1;

    i = x;
    k = 0;
 
    for (k = 0; k < lsp;) {
        if (last_was != BLOCK && !in_block && !is_block_ok) {
            return 0;
        } else if (in_class) {
            // no escapes in classes
            if (sp[k] == ']') {
                in_class = 0;
                is_elem_ok = (is_neg_class) ? (!found_in_class) : (found_in_class);
                i++;
                k++;
            } else if (k+2 < lsp && sp[k+1] == '-' && sp[k+2] != ']') {
                found_in_class = found_in_class ||
                    (sp[k] <= chars[x+i]) && (chars[x+i] <= sp[k+2]);
                k += 3;
            } else {
                found_in_class = found_in_class || (sp[k] == chars[x+i]);
                k++;
            }
        } else if (sp[k] == '[') {
            if (i == x + n)
                return 0;
            if (!is_elem_ok)
                is_block_ok = 0;
            start_elem = k;
            start_elem_i = i;
            last_was = ELEM;
            in_class = 1;
            found_in_class = is_neg_class = 0;
            if (k+1 < lsp && sp[k+1] == '^') {
                k++;
                is_neg_class = 1;
            }
            k++;
        } else if (sp[k] == '*' || sp[k] == '+' || sp[k] == '?' || sp[k] == '{') {
            // computing min and max
            if (sp[k] == '{') {
                min = max = 0;
                k++;
                while ((c = sp[k++]) != '}' && c != ',')
                    if (k == lsp || c < '0' || c > '9') {
                        return 0; // error
                    } else {
                        min = 10*min + c - '0';
                    }
                if (c == ',') {
                    while ((c = sp[k++]) != '}' && c != ',')
                        if (k == lsp || c < '0' || c > '9') {
                            return 0; // error
                        } else {
                            max = 10*max + c - '0';
                        }
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
                if (!is_block_ok) {
                    // cancelling read
                    if (nb_block - 1 < min)
                        return 0;
                    subpatterns[s-1].n = start_block_i - subpatterns[s-1].st;
                    i = start_block_i;
                    is_block_ok = 1;
                    last_was = NONE;
                } else if (i == x + n) {
                    if (nb_block < min)
                        return 0;
                } else if (max && nb_block == max) {
                    // get out
                    subpatterns[s-1].n = i - subpatterns[s-1].st;
                    start_block_i = i;
                    last_was = NONE;
                } else {
                    // another read
                    s--;
                    start_block_i = i;
                    in_block = 1;
                    last_was = NONE;
                    k = start_block;
                }
            }
        } else if (sp[k] == '|') {
            if (last_was == NONE) {
                return 0; // error
            } else if ((last_was == ELEM && is_elem_ok) ||
                (last_was == BLOCK && is_block_ok)) {
                last_was = NONE;
                // move k to next location
                while (k < lsp && sp[k] == '|') {
                    k++;
                    if (k == lsp) {
                        return 0; // error
                    } else if (k+1 < lsp && sp[k] == '\\' && sp[k+1] == '(') {
                        while (k+1 < lsp && !(sp[k] == '\\' && sp[k+1] == ')'))
                            k++;
                        k += 2;
                        s++;
                    } else if (sp[k] == '[') {
                        while (k < lsp && sp[k] != ']')
                            k++;
                        k++;
                    } else if (sp[k] == '\\') {
                        k += 2;
                    } else {
                        k++;
                    }
                }
            } else {
                i = (last_was == ELEM) ? start_elem_i : start_block_i;
                last_was = NONE;
                k++;
                is_block_ok = is_elem_ok = 1;
            }
        } else if (k + 1 < lsp && sp[k] == '\\' && sp[k+1] == '(') {
            if (in_block)
                return 0; // error
            in_block = 1;
            is_block_ok = is_elem_ok = 1;
            last_was = NONE;
            nb_block = 0;
            start_block = k + 2;
            start_block_i = i;
            k += 2; 
        } else if (k + 1 < lsp && sp[k] == '\\' && sp[k+1] == ')') {
            if (!in_block)
                return 0; // error
            in_block = 0;
            last_was = BLOCK;
            is_elem_ok = 1;
            if (nb_block == 0)
                subpatterns[s].st = start_block_i;
            subpatterns[s].n = (is_block_ok) ? (i - start_block_i) : 0;
            nb_block++;
            s++;
            k += 2; 
        } else if (sp[k] == '^') {
            if (i > x)
                is_block_ok = 0;
            k++;
        } else if (sp[k] == '$') {
            if (k < lsp - 1 || i < x + n)
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
            if (k + 1 < lsp && sp[k] == '\\') {
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

    if (in_block || in_class || !is_block_ok || !is_elem_ok)
        return 0;

    return i - x;
}


int
_replace(char *chars, int x, int n, int line_length)
{
    char *rp; // search pattern
    int lrp; // length of search pattern

    int i; // index in chars
    int j; // index in replaced
    int k; // index in rp
    int l; // no name index
    int f, st; // number of fields, start of running field
    
    rp = replace_pattern.current;
    lrp = strlen(rp);

    // search for fields
    fields[0].st = x;
    fields[0].n = n;
    for (l = 1; l < 10; l++)
        fields[l].n = 0;
    f = 1;
    st = x;
    for (i = x; i < x + n && f < 9; i++) {
        if (chars[i] == settings.field_separator) {
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
    mark_pattern(chars, x, n, line_length);

    // copy before selection
    i = j = 0;
    while (i < x)
        replaced[j++] = chars[i++];
    
    // replace selection
    for (k = 0; k < lrp - 1; k++) {
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
    if (k < lrp)
        replaced[j++] = rp[k];

    // copy after selection
    i = x + n;
    while (i < line_length)
        replaced[j++] = chars[i++];

    return j;
}
