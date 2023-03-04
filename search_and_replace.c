int
compare_chars(const char *s1, int i1, const char *s2, int i2)
{
    // compare characters from s1 and s2 strings starting at i1 and i2 indexes
    // return 0 if characters are equal
    // return a positive (resp. negative) integer if character from s1 is lower
    // (resp. greater) than character from s2 (as of unicode codepoint)

    int k, l1, l2, delta;

    if ((l1 = utf8_char_length(s1[i1])) != (l2 = utf8_char_length(s2[i2]))) {
        return l2 - l1;
    } else {
        for (k = 0; k < l1; k++) {
            if (delta = (s2[i2+k] - s1[i1+k])) {
                if (!settings.case_sensitive && k == l1 - 1 &&
                    (delta == (1 << 5) || delta == - (1 << 5)))
                    return 0;
                else
                    return delta;
            }
        }

        return 0;
    }
}

void
decrement(const char *chars, int *i, int *k, int goal)
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
mark_pattern(const char *chars, int x, int n)
{
    // try to read searched pattern in chars, store identified subpatterns
    // return length of read pattern if found at x, of length < n, else 0

    char *sp; // search pattern
    int in_block, in_class, last_was, found_in_class, is_neg_class; // booleans
    int i, k; // indexes in chars (characters, bytes)
    int j, l; // indexes in sp (characters, bytes)
    int s, st; // number of subpatterns, start of running subpattern
    int start_block, start_block_i, start_block_k, nb_block, is_block_ok; // block management
    int start_elem, start_elem_i, nb_elem, is_elem_ok; // character management
    // can be: char, ., \w, \W, \d, \D, \^, \$, \\, \., class
    int min, max, lsp, a; // sp length, repetition boundaries, generic
    enum markers { NONE, ELEM, BLOCK }; // markers
    char c; // generic

    // init subpatterns
    for (k = i = 0; i < x; i++)
        k += utf8_char_length(chars[k]);
    for (a = k; i < x + n; i++)
        a += utf8_char_length(chars[a]);
    subpatterns[0].st = x;
    subpatterns[0].mst = k;
    subpatterns[0].n = n;
    subpatterns[0].mn = a - k;
    for (a = 1; a < 10; a++) {
        subpatterns[a].st = subpatterns[a].mst = 0;
        subpatterns[a].n = subpatterns[a].mn = 0;
    }

    sp = search_pattern.current;
    lsp = strlen(sp);

    in_block = in_class = 0;
    nb_block = nb_elem = 0;
    start_block = start_elem = 0;
    start_block_k = start_block_i = start_elem_i = 0;
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
                    (compare_chars(sp, l, chars, k) >= 0 &&
                     compare_chars(sp, l+a+1, chars, k) <= 0);
                j++; l += a; // lower bound
                j++; l++; // "-" separator
                j++; l += utf8_char_length(sp[l]); // upper bound
            } else {
                // try ||= ?
                found_in_class = found_in_class ||
                    (compare_chars(sp, l, chars, k) == 0);
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
                    subpatterns[s-1].mn = start_block_k - subpatterns[s-1].mst;
                    decrement(chars, &i, &k, start_block_i);
                    is_block_ok = 1;
                    last_was = NONE;
                } else if (i == x + n) {
                    if (nb_block < min)
                        return 0;
                } else if (max && nb_block == max) {
                    // get out
                    subpatterns[s-1].n = i - subpatterns[s-1].st;
                    subpatterns[s-1].mn = k - subpatterns[s-1].mst;
                    start_block_i = i;
                    start_block_k = k;
                    last_was = NONE;
                } else {
                    // another read
                    s--;
                    start_block_i = i;
                    start_block_k = k;
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
            start_block_k = k;
            j += 2; l += 2;
        } else if (l+1 < lsp && sp[l] == '\\' && sp[l+1] == ')') {
            if (!in_block)
                return 0; // error
            in_block = 0;
            last_was = BLOCK;
            is_elem_ok = 1;
            if (nb_block == 0) {
                subpatterns[s].st = start_block_i;
                subpatterns[s].mst = start_block_k;
            }
            subpatterns[s].n = (is_block_ok) ? (i - start_block_i) : 0;
            subpatterns[s].mn = (is_block_ok) ? (k - start_block_k) : 0;
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
                is_elem_ok = (sp[l] == 'w' && is_word_char(chars[k])) ||
                             (sp[l] == 'W' && !is_word_char(chars[k])) ||
                             (sp[l] == 'd' && isdigit(chars[k])) ||
                             (sp[l] == 'D' && !isdigit(chars[k])) ||
                             (compare_chars(sp, l, chars, k) == 0); // escaped char
                j++; l += utf8_char_length(sp[l]);
            } else {
                is_elem_ok = (sp[l] == '.') ||
                    (compare_chars(sp, l, chars, k) == 0);
                j++; l += utf8_char_length(sp[l]);
            }
            i++; k += utf8_char_length(chars[k]);
        }
    }

    if (in_block || in_class || !is_block_ok || !is_elem_ok)
        return 0;

    return i - x;
}

int
mark_fields(const char *chars, int x, int n)
{
    // search for fields
    // return number of fields

    int f; // index of running selection
    int i, k; // index in chars (characters, bytes)
    int st, mst; // start of running selection (characters, bytes)
    int a; // generic

    // init fields and variables
    for (k = i = 0; i < x; i++)
        k += utf8_char_length(chars[k]);
    for (a = k; i < x + n; i++)
        a += utf8_char_length(chars[a]);
    fields[0].st = st = i = x;
    fields[0].mst = mst = k;
    fields[0].n = n;
    fields[0].mn = a - k;
    for (a = 1; a < 10; a++) {
        fields[a].st = fields[a].mst = 0;
        fields[a].n = fields[a].mn = 0;
    }

    // read chars
    f = 1;
    while (i < x + n && f < 10) {
        if ((chars[k] == settings.field_separator) &&
            (k == 0 || chars[k-1] != '\\')) {
            fields[f].st = st;
            fields[f].mst = mst;
            fields[f].n = i - st;
            fields[f].mn = k - mst;
            st = i + 1;
            mst = k + 1;
            f++;
        }
        i++; k += utf8_char_length(chars[k]);
    }
    if (f < 10) {
        fields[f].st = st;
        fields[f].mst = mst;
        fields[f].n = i - st;
        fields[f].mn = k - mst;
        f++;
    }

    return f - 1;
}
