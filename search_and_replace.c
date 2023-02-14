int
compare_chars(char *s1, int i1, char *s2, int i2)
{
    // compare characters from s1 and s2 strings starting at i1 and i2 indexes
    // return 0 if characters are equal
    // return a positive (resp. negative) integer if character from s1 is lower
    // (resp. greater) than character from s2 (as of unicode codepoint)

    int k, l1, l2;

    if ((l1 = utf8_char_length(s1[i1])) != (l2 = utf8_char_length(s2[i2]))) {
        return l2 - l1;
    } else {
        for (k = 0; k < l1; k++) {
            if (s1[i1+k] != s2[i2+k])
                return s2[i2+k] - s1[i1+k];
        }

        return 0;
    }
}

void
decrement(char *chars, int *i, int *k, int goal)
{
    // assuming *k is an index in chars corresponding to the (*i)-th character,
    // decrement *i to goal (and *k accordingly)

    while (*i > goal) {
        (*i)--;
        (*k)--;
        while ((chars[*k] & 0xc0) == 0x80)
            (*k)--;
    }
}

int
mark_pattern(char *chars, int x, int n)
{
    // try to read searched pattern in chars, store identified subpatterns
    // return length of read pattern if found at x, of length < n, else 0

    char *sp; // search pattern
    int in_block, in_class, last_was, found_in_class, is_neg_class; // booleans
    int i, k; // indexes in chars (characters, bytes)
    int j, l; // indexes in sp (characters, bytes)
    int s, st; // number of subpatterns, start of running subpattern
    int start_block, start_block_i, nb_block, is_block_ok; // block management
    int start_elem, start_elem_i, nb_elem, is_elem_ok; // character management
    // can be: char, ., \w, \W, \d, \D, \^, \$, \\, \., class
    int min, max, lsp, a; // sp length, repetition boundaries, generic
    int NONE = 0, ELEM = 1, BLOCK = 2; // markers
    char c; // generic

    // init subpatterns
    subpatterns[0].st = x;
    subpatterns[0].n = n;
    for (a = 1; a < 10; a++)
        subpatterns[a].n = 0;

    sp = search_pattern.current;
    lsp = strlen(sp);

    in_block = in_class = 0;
    nb_block = nb_elem = 0;
    start_block = start_elem = 0;
    start_block_i = start_elem_i = 0;
    last_was = NONE;
    is_block_ok = is_elem_ok = 1;
    s = 1;

    // init indexes
    for (k = i = 0; i < x; i++)
        k += utf8_char_length(chars[k]);
 
    // try to read whole pattern
    for (j = l = 0; l < lsp;) {
        if (last_was != BLOCK && !in_block && !is_block_ok) {
            return 0; // error
        } else if (in_class) {
            // no escapes in classes
            a = utf8_char_length(sp[l]);
            if (sp[l] == ']') {
                in_class = 0;
                is_elem_ok = (is_neg_class) ? (!found_in_class) : (found_in_class);
                i++; k += utf8_char_length(chars[k]);
                j++; l++;
            } else if (l+a+1 < lsp && sp[l+a] == '-' && sp[l+a+1] != ']') {
                // range
                // try ||= ?
                found_in_class = found_in_class ||
                    (compare_chars(sp, l, chars, x+i) >= 0 &&
                     compare_chars(sp, l+a+1, chars, x+i) <= 0);
                j++; l += a; // lower bound
                j++; l++; // "-" separator
                j++; l += utf8_char_length(sp[l]); // upper bound
            } else {
                // try ||= ?
                found_in_class = found_in_class ||
                    (compare_chars(sp, l, chars, x+i) == 0);
                j++; l += utf8_char_length(sp[l]);
            }
        } else if (sp[l] == '[') {
            if (i == x + n)
                return 0;
            if (!is_elem_ok)
                is_block_ok = 0;
            start_elem = j;
            start_elem_i = i;
            last_was = ELEM;
            in_class = 1;
            found_in_class = is_neg_class = 0;
            if (l+1 < lsp && sp[l+1] == '^') {
                j++; l++;
                is_neg_class = 1;
            }
            j++; l++;
        } else if (sp[l] == '*' || sp[l] == '+' || sp[l] == '?' || sp[l] == '{') {
            // computing min and max
            if (sp[l] == '{') {
                min = max = 0;
                j++; l++;
                while ((c = sp[l]) != '}' && c != ',')
                    if (l+1 == lsp || c < '0' || c > '9') {
                        return 0; // error
                    } else {
                        min = 10*min + c - '0';
                        j++; l++;
                    }
                if (c == ',') {
                    while ((c = sp[l]) != '}')
                        if (l+1 == lsp || c < '0' || c > '9') {
                            return 0; // error
                        } else {
                            max = 10*max + c - '0';
                            j++; l++;
                        }
                } else {
                    max = min;
                }
            } else {
                min = (sp[l] == '+') ? 1 : 0;
                max = (sp[l] == '?') ? 1 : 0;
                j++; l++;
            }
            if (last_was == NONE) {
                return 0; // error
            } else if (last_was == ELEM) {
                if (!is_elem_ok) {
                    if (nb_elem < min)
                        is_block_ok = 0;
                    nb_elem = 0;
                    is_elem_ok = 1;
                    decrement(chars, &i, &k, start_elem_i);
                } else if (i == x + n) {
                    if (nb_elem + 1 < min)
                        is_block_ok = 0;
                } else if (max && nb_elem + 1 == max) {
                    nb_elem = 0;
                    last_was = NONE;
                } else {
                    nb_elem++;
                    decrement(sp, &j, &l, start_elem);
                }
            } else {
                if (!is_block_ok) {
                    // cancelling read
                    if (nb_block - 1 < min)
                        return 0;
                    subpatterns[s-1].n = start_block_i - subpatterns[s-1].st;
                    decrement(chars, &i, &k, start_block_i);
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
                    decrement(sp, &j, &l, start_block);
                }
            }
        } else if (sp[l] == '|') {
            if (last_was == NONE) {
                return 0; // error
            } else if ((last_was == ELEM && is_elem_ok) ||
                (last_was == BLOCK && is_block_ok)) {
                last_was = NONE;
                // move j and l to next location
                while (l < lsp && sp[l] == '|') { // l < lsp needed ?
                    j++; l++;
                    if (l == lsp) {
                        return 0; // error
                    } else if (l+1 < lsp && sp[l] == '\\' && sp[l+1] == '(') {
                        // block
                        while (!(sp[l] == '\\' && sp[l+1] == ')')) {
                            j++; l += utf8_char_length(sp[l]);
                            if (l+1 == lsp)
                                return 0; // error
                        }
                        j += 2; l += 2;
                        s++;
                    } else if (sp[l] == '[') {
                        // class
                        while (sp[l] != ']') {
                            j++; l += utf8_char_length(sp[l]);
                            if (l == lsp)
                                return 0; // error
                        }
                        j++; l++;
                    } else if (sp[l] == '\\') {
                        // escaped character
                        j++; l++;
                        j++; l += utf8_char_length(sp[l]);
                    } else {
                        // normal character
                        j++; l += utf8_char_length(sp[l]);
                    }
                }
            } else {
                decrement(chars, &i, &k,
                    (last_was == ELEM) ? start_elem_i : start_block_i);
                last_was = NONE;
                j++; l++;
                is_block_ok = is_elem_ok = 1;
            }
        } else if (l+1 < lsp && sp[l] == '\\' && sp[l+1] == '(') {
            if (in_block)
                return 0; // error
            in_block = 1;
            is_block_ok = is_elem_ok = 1;
            last_was = NONE;
            nb_block = 0;
            start_block = j + 2;
            start_block_i = i;
            j += 2; l += 2;
        } else if (l+1 < lsp && sp[l] == '\\' && sp[l+1] == ')') {
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
            j += 2; l += 2;
        } else if (sp[l] == '^') {
            if (i > x)
                is_block_ok = 0;
            j++; l++;
        } else if (sp[l] == '$') {
            if (l+1 < lsp || i < x + n)
                is_block_ok = 0;
            j++; l++;
        } else if (i == x + n) {
            return 0; // error
        } else { // EATING ELEM
            if (!is_elem_ok)
                is_block_ok = 0;
            last_was = ELEM;
            start_elem = j;
            start_elem_i = i;
            if (l+1 < lsp && sp[l] == '\\') {
                j++; l++;
                is_elem_ok = (sp[l] == 'w' && is_word_char(chars[x+i])) ||
                             (sp[l] == 'W' && !is_word_char(chars[x+i])) ||
                             (sp[l] == 'd' && is_digit(chars[x+i])) ||
                             (sp[l] == 'D' && !is_digit(chars[x+i])) ||
                             (compare_chars(sp, l, chars, x+i) == 0); // escaped char
                j++; l += utf8_char_length(sp[l]);
            } else {
                is_elem_ok = (sp[l] == '.') ||
                    (compare_chars(sp, l, chars, x+i) == 0);
                j++; l += utf8_char_length(sp[l]);
            }
            i++; k += utf8_char_length(chars[k]);
        }
    }

    if (in_block || in_class || !is_block_ok || !is_elem_ok)
        return 0;

    return i - x;
}

char replaced[100]; // TODO: manage size interactively

int
replace_with_pattern(char *chars, int x, int n)
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
    mark_pattern(chars, x, n);

    // copy before selection
    i = j = 0;
    while (i < x)
        replaced[j++] = chars[i++];
    
    // replace selection
    for (k = 0; k < lrp - 1; k++) {
        if (rp[k] == '\\' && (rp[k+1] == '\\' || rp[k+1] == '$')) {
            replaced[j++] = rp[k+1];
            k++;
        } else if (rp[k] == '$' && is_digit(rp[k+1])) {
            for (l = 0; l < fields[rp[k+1] - '0'].n; l++)
                replaced[j++] = chars[fields[rp[k+1] - '0'].st + l];
            k++;
        } else if (rp[k] == '\\' && is_digit(rp[k+1])) {
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
    //while (i < line_length)
    //    replaced[j++] = chars[i++];

    return j;
}
