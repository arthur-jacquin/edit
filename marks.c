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

static int
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

static int
parse_rep(const char *sp, int *j, int *l, int *min, int *max)
{
    // compute min and max
    // return 1 on success

    char c;

    if ((c = sp[*l]) == '{') {
        *min = *max = 0;
        for ((*j)++, (*l)++; (c = sp[*l]) != '}' && c != ','; (*j)++, (*l)++) {
            if (!isdigit(c))
                return 0; // invalid syntax
            *min = (*min)*10 + c - '0';
        }
        if (c == ',') {
            for ((*j)++, (*l)++; (c = sp[*l]) != '}'; (*j)++, (*l)++) {
                if (!isdigit(c))
                    return 0; // invalid syntax
                *max = (*max)*10 + c - '0';
            }
        } else {
            *max = *min;
        }
        (*j)++; (*l)++;
    } else if (c == '*' || c == '+' || c == '?') {
        *min = (c == '+') ? 1 : 0;
        *max = (c == '?') ? 1 : 0;
        (*j)++; (*l)++;
    } else {
        *min = *max = 1;
    }

    return 1;
}

static int
eat_pattern_atom(const char *sp, int *j, int *l)
{
    // move *j, *l indexes of sp after the atom pattern
    // return 1 on success

    int min, max;

    (*j)++; (*l)++;
    if (sp[*l] == '^' || sp[*l] == '$') {
        (*j)++; (*l)++;
    } else if (sp[*l] == '\\' && strchr("AZbB", sp[*l+1])) {
        (*j) += 2; (*l) += 2;
    } else {
        if (sp[*l] == '\\' && strchr("\\^$|*+?{[.dDwW", sp[*l+1])) {
            (*j) += 2; (*l) += 2;
        } else if (sp[*l] == '[') {
            while (sp[*l] != '\0' && sp[*l] != ']') {
                (*j)++; (*l) += utf8_char_length(sp[*l]);
            }
            if (sp[*l] == '\0')
                return 0;
            (*j)++; (*l)++;
        } else {
            (*j)++; (*l) += utf8_char_length(sp[*l]);
        }
        if (!parse_rep(sp, j, l, &min, &max))
            return 0;
    }

    return 1;
}

static int
eat_pattern_block(const char *sp, int *j, int *l)
{
    // move *j, *l indexes of sp after the block pattern
    // return 1 on success

    int min, max;

    if (sp[*l] == '\\' && sp[*l+1] == '(') {
        while (!(sp[*l] == '\\' && sp[*l+1] == ')')) {
            if (sp[*l] == '\0' || !eat_pattern_atom(sp, j, l))
                return 0;
            while (sp[*l] == '|') {
                (*j)++; (*l)++;
                if (sp[*l] == '\0' || !eat_pattern_atom(sp, j, l))
                    return 0;
            }
        }
        (*j) += 2; (*l) += 2;
        return parse_rep(sp, j, l, &min, &max);
    } else {
        return eat_pattern_atom(sp, j, l);
    }
}

