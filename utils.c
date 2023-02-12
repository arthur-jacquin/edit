int
utf8_char_length(char c)
{
    // compute the length in bytes of character starting by byte c

    if ((c & 0x80) == 0) {
        return 1;
    } else if ((c & 0xe0) == 0xc0) {
        return 2;
    } else if ((c & 0xf0) == 0xe0) {
        return 3;
    } else if ((c & 0xf8) == 0xf0) {
        return 4;
    } else {
        return 0;
    }
}

int
get_str_index(struct line *l, int x)
{
    // compute the index in chars corresponding to column x

    int i, k;

    for (k = 0, i = 0; i < x; i++)
        k += utf8_char_length(l->chars[k]);

    return k;
}

void
insert_unicode(char *old, char *chars, int dx, uint32_t c)
{
    // insert unicode codepoint in an utf-8 string, after dx characters
    // old == chars is possible

    int len, i, j, k, cl, old_len;
    uint32_t first;

    // compute length of inserted character
    if (c < 0x80) {
        first = 0;
        len = 1;
    } else if (c < 0x800) {
        first = 0xc0;
        len = 2;
    } else if (c < 0x10000) {
        first = 0xe0;
        len = 3;
    } else if (c < 0x200000) {
        first = 0xf0;
        len = 4;
    } else { // Unicode error
        return;
    }
    old_len = strlen(old) + 1;

    // compute number of untouched bytes, copy them
    for (k = 0, i = 0; i < dx; i++) {
        cl = utf8_char_length(old[k]);
        for (j = 0; j < cl; j++)
            chars[k + j] = old[k + j];
        k += utf8_char_length(old[k]);
    }

    // copy bytes after new character
    for (j = old_len - 1; j >= k; j--)
        chars[j + len] = old[j];

    // insert (unicode encoded) character as UTF-8 in new_chars
    for (j = len - 1; j > 0; j--) {
        chars[k + j] = (c & 0x3f) | 0x80;
        c >>= 6;
    }
    chars[k] = c | first;
}

void
delete_unicode(char *old, char *chars, int dx)
{
    // delete dx-th displayable character in utf-8 string
    // old == chars is possible

    int len, i, j, k;

    // copy first dx - 1 characters
    for (k = 0, i = 0; i < dx - 1; i++) {
        len = utf8_char_length(old[k]);
        for (j = 0; j < len; j++)
            chars[k + j] = old[k + j];
        k += len;
    }

    // copy remaining characters
    len = utf8_char_length(old[k]);
    while (chars[k] = old[k + len])
        k++;
}

int
is_blank(char c)
{
    // check if c is a blank charater

    return (c == ' ');
}

int
is_word_char(char c)
{
    // check if c is a word character

    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || (c == '_');
}

int
is_digit(char c)
{
    // check if c is a digit

    return ('0' <= c && c <= '9');
}

int
is_in(const char *list, const char *chars, int x, int length)
{
    // check if chars substring starting at position x and of length length is
    // in space-separated list of words in list

    int i, j, ok;
    char c;

    i = j = 0;
    ok = 1;
    while (c = list[i++]) {
        if (c == ' ') {
            if (ok && j == length) {
                return 1;
            } else {
                j = 0;
                ok = 1;
            }
        } else if (c != chars[x+(j++)]) {
            ok = 0;
        }
    }

    return 0;
}
