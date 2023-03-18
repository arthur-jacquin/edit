static char utf8_start[4] = {0, 0xc0, 0xe0, 0xf0};
static char masks[4] = {0x7f, 0x1f, 0x0f, 0x07};

int
is_word_char(char c)
{
    // check if c is the start of a word character

    return isalpha(c) || (c == '_') || (c == (char) 0xc3);
}

int
utf8_char_length(char c)
{
    // compute the length in bytes of UTF8 character starting by byte c

    if ((char) (c & (char) 0x80) == utf8_start[0]) {
        return 1;
    } else if ((char) (c & (char) 0xc0) == utf8_start[1]) {
        return 2;
    } else if ((char) (c & (char) 0xe0) == utf8_start[2]) {
        return 3;
    } else if ((char) (c & (char) 0xf0) == utf8_start[3]) {
        return 4;
    } else {
        exit(ERR_UNICODE_OR_UTF8);
        return ERR_UNICODE_OR_UTF8;
    }
}

int
unicode_char_length(uint32_t c)
{
    // compute the length in bytes of Unicode codepoint c

    if (c < 0x80) {
        return 1;
    } else if (c < 0x800) {
        return 2;
    } else if (c < 0x10000) {
        return 3;
    } else if (c < 0x200000) {
        return 4;
    } else {
        exit(ERR_UNICODE_OR_UTF8);
        return ERR_UNICODE_OR_UTF8;
    }
}

uint32_t
unicode(const char *chars, int k, int len)
{
    // compute the Unicode codepoint associated with UTF-8 encoded char in chars

    uint32_t res;
    int j;

    res = masks[len - 1] & chars[k++];
    for (j = 1; j < len; j++) {
        res <<= 6;
        res |= chars[k++] & (char) 0x3f;
    }

    return res;
}

void
insert_utf8(char *chars, int k, int len, uint32_t c)
{
    // insert Unicode codepoint c as UTF-8 in chars at index k

    int j;

    for (j = len - 1; j > 0; j--) {
        chars[k + j] = (c & 0x3f) | 0x80;
        c >>= 6;
    }
    chars[k] = c | utf8_start[len - 1];
}

int
get_str_index(const char *chars, int x)
{
    // compute the index in chars corresponding to column x

    int i, k;

    for (i = k = 0; i < x; i++)
        k += utf8_char_length(chars[k]);

    return k;
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

void *
_malloc(int size)
{
    // wrap a malloc call with error detection

    void *res;

    if ((res = malloc(size)) == NULL) {
        tb_shutdown();
        exit(ERR_MALLOC);
    }

    return res;
}
