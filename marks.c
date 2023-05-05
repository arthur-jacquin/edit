// see LICENSE file for copyright and license details

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
eat_pattern_character(const char *sp, int *j, int *l)
{
    // move *j, *l indexes of sp after the character pattern
    // return 1 on success

    if (sp[*l] == '\\' && strchr("\\^$|()*+?{[.dDwW", sp[*l+1])) {
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

    return 1;
}

static int
eat_pattern_atom(const char *sp, int *j, int *l)
{
    // move *j, *l indexes of sp after the atom pattern
    // return 1 on success

    int min, max;

    if (sp[*l] == '^' || sp[*l] == '$') {
        (*j)++; (*l)++;
    } else if (sp[*l] == '\\' && strchr("AZbB", sp[*l+1])) {
        (*j) += 2; (*l) += 2;
    } else {
        if (!eat_pattern_character(sp, j, l))
            return 0;
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

    if (sp[*l] == '(') {
        while (sp[*l] != ')') {
            if (sp[*l] == '\0' || !eat_pattern_atom(sp, j, l))
                return 0;
            while (sp[*l] == '|') {
                (*j)++; (*l)++;
                if (sp[*l] == '\0' || !eat_pattern_atom(sp, j, l))
                    return 0;
            }
        }
        (*j)++; (*l)++;
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

    char *sp;           // search pattern
    int j, l, lsp;      // indexes (characters, bytes), memory length of sp
    int i, k;           // indexes (characters, bytes) of chars
    int s, a;           // number of subpatterns, generic

    // states names match cheatsheet.md wording. possible transitions below
    enum states { READ_PATTERN, READ_BLOCK, BLOCK_READ, READ_GROUP, GROUP_READ,
        READ_ATOM, ATOM_READ, READ_CHAR, CHAR_READ };
    int state;

    // ORed elements
    int is_block_ok, start_block_i;
    int is_atom_ok, start_atom_i, in_group;

    // repeated elements
    int is_group_ok, nb_group, start_group_j, start_group_i;
    int is_char_ok, nb_char, start_char_j, start_char_i, found, is_neg_class;
    int min, max;

    // IN -> READ_PATTERN -> OUT    (j, l), (i, k), state
    //         |       ^
    //         V       |
    // READ_BLOCK --> BLOCK_READ    start_block_i, is_block_ok
    //   |     |       ^     ^
    //   |     |       |     |
    //   |     | GROUP_READ  |      start_group_{j,i}, is_group_ok, nb_group
    //   |     |   |   ^     |
    //   |     V   V   |     |
    //   |    READ_GROUP     |
    //   |     |       ^     |
    //   V     V       |     |
    //  READ_ATOM --> ATOM_READ     in_group, start_atom_i, is_atom_ok
    //      |             ^
    //      V             |
    //  READ_CHAR <-> CHAR_READ     start_char_{j,i}, is_char_ok, nb_char

    // init variables
    state = READ_PATTERN;
    lsp = strlen(sp = search_pattern.current);
    l = j = 0;
    s = 0;

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
            return i - sx; // correct read, return read length
        start_block_i = i;
        state = READ_BLOCK;
        break;

    case READ_BLOCK:
        is_block_ok = 1;
        if (l == lsp) {
            return 0; // invalid syntax
        } else if (sp[l] == '(') {
            j++; l++;
            s++;
            is_group_ok = 1;
            subpatterns[s].st = start_group_i = i;
            subpatterns[s].mst = k;
            start_group_j = j;
            nb_group = 0;
            state = READ_GROUP;
        } else {
            in_group = 0;
            start_atom_i = i;
            state = READ_ATOM;
        }
        break;

    case BLOCK_READ:
        if (is_block_ok) {
            while (sp[l] == '|') { // eat following ORed blocks
                j++; l++;
                if (sp[l] == '\0' || !eat_pattern_block(sp, &j, &l))
                    return 0; // syntax error
            }
            state = READ_PATTERN;
        } else if (sp[l] == '|') { // another try
            j++; l++;
            decrement(chars, &i, &k, start_block_i);
            state = READ_BLOCK;
        } else {
            return 0; // essential block is invalid
        }
        break;

    case READ_GROUP:
        if (l == lsp) {
            return 0; // invalid syntax
        } else if (sp[l] == ')') {
            j++; l++;
            nb_group++;
            state = GROUP_READ;
        } else {
            in_group = 1;
            start_atom_i = i;
            state = READ_ATOM;
        }
        break;

    case GROUP_READ:
        if (!parse_rep(sp, &j, &l, &min, &max)) // compute min and max
            return 0; // invalid syntax
        if (!is_group_ok) { // cancelling read
            if (nb_group - 1 < min)
                is_block_ok = 0;
            decrement(chars, &i, &k, start_group_i);
        } else if (!max || nb_group < max) { // another read
            is_group_ok = 1;
            start_group_i = i;
            decrement(sp, &j, &l, start_group_j);
            state = READ_GROUP;
            break;
        }
        subpatterns[s].n = i - subpatterns[s].st;
        subpatterns[s].mn = k - subpatterns[s].mst;
        state = BLOCK_READ;
        break;

    case READ_ATOM:
        // is_atom_ok must be attributed
        if (l == lsp) {
            return 0; // invalid syntax
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
            is_atom_ok = 1;
            start_char_i = i;
            start_char_j = j;
            nb_char = 0;
            state = READ_CHAR;
            break;
        }
        state = ATOM_READ;
        break;

    case ATOM_READ:
        if (in_group) {
            if (is_atom_ok) {
                while (sp[l] == '|') { // eat followind ORed atoms
                    j++; l++;
                    if (sp[l] == '\0' || !eat_pattern_atom(sp, &j, &l))
                        return 0; // invalid syntax
                }
            } else if (sp[l] == '|') { // another try
                j++; l++;
                decrement(chars, &i, &k, start_atom_i);
            } else {
                is_group_ok = 0;
            }
            state = READ_GROUP;
        } else {
            is_block_ok = is_atom_ok;
            state = BLOCK_READ;
        }
        break;

    case READ_CHAR:
        // is_char_ok must be attributed
        if (l == lsp) {
            return 0; // invalid syntax
        } else if (i == sx + n) {
            is_char_ok = 0;
            eat_pattern_character(sp, &j, &l);
            state = CHAR_READ;
            break;
        } if (sp[l] == '\\') {
            if (strchr("\\^$|()*+?{[.", sp[l+1])) { // escaped character
                is_char_ok = (sp[l+1] == chars[k]);
            } else if (sp[l+1] == 'd' || sp[l+1] == 'D') { // [non] digit
                is_char_ok = (sp[l+1] == 'D') ^ isdigit(chars[k]);
            } else if (sp[l+1] == 'w' || sp[l+1] == 'W') { // [non] word char.
                is_char_ok = (sp[l+1] == 'W') ^ is_word_char(chars[k]);
            } else {
                return 0; // invalid syntax
            }
            j += 2; l += 2;
        } else if (sp[l] == '[') { // custom class
            found = is_neg_class = 0;
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
                        found = 1;
                    j += 3; l += a + 1 + utf8_char_length(sp[l+a+1]);
                } else { // raw comparison
                    if (!compare_chars(sp, l, chars, k))
                        found = 1;
                    j++; l += a;
                }
            }
            if (sp[l] == '\0')
                return 0; // invalid syntax
            is_char_ok = is_neg_class ^ found;
            j++; l++;
        } else { // any or regular character
            is_char_ok = (sp[l] == '.' || (!compare_chars(sp, l, chars, k) &&
                !strchr("\\^$|()*+?{[.", sp[l])));
            j++; l += utf8_char_length(sp[l]);
        }
        i++; k += utf8_char_length(chars[k]);
        nb_char++;
        state = CHAR_READ;
        break;

    case CHAR_READ:
        if (!parse_rep(sp, &j, &l, &min, &max)) // compute min and max
            return 0; // invalid syntax
        if (!is_char_ok) { // cancelling read
            if (nb_char - 1 < min)
                is_atom_ok = 0;
            decrement(chars, &i, &k, start_char_i);
        } else if (!max || nb_char < max) { // another read
            start_char_i = i;
            decrement(sp, &j, &l, start_char_j);
            state = READ_CHAR;
            break;
        }
        state = ATOM_READ;
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
        } else if (chars[k] == '\\') {
            i++; k++;
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