int
mark_subpatterns(const char *chars, int dl, int ss, int sx, int n)
{
    // try to read searched pattern in chars, store identified subpatterns
    // dl must be the visual length of chars, and ss the real selection start
    // return length of read pattern if found at x, of length < n, else 0

    int s, st;          // number of subpatterns, start of running subpattern
    char *sp;           // search pattern
    int lsp, j, l;      // memory length, indexes (characters, bytes) of sp
    int i, k;           // indexes (characters, bytes) of chars

    // states are named to match cheatsheet.md wording
    enum states {READ_PATTERN, READ_BLOCK, BLOCK_READ, READ_STRING, STRING_READ,
        READ_ATOM, ATOM_READ, READ_CHARACTER, CHARACTER_READ};
    int state, in_string, found_in_class, is_neg_class, a;

    // ORed elements
    int is_block_ok, start_block_i;
    int is_atom_ok, start_atom_i;

    // repeated elements
    int is_string_ok, nb_string, start_string_j, start_string_i;
    int is_char_ok, nb_char, start_char_j, start_char_i;
    int min, max;

    // init variables
    state = READ_PATTERN;
    lsp = strlen(sp = search_pattern.current);
    l = j = 0;
    s = 1;
    
    // init subpatterns
    subpatterns[0].st = i = sx;
    subpatterns[0].mst = k = get_str_index(chars, i);
    subpatterns[0].n = n;
    subpatterns[0].mn = get_str_index(&(chars[k]), n) - k;
    for (a = 1; a < 10; a++) {
        subpatterns[a].st = subpatterns[a].mst = 0;
        subpatterns[a].n = subpatterns[a].mn = 0;
    }

    // try to read the pattern
    while (1)
    switch (state) {
    case READ_PATTERN:
        if (l == lsp)
            return i - sx;
        state = READ_BLOCK;
        is_block_ok = 1;
        start_block_i = i;
        break;

    case READ_BLOCK:
        if (l == lsp)
            return 0;
        is_block_ok = 1;
        if (sp[l] == '\\' && sp[l+1] == '(') {
            j += 2; l += 2;
            in_string = 1;
            state = READ_STRING;
            is_string_ok = 1;
            nb_string = 0;
            subpatterns[s].st = start_string_i = i;
            subpatterns[s].mst = k;
            start_string_j = j;
        } else {
            in_string = 0;
            state = READ_ATOM;
            is_atom_ok = 1;
            start_atom_i = i;
        }
        break;

    case BLOCK_READ:
        if (is_block_ok) { // eat following blocks
            while (sp[l] == '|') {
                j++; l++;
                if (sp[l] == '\0' || !eat_pattern_block(sp, &j, &l))
                    return 0; // syntax error
            }
            state = READ_PATTERN;
        } else if (sp[l] == '|') { // another try
            decrement(chars, &i, &k, start_block_i);
            j++; l++;
            state = READ_BLOCK;
            is_block_ok = 1;
        } else {
            return 0; // essential block is invalid
        }
        break;

    case READ_STRING:
        if (l == lsp) {
            in_string = 0;
            state = STRING_READ;
            is_string_ok = 0;
            nb_string++;
        } else if (sp[l] == '\\' && sp[l+1] == ')') {
            j += 2; l += 2;
            in_string = 0;
            state = STRING_READ;
            nb_string++;
        } else {
            state = READ_ATOM;
            is_atom_ok = 1;
            start_atom_i = i;
        }
        break;

    case STRING_READ:
    case CHARACTER_READ:
        if (!parse_rep(sp, &j, &l, &min, &max)) // compute min and max
            return 0; // invalid syntax
        if (state == STRING_READ) {
            if (!is_string_ok) { // cancelling read
                if (nb_string - 1 < min)
                    is_block_ok = 0;
                decrement(chars, &i, &k, start_string_i);
            } else if (!max || nb_string < max) { // another read
                state = READ_STRING;
                in_string = 1;
                start_string_i = i;
                decrement(sp, &j, &l, start_string_j);
                break;
            }
            subpatterns[s].n = i - subpatterns[s].st;
            subpatterns[s].mn = k - subpatterns[s].mst;
            s++;
            state = BLOCK_READ;
        } else {
            if (!is_char_ok) { // cancelling read
                if (nb_char - 1 < min) {
                    if (in_string)
                        is_string_ok = 0;
                    else
                        return 0; // essential atom is invalid
                }
                decrement(chars, &i, &k, start_char_i);
            } else if (!max || nb_char < max) { // another read
                state = READ_CHARACTER;
                is_char_ok = 1;
                start_char_i = i;
                decrement(sp, &j, &l, start_char_j);
                break;
            }
            state = ATOM_READ;
        }
        break;

    case READ_ATOM:
        state = ATOM_READ;
        if (l == lsp) {
            is_atom_ok = 0;
        } else if (sp[l] == '^' || sp[l] == '$') { // assertions
            is_atom_ok = (sp[l] == '^') ? (i == 0) : (i == dl);
            j++; l++;
        } else if (sp[l] == '\\' && (sp[l+1] == 'A' || sp[l+1] == 'Z')) {
            is_atom_ok = (sp[l+1] == 'A') ? (i == ss) : (i == sx + n);
            j += 2; l += 2;
        } else if (sp[l] == '\\' && (sp[l+1] == 'b' || sp[l+1] == 'B')) {
            is_atom_ok = (sp[l+1] == 'B') ^ is_word_boundary(chars, k);
            j += 2; l += 2;
        } else { // character
            state = READ_CHARACTER;
            is_char_ok = 1;
            nb_char = 0;
            start_char_j = j;
            start_char_i = i;
        }
        break;

    case ATOM_READ:
        if (in_string) {
            if (is_atom_ok) { // eat following atoms
                while (sp[l] == '|') {
                    j++; l++;
                    if (sp[l] == '\0' || !eat_pattern_atom(sp, &j, &l))
                        return 0; // invalid syntax
                }
            } else if (sp[l] == '|') { // another try
                decrement(chars, &i, &k, start_atom_i);
                j++; l++;
            } else {
                is_string_ok = 0;
            }
            state = READ_STRING;
        } else if (is_atom_ok) {
            state = BLOCK_READ;
        } else {
            return 0; // essential atom is invalid
        }
        break;

    case READ_CHARACTER:
        state = CHARACTER_READ;
        nb_char++;
        if (l == lsp) {
            return 0; //invalid syntax
        } else if (i == sx + n) {
            is_char_ok = 0;
            break;
        } if (sp[l] == '\\') {
            if (strchr("\\^$|*+?{[.", sp[l+1])) { // escaped character
                is_char_ok = (compare_chars(sp, l+1, chars, k) == 0);
            } else if (sp[l+1] == 'd' || sp[l+1] == 'D') { // [non] digit
                is_char_ok = (sp[l+1] == 'D') ^ isdigit(chars[k]);
            } else if (sp[l+1] == 'w' || sp[l+1] == 'W') { // [non] word char.
                is_char_ok = (sp[l+1] == 'W') ^ is_word_char(chars[k]);
            } else {
                return 0; // invalid syntax
            }
            j += 2; l += 2;
        } else if (sp[l] == '[') { // custom class
            found_in_class = is_neg_class = 0;
            j++; l++;
            if (sp[l] == '^') {
                j++; l++;
                is_neg_class = 1;
            }
            while (sp[l] != '\0' && sp[l] != ']') {
                a = utf8_char_length(sp[l]);
                if (l+a+1 < lsp && sp[l+a] == '-' && sp[l+a+1] != ']') { //range
                    if (compare_chars(sp, l, chars, k) >= 0 &&
                         compare_chars(sp, l+a+1, chars, k) <= 0)
                        found_in_class = 1;
                    j += 3; l += a + 1 + utf8_char_length(sp[l+a+1]);
                } else { // raw comparison
                    if (!compare_chars(sp, l, chars, k))
                        found_in_class = 1;
                    j++; l += a;
                }
            }
            if (sp[l] == '\0')
                return 0; // invalid syntax
            j++; l++;
            is_char_ok = is_neg_class ^ found_in_class;
        } else { // any or regular character
            is_char_ok = (sp[l] == '.' || !compare_chars(sp, l, chars, k));
            j++; l += utf8_char_length(sp[l]);
        }
        i++; k += utf8_char_length(chars[k]);
        break;
    }
}

int
mark_fields(const char *chars, int sx, int n)
{
    // search for fields
    // return number of fields

    int f, a;           // number of fields, generic
    int st, mst;        // indexes (characters, bytes) of running field start
    int i, k;           // indexes (characters, bytes) of chars

    // init fields and indexes
    k = get_str_index(chars, i = sx);
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
    while (i < sx + n && f < 10) {
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
