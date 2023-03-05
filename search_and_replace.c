static int
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

int
is_word_boundary(const char *chars, int k)
{
    // detect if k is the memory index of a word boundary in chars

    int i, is_word;

    i = 1;
    is_word = is_word_char(chars[k]);

    if (k == 0) {
        return is_word;
    } else {
        decrement(chars, &i, &k, 0);
        return (is_word != is_word_char(chars[k]));
    }
}

int
mark_subpatterns(const char *chars, int dl, int ss, int x, int n)
{
    // try to read searched pattern in chars, store identified subpatterns
    // dl must be the visual length of chars, and ss the real selection start
    // return length of read pattern if found at x, of length < n, else 0

    enum markers { NONE, ELEM, BLOCK }; // remember last read element type
    int in_block, in_class, last_was, found_in_class, is_neg_class;
    int nb_block, is_block_ok, nb_elem, is_elem_ok;
    int s, st;          // number of subpatterns, start of running subpattern
    int min, max, a;    // repetition boundaries, generic
    char c;             // generic

    char *sp;           // search pattern
    int lsp, j, l;      // memory length, indexes (characters, bytes) of sp
    int start_block_j, start_elem_j;

    int i, k;           // indexes (characters, bytes) of chars
    int start_block_i, start_block_k, start_elem_i;

    // init variables
    in_block = in_class = 0;
    last_was = NONE;
    nb_block = nb_elem = 0;
    is_block_ok = is_elem_ok = 1;
    s = 1;

    // init indexes
    lsp = strlen(sp = search_pattern.current);
    j = l = 0;
    k = get_str_index(chars, i = x);

    // init subpatterns
    subpatterns[0].st = i;
    subpatterns[0].mst = k;
    subpatterns[0].n = n;
    subpatterns[0].mn = get_str_index(&(chars[k]), n) - k;
    for (a = 1; a < 10; a++) {
        subpatterns[a].st = subpatterns[a].mst = 0;
        subpatterns[a].n = subpatterns[a].mn = 0;
    }

    // try to read whole pattern
    while (l < lsp) {
        if (!in_block && !is_block_ok && last_was != BLOCK) {
            return 0; // pattern does not match

        // CUSTOM CLASS
        } else if (in_class) {
            a = utf8_char_length(c);
            if (c == ']') { // move on
                in_class = 0;
                is_elem_ok = (is_neg_class) ? (!found_in_class) : (found_in_class);
                i++; k += utf8_char_length(chars[k]);
                j++; l++;
            } else if (l+a+1 < lsp && sp[l+a] == '-' && sp[l+a+1] != ']') { // range
                found_in_class |= (compare_chars(sp, l, chars, k) >= 0 &&
                     compare_chars(sp, l+a+1, chars, k) <= 0); // XXX |= working ?
                j++; l += a;                        // lower bound
                j++; l++;                           // "-" separator
                j++; l += utf8_char_length(sp[l]);  // upper bound
            } else { // raw comparison
                found_in_class |= (compare_chars(sp, l, chars, k) == 0); // XXX |= working ?
                j++; l += a;
            }

        // ASSERTIONS
        } else if (((c = sp[l]) == '^') || (c == '\\' && sp[l+1] == 'A')) {
            if (i > ((c == '^') ? 0 : ss))
                is_block_ok = 0;
            j += (c == '\\') ? 2 : 1;
            l += (c == '\\') ? 2 : 1;
        } else if ((c == '$') || (c == '\\' && sp[l+1] == 'Z')) {
            if (l+1 < lsp || i < ((c == '$') ? dl : (x+n)))
                is_block_ok = 0;
            j += (c == '\\') ? 2 : 1;
            l += (c == '\\') ? 2 : 1;
        } else if (c == '\\' && (sp[l+1] == 'b' || sp[l+1] == 'B')) {
            a = is_word_boundary(chars, k);
            if ((sp[l+1] == 'b' && !a) || (sp[l+1] == 'B' && a))
                is_block_ok = 0;
            j += 2; l += 2;

        // REPEATERS
        } else if (c == '*' || c == '+' || c == '?' || c == '{') {
            // computing min and max
            if (c == '{') {
                min = max = 0;
                j++; l++;
                for (; (c = sp[l]) != '}' && c != ','; j++, l++) {
                    if (l+1 == lsp || !isdigit(c))
                        return 0; // error: bad repeater syntax
                    min = 10*min + c - '0';
                }
                if (c == ',') {
                    for (; (c = sp[l]) != '}'; j++, l++) {
                        if (l+1 == lsp || !isdigit(c))
                            return 0; // error: bad repeater syntax
                        max = 10*max + c - '0';
                    }
                } else {
                    max = min;
                }
            } else {
                min = (c == '+') ? 1 : 0;
                max = (c == '?') ? 1 : 0;
                j++; l++;
            }
            // compare with number of read elements or blocks
            if (last_was == NONE) {
                return 0; // error: repeater must act on element or block
            } else if (last_was == ELEM) {
                if (!is_elem_ok) { // cancelling read
                    if (nb_elem < min)
                        is_block_ok = 0;
                    nb_elem = 0;
                    is_elem_ok = 1;
                    decrement(chars, &i, &k, start_elem_i);
                } else if (i == x + n) {
                    if (nb_elem + 1 < min)
                        is_block_ok = 0;
                } else if (max && nb_elem + 1 == max) { // move on
                    nb_elem = 0;
                    last_was = NONE;
                } else { // another read
                    nb_elem++;
                    decrement(sp, &j, &l, start_elem_j);
                }
            } else {
                if (!is_block_ok) { // cancelling read
                    if (nb_block - 1 < min)
                        return 0; // no match: not enough blocks in selection
                    subpatterns[s-1].n = start_block_i - subpatterns[s-1].st;
                    subpatterns[s-1].mn = start_block_k - subpatterns[s-1].mst;
                    decrement(chars, &i, &k, start_block_i);
                    is_block_ok = 1;
                    last_was = NONE;
                } else if (i == x + n) {
                    if (nb_block < min)
                        return 0; // no match: not enough blocks in selection
                } else if (max && nb_block == max) { // move on
                    subpatterns[s-1].n = i - subpatterns[s-1].st;
                    subpatterns[s-1].mn = k - subpatterns[s-1].mst;
                    start_block_i = i;
                    start_block_k = k;
                    last_was = NONE;
                } else { // another read
                    s--;
                    start_block_i = i;
                    start_block_k = k;
                    in_block = 1;
                    last_was = NONE;
                    decrement(sp, &j, &l, start_block_j);
                }
            }

        // OR
        } else if (c == '|') {
            if (last_was == NONE) {
                return 0; // error: | must follow a non-repeated block/element
            } else if ((last_was == ELEM && is_elem_ok) ||
                (last_was == BLOCK && is_block_ok)) {
                last_was = NONE;
                // move j and l to next location
                while (sp[l] == '|') {
                    j++; l++;
                    if (l == lsp || sp[l] == '|') { // empty pattern
                        return 0; // error: patterns must be non-null
                    } else if (sp[l] == '\\' && sp[l+1] == '(') { // block
                        while (!(sp[l] == '\\' && sp[l+1] == ')')) {
                            j++; l += utf8_char_length(sp[l]);
                            if (l+1 == lsp)
                                return 0; // error: block not ended
                        }
                        j += 2; l += 2;
                        s++;
                    } else if (sp[l] == '[') { // class
                        while (sp[l] != ']') {
                            j++; l += utf8_char_length(sp[l]);
                            if (l == lsp)
                                return 0; // error: class not ended
                        }
                        j++; l++;
                    } else if (sp[l] == '\\') { // special
                        j++; l++;
                        j++; l += utf8_char_length(sp[l]);
                    } else { // normal character
                        j++; l += utf8_char_length(sp[l]);
                    }
                }
            } else { // try the next pattern
                decrement(chars, &i, &k,
                    (last_was == ELEM) ? start_elem_i : start_block_i);
                last_was = NONE;
                j++; l++;
                is_block_ok = is_elem_ok = 1;
            }

        // CUSTOM CLASS START
        } else if (c == '[') {
            if (i == x + n)
                return 0; // no match: early end of selection
            if (!is_elem_ok)
                is_block_ok = 0;
            start_elem_j = j;
            start_elem_i = i;
            last_was = ELEM;
            in_class = 1;
            found_in_class = is_neg_class = 0;
            if (sp[l+1] == '^') {
                j++; l++;
                is_neg_class = 1;
            }
            j++; l++;

        // BLOCK START
        } else if (c == '\\' && sp[l+1] == '(') {
            if (in_block)
                return 0; // error: blocks can not be nested
            in_block = 1;
            is_block_ok = is_elem_ok = 1;
            last_was = NONE;
            nb_block = 0;
            subpatterns[s].st = start_block_i = i;
            subpatterns[s].mst = start_block_k = k;
            start_block_j = j + 2;
            j += 2; l += 2;

        // BLOCK END
        } else if (c == '\\' && sp[l+1] == ')') {
            if (!in_block)
                return 0; // error: block end without start
            in_block = 0;
            last_was = BLOCK;
            is_elem_ok = 1;
            subpatterns[s].n = (is_block_ok) ? (i - start_block_i) : 0;
            subpatterns[s].mn = (is_block_ok) ? (k - start_block_k) : 0;
            nb_block++;
            s++;
            j += 2; l += 2;

        // CHARACTER
        } else {
            if (i == x + n)
                return 0; // no match: early end of selection
            if (!is_elem_ok)
                is_block_ok = 0;
            last_was = ELEM;
            start_elem_j = j;
            start_elem_i = i;
            if (c == '\\') { // known classes and escaped characters
                j++; l++;
                c = sp[l];
                is_elem_ok = (c == 'w' && is_word_char(chars[k])) ||
                             (c == 'W' && !is_word_char(chars[k])) ||
                             (c == 'd' && isdigit(chars[k])) ||
                             (c == 'D' && !isdigit(chars[k])) ||
                             (strchr("\\^$|*+?{[.", c) != NULL &&
                              compare_chars(sp, l, chars, k) == 0);
            } else { // normal characters
                is_elem_ok = (sp[l] == '.') ||
                    (compare_chars(sp, l, chars, k) == 0);
            }
            j++; l += utf8_char_length(sp[l]);
            i++; k += utf8_char_length(chars[k]);
        }
    }

    if (in_block || in_class || !is_block_ok || !is_elem_ok)
        return 0; // error: block or class started but not ended, or no match

    return i - x; // match: return read length
}

int
mark_fields(const char *chars, int x, int n)
{
    // search for fields
    // return number of fields

    int f, a;           // number of fields, generic
    int st, mst;        // indexes (characters, bytes) of running field start
    int i, k;           // indexes (characters, bytes) of chars

    // init fields and indexes
    k = get_str_index(chars, i = x);
    fields[0].st = st = i;
    fields[0].mst = mst = k;
    fields[0].n = n;
    fields[0].mn = get_str_index(&(chars[k]), n) - k;
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
